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
        print("Using small x approximation for omega")
        # return lib.omega4_py(x)
        return omega_small(x)


class BJT:
    buffer_size = 16

    def __init__(self, vp=9, vt=0.026, i_s=0.1e-15, beta_f=100, re=1_000):
        self.vp = vp
        self.vt = vt
        self.i_s = i_s
        self.beta_f = beta_f
        self.re = re
        self.k = np.log((self.i_s * self.re / self.vt) * (1 + 1 / self.beta_f))
        self.v_sat = 9.8
        self.y_sat = self._process_sample(self.v_sat)
        self.dy_sat = self.get_v_sat_derivative()

    def get_v_sat_derivative(self):
        dx = 1e-4
        dv = self._process_sample(self.v_sat + dx / 2) - self._process_sample(
            self.v_sat - dx / 2
        )
        return dv / dx

    def approximate_sat_function(self, vin):
        alpha = self.dy_sat / (self.vp - self.y_sat)
        return self.y_sat + (self.vp - self.y_sat) * np.tanh(alpha * (vin - self.v_sat))

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

    def process_sample(self, vin):
        if vin >= self.v_sat:
            return self.approximate_sat_function(vin)
        else:
            return self._process_sample(vin)


if __name__ == "__main__":
    import matplotlib.pyplot as plt

    frequency = 880
    sample_rate = 44100
    duration = 5 / frequency
    n_samples = int(duration * sample_rate)

    bjt = BJT()
    # t = np.linspace(0, duration, n_samples, endpoint=False)
    # y = 5 * np.sin(2 * np.pi * frequency * t)
    # y_clipped = [bjt.process_sample(4.5 + v) for v in y]

    x = np.linspace(-2, 12, n_samples)
    y = [bjt.process_sample(v) for v in x]
    plt.plot(x, y, color="red", label="output")
    plt.show()

    # plt.plot(t, y, color="blue", label="input")
    # plt.plot(t, vout, color="red", label="output")
    # plt.legend()
    # plt.show()

    # Y = np.fft.fft(y)
    # Y_clipped = np.fft.fft(y_clipped)
    # xf = np.fft.fftfreq(n_samples, 1 / sample_rate)
    #
    # Y_norm = 2 / n_samples * np.abs(Y[0 : n_samples // 2])
    # Y_clipped_norm = 2 / n_samples * np.abs(Y_clipped[0 : n_samples // 2])
    # xf_pos = xf[0 : n_samples // 2]
    #
    # plt.plot(xf_pos, Y_norm, alpha=0.5)
    # plt.plot(xf_pos, Y_clipped_norm, alpha=0.5, color="red")
    # plt.xlim(0, 8000)
    # plt.show()
