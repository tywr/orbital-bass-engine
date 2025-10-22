import numpy as np
import ctypes


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
    if np.abs(x) > 0.5:
        return lib.omega4_py(x)
    else:
        return omega_small(x)


class OpAmp:
    buffer_size = 16

    def __init__(self):
        self.vp = 2
        self.vt = 0.026
        self.i_s = 0.1e-15
        self.beta_f = 100
        self.re = 1_000

    def process(self, input_signal: np.array):
        output = np.zeros_like(input_signal)
        for i in range(0, len(input_signal), self.buffer_size):
            buffer = input_signal[i : i + self.buffer_size]
            output[i : i + self.buffer_size] = self.process_buffer(buffer)
        return output

    def process_buffer(self, buffer: np.array):
        return np.array([self.process_sample(sample) for sample in buffer])

    def _process_sample(self, vin):
        exp_in = (vin - self.vp) / self.vt
        v_x = self.i_s * self.re * (np.exp(exp_in) + 1 / self.beta_f)
        v_out = self.vt * omega(((vin + v_x) / self.vt) + self.k) - v_x
        return v_out

    def knee(self, x, bias=1.0, vt=0.001):
        return 0.5 * (1 - np.tanh((x - bias) / vt))

    def process_sample(self, x):
        vt = 0.026
        C = 0.5 * vt * np.log(np.cosh(1 / vt))
        if x > 0:
            return 0.5 * x - 0.5 * vt * np.log(np.cosh((x - 1) / vt)) + C
        else:
            return 0.5 * x + 0.5 * vt * np.log(np.cosh((-x - 1) / vt)) - C


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    frequency = 880
    sample_rate = 44100
    duration = 5 / frequency
    n_samples = int(duration * sample_rate)

    amp = OpAmp()
    t = np.linspace(0, duration, n_samples, endpoint=False)
    # y = 5 * np.sin(2 * np.pi * frequency * t)
    # y_clipped = [amp.process_sample(v) for v in y]

    x = np.linspace(-3, 3, 10000)
    y = [amp.process_sample(v) for v in x]

    dy = np.gradient(y, x)

    plt.plot(x, y, color="red", label="output")
    # plt.plot(x, dy, color="blue", label="tanh")
    # plt.axhline(1, color="red", linestyle="--", label="unity gain")

    # plt.plot(x, y2, color="orange", label="omega")
    # plt.plot(x, x, color="blue", label="y=x")
    plt.ylim([-4, 4])
    plt.xlim([-4, 4])
    plt.legend()
    plt.show()
