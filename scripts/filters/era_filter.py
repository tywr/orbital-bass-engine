import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import bilinear


def create_peak_filter(fc, G, Q, fs):
    """
    Creates the coefficients for a digital peak (peaking EQ) filter.

    Args:
        fc (float): The center frequency of the filter in Hz.
        G (float): The gain of the filter in dB.
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

    # Coefficients for the transfer function H(z) = (b0 + b1*z^-1 + b2*z^-2) / (a0 + a1*z^-1 + a2*z^-2)
    # These formulas are from the "Audio EQ Cookbook" by Robert Bristow-Johnson
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
    freq = np.logspace(np.log10(5), np.log10(fs / 2 - 1), n_points)

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


def create_era_filter(fs, era_position=0):
    a2 = 4.7e-8 * era_position + 4.7e-9
    a1 = 2.10e-4
    a0 = 1.0

    b2 = 5.17e-8
    b1 = 6.8e-4
    b0 = 1.0

    T = 1.0 / fs
    K = 2.0 / T

    A0 = b2 * K * K + b1 * K + b0
    A1 = 2.0 * (b0 - b2 * K * K)
    A2 = b2 * K * K - b1 * K + b0

    B0 = a2 * K * K + a1 * K + a0
    B1 = 2.0 * (a0 - a2 * K * K)
    B2 = a2 * K * K - a1 * K + a0

    norm = 1.0 / A0

    a = [B0 * norm, B1 * norm, B2 * norm]
    b = [1.0, A1 * norm, A2 * norm]

    fc = 150
    G = -30
    a2, b2 = create_high_shelf(fc=fc, G=G, Q=0.6, fs=fs)
    return a, b, a2, b2


if __name__ == "__main__":
    fs = 48000  # Sampling frequency in Hz

    fig, ax = plt.subplots(1, 1, figsize=(10, 8), sharex=True)
    fig.suptitle("Bode Diagram of the Peak Filter", fontsize=16)
    ax.set_ylabel("Magnitude (dB)")
    ax.grid(which="both", linestyle="--", linewidth=0.5)
    ax.set_title("Magnitude Response")
    ax.set_xlim(5, fs / 2)

    for i in [1.0]:
        pos = i
        b, a, b2, a2 = create_era_filter(fs, era_position=pos)
        b3, a3 = create_peak_filter(fc=700, G=-10, Q=0.6, fs=fs)
        f, m = plot_bode(ax, b, a, fs)
        f3, m3 = plot_bode(ax, b3, a3, fs)
        ax.semilogx(f, m, label=f"Era Response ({pos})", linewidth=2)
        ax.semilogx(f3, m3, label=f"Peak Filter ({pos})", linewidth=2)

    plt.legend()
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
