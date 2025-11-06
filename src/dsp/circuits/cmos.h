#pragma once

#include <algorithm>
#include <array>
#include <cfloat>
#include <juce_dsp/juce_dsp.h>
#include <utility>

class CMOS
{
  public:
    CMOS()
    {
    }
    void reset()
    {
    }

    std::pair<float, float> pmos(float, float);
    std::pair<float, float> nmos(float, float);

    float waveshaper_cmos(float);

    void prepare();
    float processSample(float);

    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

  private:
    static constexpr size_t lut_plus_size = 8192;
    static constexpr size_t lut_minus_size = 4096;
    static constexpr float lut_min_input = -1.8f;
    static constexpr float lut_max_input = 5.1f;
    std::array<float, lut_minus_size + lut_plus_size - 1> waveshaper_lut;

    static constexpr float n_vtc1 = 1.208306917691355f;
    static constexpr float n_vtc2 = 0.3139084341943607f;
    static constexpr float n_alpha1 = 0.020662094888127674f;
    static constexpr float n_alpha2 = -0.0017181795239085821f;

    static constexpr float p_vtc1 = -0.25610349392710086f;
    static constexpr float p_vtc2 = 0.27051216771368214f;
    static constexpr float p_alpha1 = -0.0003577445606469842f;
    static constexpr float p_alpha2 = -0.0008620153809796321f;
    static constexpr float p_alpha3 = -0.00016848836814836602f;
    static constexpr float p_alpha4 = -1.0800821774906936e-5f;

    static constexpr float bias = 3.45f;
    static constexpr float v_dd = 9.0f;
    static constexpr float delta = 0.06f;

    static constexpr float NR_EPSILON = FLT_MIN;
};

inline std::pair<float, float> CMOS::nmos(float vgs, float vds)
{
    float vt = n_vtc2 * vgs + n_vtc1;
    float alpha = n_alpha2 * vgs + n_alpha1;
    float vgs_minus_vt = vgs - vt;

    // 1. Triode Region
    float ids_tri = alpha * (vgs_minus_vt - vds * 0.5f) * vds;
    float gds_tri = alpha * (vgs_minus_vt - vds);

    // 2. Saturation Region
    float vgs_vt_sq = vgs_minus_vt * vgs_minus_vt;
    float ids_sat = 0.5f * alpha * vgs_vt_sq;
    float gds_sat = 0.0f;

    // --- Selection ---
    bool is_triode = (vds <= vgs_minus_vt);
    float ids_tri_or_sat = is_triode ? ids_tri : ids_sat;
    float gds_tri_or_sat = is_triode ? gds_tri : gds_sat;

    bool is_cutoff = (vgs_minus_vt <= 0.0f);
    float ids = is_cutoff ? 0.0f : ids_tri_or_sat;
    float gds = is_cutoff ? 0.0f : gds_tri_or_sat;

    return {ids, gds};
}

inline std::pair<float, float> CMOS::pmos(float vgs, float vds)
{
    float alpha =
        p_alpha1 + vgs * (p_alpha2 + vgs * (p_alpha3 + vgs * p_alpha4));
    float vt = p_vtc1 + p_vtc2 * vgs;
    float vgs_minus_vt = vgs - vt;

    // 1. Triode Region
    float vds_factor = (1.0f - delta * vds);
    float ids_tri = -alpha * (vgs_minus_vt - vds * 0.5f) * vds * vds_factor;
    float gds_tri =
        -alpha * (1.5f * delta * vds * vds -
                  (2.0f * delta * vgs_minus_vt + 1.0f) * vds + vgs_minus_vt);

    // 2. Saturation Region
    float vgs_vt_sq = vgs_minus_vt * vgs_minus_vt;
    float ids_sat = -0.5f * alpha * vgs_vt_sq * vds_factor;
    float gds_sat = 0.5f * alpha * delta * vgs_vt_sq;

    // --- Selection ---
    bool is_triode = (vds >= vgs_minus_vt);
    float ids_tri_or_sat = is_triode ? ids_tri : ids_sat;
    float gds_tri_or_sat = is_triode ? gds_tri : gds_sat;

    bool is_cutoff = (vgs >= vt);
    float ids = is_cutoff ? 0.0f : ids_tri_or_sat;
    float gds = is_cutoff ? 0.0f : gds_tri_or_sat;

    return {ids, gds};
}

inline void CMOS::prepare()
{
    float h_minus = -lut_min_input / (lut_minus_size - 1);
    for (size_t i = 0; i < lut_minus_size; i++)
    {
        float t = 1.0f - float(i) / (lut_minus_size - 1);
        float mapped_t = t * t;
        float x = mapped_t * h_minus;
        waveshaper_lut[i] = waveshaper_cmos(x);
    }

    float h_plus = lut_max_input / (lut_plus_size - 1);
    for (size_t i = 0; i < lut_plus_size - 1; i++)
    {
        float t = float(i) / (lut_plus_size - 1);
        float mapped_t = t * t;
        float x = mapped_t * h_plus;
        waveshaper_lut[lut_minus_size + i - 1] = waveshaper_cmos(x);
    }

    // Ensure exact value at x = 0
    waveshaper_lut[lut_minus_size - 1] = 0.0f;
}

inline float CMOS::waveshaper_cmos(float x)
{
    float vin = x + bias;
    float vout = bias;

    for (int i = 0; i < 5; i++)
    {
        float vgs_n = vin;
        float vds_n = vout;

        float vgs_p = vin - v_dd;
        float vds_p = vout - v_dd;

        auto [ids_n, gds_n] = nmos(vgs_n, vds_n);
        auto [ids_p, gds_p] = pmos(vgs_p, vds_p);

        float f_x = ids_n + ids_p;

        float f_prime_x = gds_n + gds_p;

        vout = vout - f_x / (f_prime_x + NR_EPSILON);

        vout = std::clamp(vout, 0.0f, v_dd);
    }
    return 1.0f - 2.0f * vout / v_dd;
}

// inline float CMOS::processSample(float x)
// {
//     return waveshaper_cmos(x);
// }

inline float CMOS::processSample(float x)
{
    const size_t lut_size = lut_minus_size + lut_plus_size - 1;

    float tn = (x - lut_min_input) / (-lut_min_input);
    float idxn = (lut_minus_size - 1) * tn * tn;

    float tp = x / lut_max_input;
    float idxp = (lut_plus_size - 1) * tp * tp;

    float idx = (x < 0.0f) ? idxn : lut_minus_size - 1 + idxp;

    size_t i0 = size_t(idx);
    size_t i1 = std::min(i0 + 1, lut_size - 1);
    float t = idx - float(i0);

    float y0 = waveshaper_lut[i0];
    float y1 = waveshaper_lut[i1];

    return y0 + (y1 - y0) * t;
}

inline void CMOS::process(
    const juce::dsp::ProcessContextReplacing<float>& context
)
{
    auto& block = context.getOutputBlock();
    const size_t num_samples = block.getNumSamples();

    auto* ch = block.getChannelPointer(0);
    for (size_t i = 0; i < num_samples; ++i)
    {
        ch[i] = processSample(ch[i]);
    }
}
