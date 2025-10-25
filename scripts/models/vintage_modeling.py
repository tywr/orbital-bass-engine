import numpy as np
import ctypes
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

from cmos_modeling_llama import ModelLlama

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


def omega(x: np.array) -> float:
    out = []
    for e in x:
        if abs(e) < 1.5:
            out.append(lib.omega4_py(e))
        else:
            out.append(omega_small(e))
    return np.array(out)


def mixed_func(x, s1, alpha, s2):
    dy0 = 7.448510216701049
    return dy0 / ((x / s1) ** alpha + np.exp(x / s2))


def rational_func(x, s1, s2):
    dy0 = 7.448510216701049
    return dy0 / (1 + (x / s1) + (x / s2) ** 3)


def exponential_decay(x, sc, k):
    dy0 = 7.448510216701049
    return dy0 * np.exp(-((x / sc) ** k))


def tanh_decay(x, xc, s):
    dy0 = 7.448510216701049
    return dy0 * 0.5 * (1 - np.tanh((x - xc) / s))


def gen_logistic4(x, A, K, B, M):
    Q = 1.0
    nu = 1.0
    return A + (K - A) / ((1 + Q * np.exp(-B * (x - M))) ** (1.0 / nu))


if __name__ == "__main__":
    model = ModelLlama()
    x = np.linspace(0, 7, 1000)
    y = np.array([-model.solve(-v) for v in x])
    # x = np.linspace(0, 7, 1000)
    # y = np.array([model.solve(v) for v in x])
    dy = np.gradient(y, x)

    # s1 = 0.04
    # s2 = 0.75
    # s1 = 0.2
    # s2 = 0.4
    # y_approx = 1 - 2 * (1 + x / s1 + np.exp((x / s2))) ** -1

    # omega p
    # s1 = 0.04
    # s2 = 0.25

    # omega n
    s1 = 0.5
    s2 = 0.1
    y_approx = 1 - 2 * (1 + x/s1 + omega(1 + x / s2)) ** -1

    plt.plot(x, y, color="green", label="CMOS Model")
    plt.plot(x, y_approx, color="red")
    # plt.plot(x, dy)
    # plt.plot(x, y_fit, linestyle="--", label="Approximation")

    plt.show()
