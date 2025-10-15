import os
import ctypes

import numpy as np

lib = ctypes.CDLL("scripts/bin/omega.so")

lib.omega4_py.argtypes = [ctypes.c_float]
lib.omega4_py.restype = ctypes.c_float


def omega_small(x):
    """
    Approximates the Wright Omega function for small values of x using a
    5th-order Taylor series expansion.
    """
    c0 = 0.5671432904097838
    c1 = 0.3618963236098023
    c2 = 0.0736778463779836
    c3 = -0.0013437346889135
    c4 = -0.0016355437889344
    c5 = 0.0002166542734346

    return c0 + c1 * x + c2 * x**2 + c3 * x**3 + c4 * x**4 + c5 * x**5


def omega(x: float) -> float:
    if x > 1.5 or x < -1.5:
        return lib.omega4_py(x)
    else:
        # return lib.omega4_py(x)
        return omega_small(x)


class DiodeClipper:
    # State variables
    buffer_size = 16

    # Diode parameters

    # Silicon diode
    # c = 1e-8
    # re = 2_200
    # i_s = 200e-9
    # v_t = 0.02585

    # Germanium diode
    c = 5e-8
    re = 500
    i_s = 5e-6
    v_t = 0.02585

    # Discretization coefficients

    def __init__(self, sample_rate, side="both"):
        self.sample_rate = sample_rate
        self.side = side

        self.b0 = 2 / sample_rate
        self.b1 = -2 / sample_rate
        self.a1 = -1

        crb_1 = self.c * self.re + self.b0 + 1
        self.k1 = 1 / (self.c * self.re)
        self.k2 = (self.c * self.re) / crb_1
        self.k3 = (self.i_s * self.re) / crb_1
        self.k4 = 1 / self.v_t
        self.k5 = np.log((self.i_s * self.re) / crb_1 * self.v_t)
        self.k6 = self.b1 - self.a1 * self.b0

        self.prev_v = 1
        self.prev_p = self.k6 * self.prev_v

    def process(self, input_signal: np.array):
        output = np.zeros_like(input_signal)
        for i in range(0, len(input_signal), self.buffer_size):
            buffer = input_signal[i : i + self.buffer_size]
            output[i : i + self.buffer_size] = self.process_buffer(buffer)
        return output

    def process_buffer(self, buffer: np.array):
        return np.array([self.process_sample(sample) for sample in buffer])

    def process_sample(self, s: float):
        vin = s  # Input sample value
        if abs(vin) < 0.1:
            p = self.k6 * vin - self.a1 * self.prev_p
            self.prev_p = p
            return vin
        if self.side == "up" and vin < 0:
            return vin
        elif self.side == "down" and vin > 0:
            return vin
        else:
            q = self.k1 * vin - self.prev_p
            r = np.sign(q)
            w = self.k2 * q + self.k3 * r
            vout = w - self.v_t * r * omega(self.k4 * r * w + self.k5)
            p = self.k6 * vout - self.a1 * self.prev_p
            self.prev_p = p
            return vout


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    sample_rate = 88200
    frequency = 440
    duration = 5 / frequency
    n_samples = int(duration * sample_rate)

    t = np.linspace(0, duration, n_samples, endpoint=False)
    y = 1 * np.sin(2 * np.pi * frequency * t)

    diode_pair = DiodeClipper(sample_rate)
    hard_clipped = diode_pair.process(y)

    plt.plot(
        t,
        hard_clipped,
        label="output",
    )
    plt.legend()
    plt.show()

    # Y = np.fft.fft(y)
    # Y_clipped = np.fft.fft(y_clipped)
    # xf = np.fft.fftfreq(n_samples, 1 / sample_rate)
    #
    # Y_norm = 2 / n_samples * np.abs(Y[0 : n_samples // 2])
    # Y_clipped_norm = 2 / n_samples * np.abs(Y_clipped[0 : n_samples // 2])
    # xf_pos = xf[0 : n_samples // 2]
    #
    # plt.plot(xf_pos, Y_norm)
    # plt.plot(xf_pos, Y_clipped_norm)
    # plt.xlim(0, 8000)
    # plt.show()
