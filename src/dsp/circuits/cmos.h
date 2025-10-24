#pragma once

#include <algorithm>
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

    using SIMD_Float = juce::dsp::SIMDRegister<float>;

    float processSample(float);
    std::pair<float, float> pmos(float, float);
    std::pair<float, float> nmos(float, float);

    std::pair<SIMD_Float, SIMD_Float> nmos_vec(SIMD_Float vgs, SIMD_Float vds);
    std::pair<SIMD_Float, SIMD_Float> pmos_vec(SIMD_Float vgs, SIMD_Float vds);
    void processBlock(juce::AudioBuffer<float>& buffer, int numSamples);

  private:
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

inline float CMOS::processSample(float x)
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
