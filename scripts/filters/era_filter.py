import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import bilinear
from scripts.circuits.vmt.era_knob import GAIN_DB, FREQUENCY


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
    # Q1 = 0.15 + era_position * 0.05
    Q1 = 0.7 + era_position * 0.1
    Q2 = 0.6 + era_position * 0.5
    wc = 3000
    # Simple version
    # a2 = 4.7e-7 * (era_position + 0.1)
    # a1 = 2.10e-4
    a2 = (wc**-2) * (era_position + 0.1)
    a1 = (a2**0.5) / Q2
    a0 = 1.0

    # b2 = 5.17e-8
    # b2 = 19 * (5.5 ** (1 - era_position)) * a2
    # b1 = 6.8e-4
    # b0 = 1.0
    b2 = 19 * (5.5 ** (1 - era_position)) * a2
    b1 = b2**0.5 / Q1
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
    bb0 = B0 * norm
    bb1 = B1 * norm
    bb2 = B2 * norm
    aa0 = 1.0
    aa1 = A1 * norm
    aa2 = A2 * norm

    mix = 1.0

    b0m = (1.0 - mix) * aa0 + mix * bb0
    b1m = (1.0 - mix) * aa1 + mix * bb1
    b2m = (1.0 - mix) * aa2 + mix * bb2

    a = [aa0, aa1, aa2]
    b = [b0m, b1m, b2m]
    return b, a


if __name__ == "__main__":
    fs = 96000  # Sampling frequency in Hz

    fig, ax = plt.subplots(1, 1, figsize=(10, 8), sharex=True)
    fig.suptitle("Bode Diagram of the Peak Filter", fontsize=16)
    ax.set_ylabel("Magnitude (dB)")
    ax.grid(which="both", linestyle="--", linewidth=0.5)
    ax.set_title("Magnitude Response")
    ax.set_xlim(5, fs / 2)

    # ax.semilogx(f2, m2, label="High Shelf Filter")
    for i in [0, 4]:
        pos = i / 4.0
        b, a = create_era_filter(fs, era_position=pos)
        f, m = plot_bode(ax, b, a, fs)

        b2, a2 = create_high_shelf(
            fc=500 - (i / 4) * 180, G=-40 + 12 * (i / 4), Q=0.7, fs=fs
        )
        f2, m2 = plot_bode(ax, b2, a2, fs)

        ax.semilogx(f2, m2, label=f"Classic shelf ({i})", linewidth=2)
        ax.semilogx(f, m, label=f"Era Response ({i})", linewidth=2)

        # gain = np.array(GAIN_DB[i]) - 1.05 + 110
        # ax.semilogx(FREQUENCY, gain, label="Target Gain dB", linestyle="--")

    plt.legend()
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
