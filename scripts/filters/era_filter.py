import numpy as np
import matplotlib.pyplot as plt


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
    # mid scoop
    fc = 750 + (1 - era_position) * (2500 - 750)
    G = -4.0 * era_position
    Q = 0.5 + 0.5 * era_position 
    b1, a1 = create_peak_filter(fc, G, Q, fs)

    fc2 = 250 - era_position * 100
    G2 = -32.0 + 10.0 * era_position
    Q2 = 0.6
    b2, a2 = create_high_shelf(fc2, G2, Q2, fs)

    return b1, a1, b2, a2


if __name__ == "__main__":
    fs = 48000  # Sampling frequency in Hz

    # Parameters for era turned to minimum
    # fc = 1200
    # G = -4.0
    # Q = 0.5
    # b1, a1 = create_peak_filter(fc, G, Q, fs)
    #
    # fc2 =1200
    # G2 = -18.0
    # Q2 = 0.7
    # b2, a2 = create_high_shelf(fc2, G2, Q2, fs)

    # Parameters for era turned to maximum
    # fc = 700
    # G = -8.0
    # Q = 0.5
    # b1, a1 = create_peak_filter(fc, G, Q, fs)
    #
    # fc2 = 500
    # G2 = -8.0
    # Q2 = 0.7
    # b2, a2 = create_high_shelf(fc2, G2, Q2, fs)

    fig, ax = plt.subplots(1, 1, figsize=(10, 8), sharex=True)
    fig.suptitle("Bode Diagram of the Peak Filter", fontsize=16)
    ax.set_ylabel("Magnitude (dB)")
    ax.grid(which="both", linestyle="--", linewidth=0.5)
    ax.set_title("Magnitude Response")
    ax.set_xlim(5, fs / 2)

    for i in [0, 3, 6, 10]:
        pos = i / 10
        b1, a1, b2, a2 = create_era_filter(fs, era_position=pos)
        f2, m2 = plot_bode(ax, b2, a2, fs)
        f1, m1 = plot_bode(ax, b1, a1, fs)
        ax.semilogx(f2, m2 + m1, label=f"Era Response ({pos})", linewidth=2)

    plt.legend()
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.show()
