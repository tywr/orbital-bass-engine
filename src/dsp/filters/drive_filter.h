inline std::shared_ptr<juce::dsp::IIR::Coefficients<float>> makeDriveFilter(
    float fs, float f_hpf, float f_lpf, float gain
)
{
    // Creates a broad bandpass filter with a gain boost in the passband
    
    const float pi = juce::MathConstants<float>::pi;

    const float tau_hpf = 1.0f / (2 * pi * f_hpf);
    const float tau_lpf = 1.0f / (2 * pi * f_lpf);

    const float a2s = tau_hpf * tau_lpf;
    const float a1s = tau_hpf + tau_lpf;
    const float a0s = 1.0f;

    const float b2s = tau_hpf * tau_lpf;
    const float b1s = tau_hpf * (1.0f + gain) + tau_lpf;
    const float b0s = 1.0f;
    const float K = 2.0f * fs;
    const float Ks = K * K;

    float a0 = a2s * Ks + a1s * K + a0s;
    float a1 = 2.0f * (a0s - a2s * Ks) / a0;
    float a2 = (a2s * Ks - a1s * K + a0s) / a0;

    float b0 = (b2s * Ks + b1s * K + b0s) / a0;
    float b1 = 2.0f * (b0s - b2s * Ks) / a0;
    float b2 = (b2s * Ks - b1s * K + b0s) / a0;

    return std::make_shared<juce::dsp::IIR::Coefficients<float>>(b0, b1, b2, 1.0f, a1, a2);
}
