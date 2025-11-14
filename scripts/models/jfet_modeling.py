import numpy as np
import matplotlib.pyplot as plt
from numpy.polynomial.polynomial import Polynomial
from scipy.optimize import curve_fit
from sklearn.neural_network import MLPRegressor
from sklearn.preprocessing import StandardScaler
import onnxruntime as rt
import skl2onnx
from skl2onnx import convert_sklearn
from skl2onnx.common.data_types import FloatTensorType
from scipy.optimize import curve_fit


def jfet(x: float):
    gamma = 0.15
    vp = 0.5
    a = gamma * (1 - vp)

    c = 0.9 if x < 0 else 1
    xsat = vp + 1 / (2 * a)
    fsat = -1 / (4 * a) + 1 / (2 * a) + vp

    sign_x = np.sign(x)
    x_abs = np.abs(x)

    xc = xsat if x_abs >= xsat else x_abs
    fc = sign_x * (-a * c * (xc - vp) ** 2 + c * (xc - vp) + vp) / fsat

    fl = x / fsat
    return fl if x_abs < vp else fc




    # if x_abs < vp:
    #     return x / fsat
    # elif x_abs >= xsat:
    #     return sign_x * (-a * c * (xsat - vp) ** 2 + c * (xsat - vp) + vp) / fsat
    # else:
    #     return sign_x * (-a * c * (x_abs - vp) ** 2 + c * (x_abs - vp) + vp) / fsat


if __name__ == "__main__":
    vin = np.linspace(-10, 10, 1000)
    vout = [jfet(v) for v in vin]

    plt.figure(figsize=(10, 6))
    plt.plot(vin, vout)
    plt.show()
