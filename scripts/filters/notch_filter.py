import numpy as np
import scipy.signal as signal
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


def create_notch_filter(fc, Q, fs, A=1):
    """
    Creates the coefficients for a digital notch filter (biquad).

    Args:
        fc (float): The center frequency of the filter in Hz.
        Q (float): The Q-factor of the filter (defines the notch width).
        fs (int): The sampling frequency in Hz.
        A (float, optional): A linear gain factor. Defaults to 1.

    Returns:
        tuple: A tuple containing two numpy arrays (b, a) representing the
               numerator and denominator coefficients of the transfer function.
    """
    w0 = 2 * np.pi * fc / fs
    alpha = np.sin(w0) / (2 * Q)

    # These are the specific coefficients for a notch filter
    b0 = 1
    b1 = -2 * np.cos(w0)
    b2 = 1

    # The denominator (a coefficients) is the same as the high-pass
    a0 = 1 + alpha
    a1 = -2 * np.cos(w0)
    a2 = 1 - alpha

    # Normalize coefficients so that a0 = 1
    b = np.array([b0, b1, b2]) / a0
    a = np.array([a0, a1, a2]) / a0

    return A * b, a


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

    # b, a = create_notch_filter(fc=320, Q=0.18, fs=fs)
    # f, m = plot_bode(ax, b, a, fs)
    # plt.semilogx(f, m)

    b, a = create_peak_filter(fc=280, G=-32, Q=1.0, fs=fs)
    f, m = plot_bode(ax, b, a, fs)
    plt.semilogx(f, m)

    plt.legend()
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
