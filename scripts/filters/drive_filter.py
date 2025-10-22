import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from scipy.signal import bilinear


# from scripts.circuits.vmt.era_knob import GAIN_DB, FREQUENCY
#
def create_peak_filter(fc, G, Q, fs):
    """
    Creates the coefficients for a digital peak (or peaking EQ) filter.

    This filter boosts or cuts a specific frequency band around the center
    frequency.

    Args:
        fc (float): The center frequency of the filter in Hz.
        G (float): The gain at the peak in dB (can be positive or negative).
        Q (float): The Q-factor of the filter, which determines the bandwidth.
        fs (int): The sampling frequency in Hz.

    Returns:
        tuple: A tuple containing two numpy arrays (b, a) representing the
               numerator and denominator coefficients of the transfer function.
    """
    # Intermediate variables
    A = 10 ** (G / 40.0)
    w0 = 2 * np.pi * fc / fs
    alpha = np.sin(w0) / (2 * Q)

    # Coefficients for the transfer function, from the "Audio EQ Cookbook"
    b0 = 1 + alpha * A
    b1 = -2 * np.cos(w0)
    b2 = 1 - alpha * A
    a0 = 1 + alpha / A
    a1 = -2 * np.cos(w0)
    a2 = 1 - alpha / A

    # Normalize the coefficients so that a0 is 1
    b = np.array([b0, b1, b2]) / a0
    a = np.array([a0, a1, a2]) / a0

    return b, a


def create_high_pass(fc, Q, fs, A=1):
    """
    Creates the coefficients for a digital high-pass filter (biquad).

    Args:
        fc (float): The cutoff frequency of the filter in Hz.
        Q (float): The Q-factor of the filter (resonance).
        fs (int): The sampling frequency in Hz.

    Returns:
        tuple: A tuple containing two numpy arrays (b, a) representing the
               numerator and denominator coefficients of the transfer function.
    """
    w0 = 2 * np.pi * fc / fs
    alpha = np.sin(w0) / (2 * Q)

    b0 = (1 + np.cos(w0)) / 2
    b1 = -(1 + np.cos(w0))
    b2 = (1 + np.cos(w0)) / 2
    a0 = 1 + alpha
    a1 = -2 * np.cos(w0)
    a2 = 1 - alpha

    # Normalize coefficients so that a0 = 1
    b = np.array([b0, b1, b2]) / a0
    a = np.array([a0, a1, a2]) / a0

    return A * b, a


def create_high_shelf(fc, G, Q, fs):
    """
    Creates the coefficients for a digital high-shelf filter.

    Args:
        fc (float): The center frequency of the filter in Hz.
        G (float): The gain of the shelf in dB.
        Q (float): The Q-factor of the filter, which determines the transition steepness.
        fs (int): The sampling frequency in Hz.

    Returns:
        tuple: A tuple containing two numpy arrays (b, a) representing the
               numerator and denominator coefficients of the transfer function.
    """
    # Intermediate variables
    A = 10 ** (G / 40.0)
    w0 = 2 * np.pi * fc / fs
    alpha = np.sin(w0) / (2 * Q)

    # Coefficients for the transfer function, from the "Audio EQ Cookbook"
    b0 = A * ((A + 1) + (A - 1) * np.cos(w0) + 2 * np.sqrt(A) * alpha)
    b1 = -2 * A * ((A - 1) + (A + 1) * np.cos(w0))
    b2 = A * ((A + 1) + (A - 1) * np.cos(w0) - 2 * np.sqrt(A) * alpha)
    a0 = (A + 1) - (A - 1) * np.cos(w0) + 2 * np.sqrt(A) * alpha
    a1 = 2 * ((A - 1) - (A + 1) * np.cos(w0))
    a2 = (A + 1) - (A - 1) * np.cos(w0) - 2 * np.sqrt(A) * alpha

    # Normalize the coefficients so that a0 is 1
    b = np.array([b0, b1, b2]) / a0
    a = np.array([a0, a1, a2]) / a0

    return b, a


def create_generalized_filter(f_hpf, f_lpf, drive_setting, fs, max_gain=10.0):
    w_hpf = 2 * np.pi * f_hpf
    w_lpf = 2 * np.pi * f_lpf

    # Calculate time constants
    tau_hpf = 1 / w_hpf
    tau_lpf = 1 / w_lpf

    # Map the 0-1 drive_setting to the effective gain 'g'
    g = drive_setting * max_gain

    a2_s = tau_hpf * tau_lpf
    a1_s = tau_hpf + tau_lpf
    a0_s = 1.0
    den_s = np.array([a2_s, a1_s, a0_s])

    b2_s = tau_hpf * tau_lpf
    b1_s = tau_hpf * (1 + g) + tau_lpf
    b0_s = 1.0
    num_s = np.array([b2_s, b1_s, b0_s])

    num_z, den_z = signal.bilinear(num_s, den_s, fs)

    norm_factor = den_z[0]
    b = num_z / norm_factor
    a = den_z / norm_factor

    return b, a


def create_drive_filter(fc1, fc2, G, fs):
    f0 = np.sqrt(fc1 * fc2)
    Q = f0 / (fc1 + fc2)
    b, a = create_peak_filter(f0, G, Q, fs)
    return b, a


def cascade_filters(b1, a1, b2, a2):
    """
    Cascades two IIR filters by convolving their coefficients.

    Args:
        b1, a1 (np.array): Numerator and denominator of the first filter.
        b2, a2 (np.array): Numerator and denominator of the second filter.

    Returns:
        tuple: A tuple (b_cascaded, a_cascaded) for the combined filter.
    """
    b_cascaded = np.convolve(b1, b2)
    a_cascaded = np.convolve(a1, a2)
    return b_cascaded, a_cascaded


def plot_bode(ax, b, a, fs):
    """
    Calculates the frequency response of a filter and plots it on a Bode diagram.

    Args:
        b (np.array): The numerator coefficients of the filter.
        a (np.array): The denominator coefficients of the filter.
        fs (int): The sampling frequency in Hz.
    """
    # Create a frequency vector from 20 Hz to Nyquist frequency (fs/2)
    # Using a logarithmic scale for better visualization on the Bode plot
    n_points = 2048
    freq = np.logspace(np.log10(50), np.log10(fs / 2 - 1), n_points)

    # Corresponding angular frequency
    w = 2 * np.pi * freq / fs

    # Calculate the frequency response H(e^jw)
    # The term z represents e^(jw)
    z = np.exp(1j * w)

    # Evaluate the numerator and denominator polynomials at each frequency point
    # H(z) = (b0 + b1*z^-1 + b2*z^-2) / (a0 + a1*z^-1 + a2*z^-2)
    numerator = b[0] + b[1] * (z**-1) + b[2] * (z**-2)
    denominator = a[0] + a[1] * (z**-1) + a[2] * (z**-2)

    H = numerator / denominator

    # Calculate magnitude in dB
    magnitude_db = 20 * np.log10(np.abs(H))

    return freq, magnitude_db


if __name__ == "__main__":
    fs = 96000  # Sampling frequency in Hz

    fig, ax = plt.subplots(1, 1, figsize=(10, 8), sharex=True)
    fig.suptitle("Bode Diagram of the Peak Filter", fontsize=16)
    ax.set_ylabel("Magnitude (dB)")
    ax.grid(which="both", linestyle="--", linewidth=0.5)
    ax.set_title("Magnitude Response")
    ax.set_xlim(50, 10000)

    # for g in [3, 6, 9, 12, 15, 18]:
    #     fc = 100
    #     b2, a2 = create_high_shelf(fc=fc, Q=0.707, G=1, fs=fs)
    #     f2, m2 = plot_bode(ax, b2, a2, fs)
    #     ax.semilogx(f2, g * m2, label=f"High Shelf Filter {g} dB")

    # fc = 155
    # b, a = create_high_pass(fc=fc, Q=0.507, fs=fs, A=10)
    # b2, a2 = create_high_shelf(fc=fc, G=3, Q=0.507, fs=fs)
    #
    # f, m = plot_bode(ax, b, a, fs)
    # f2, m2 = plot_bode(ax, b2, a2, fs)
    #
    # for alpha in [0, 0.2, 0.4, 0.6, 0.8, 1.0]:
    #     ax.semilogx(f, (1 - alpha) * m2 + alpha * m, label=f"Mix α={alpha}")

    for alpha in [0.0, 0.01, 0.1, 0.5, 0.9, 0.99, 1.0]:
        drive_setting = alpha
        b, a = create_generalized_filter(
            f_hpf=100, f_lpf=5000, drive_setting=drive_setting, fs=fs, max_gain=10.0
        )
        # b, a = create_drive_filter(fc1=55, fc2=2200, G=drive_setting * 10.0, fs=fs)
        f, m = plot_bode(ax, b, a, fs)
        ax.semilogx(f, m, label=f"Drive Filter Setting: {alpha:.1f}")

    plt.legend()
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
