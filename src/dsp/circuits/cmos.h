#pragma once
#include <algorithm>
#include <tuple>

class CMOS
{
  public:
    CMOS()
    {
    }
    float processSample(float);
    std::tuple<float, float> pmos(float, float);
    std::tuple<float, float> nmos(float, float);

  private:
    float v0 = 0.34486360573756947f;

    float n_vtc1 = 1.208306917691355f;
    float n_vtc2 = 0.3139084341943607f;
    float n_alpha1 = 0.020662094888127674f;
    float n_alpha2 = -0.0017181795239085821f;

    float p_vtc1 = -0.25610349392710086f;
    float p_vtc2 = 0.27051216771368214f;
    float p_alpha1 = -0.0003577445606469842f;
    float p_alpha2 = -0.0008620153809796321f;
    float p_alpha3 = -0.00016848836814836602f;
    float p_alpha4 = -1.0800821774906936e-5f;

    float v_dd = 9.0f;
    float delta = 0.06f;

    float prev_v = 0.0f;
};

inline std::tuple<float, float> CMOS::nmos(float vgs, float vds)
{
    float vt = n_vtc2 * vgs + n_vtc1;
    float alpha = n_alpha2 * vgs + n_alpha1;

    if (vgs <= vt)
    {
        return {0.0f, 0.0f};
    }

    if ((vds <= (vgs - vt)) && vgs > vt)
    {
        float ids = alpha * (vgs - vt - vds / 2) * vds;
        float gds = alpha * (vgs - vt) - alpha * vds;
        return {ids, gds};
    }
    float ids = 0.5f * alpha * (vgs - vt) * (vgs - vt);
    float gds = 0.0f;
    return {ids, gds};
}

inline std::tuple<float, float> CMOS::pmos(float vgs, float vds)
{
    float alpha = p_alpha1 + p_alpha2 * vgs + p_alpha3 * vgs * vgs +
                  p_alpha4 * vgs * vgs * vgs;
    float vt = p_vtc1 + p_vtc2 * vgs;

    if (vgs >= vt)
    {
        return {0.0f, 0.0f};
    }

    if (vds >= (vgs - vt) && vgs < vt)
    {
        float ids = -alpha * (vgs - vt - vds / 2) * vds * (1.0f - delta * vds);
        float gds = -alpha * (3 * delta * (vds * vds) / 2.0f -
                              (2 * delta * (vgs - vt) + 1.0f) * vds + vgs - vt);
        return {ids, gds};
    }

    float ids =
        -0.5f * alpha * ((vgs - vt) * (vgs - vt)) * (1.0f - delta * vds);
    float gds = 0.5f * alpha * delta * (vgs - vt) * (vgs - vt);
    return {ids, gds};
}

inline float CMOS::processSample(float vin)
{
    if (vin >= 8.5f)
    {
        prev_v = 0.0f;
        return 0.0f;
    }
    float vout = prev_v;

    // Iterate a fixed number of times for stability and real - time safety
    for (int i = 0; i < 5; i++)
    {
        // NMOS:
        float vgs_n = vin;
        float vds_n = vout;

        // PMOS:
        float vgs_p = vin - v_dd;
        float vds_p = vout - v_dd;

        // Get currents and their derivatives
        auto [ids_n, gds_n] = nmos(vgs_n, vds_n);
        auto [ids_p, gds_p] = pmos(vgs_p, vds_p);

        // The function we want to find the root of(KCL)
        float f_x = ids_n + ids_p;

        // The derivative of the function w.r.t.vout
        float f_prime_x = gds_n + gds_p;

        // Newton - Raphson update step
        vout = vout - f_x / (f_prime_x + 1e-9f);

        vout = std::max(vout, 0.0f);
        vout = std::min(vout, v_dd);
    }
    prev_v = vout;
    return vout / v0;
}
