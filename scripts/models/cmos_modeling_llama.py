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


class ModelLlama:
    """
    Models a single CMOS inverter unit based on the DAFx 2020 paper_21.

    Args:
        vin: The input voltage to the CMOS inverter gate.

    Returns:
        The corresponding output voltage from the inverter.
    """

    def __init__(self):
        self.bias = 3.45
        self.V_dd = 9.0
        self.delta = 0.06
        self.r = 330e3
        self.c = 22e-12
        self.prev_vout = 0
        self.prev_vin = 0

    def nmos(self, vgs, vds):
        """Calculates NMOS I_ds and its derivative w.r.t. vds (g_ds)."""
        vt_coef = [1.208306917691355, 0.3139084341943607]
        alpha_coef = [0.020662094888127674, -0.0017181795239085821]

        alpha = alpha_coef[1] * vgs + alpha_coef[0]
        vt = vt_coef[1] * vgs + vt_coef[0]

        if vgs <= vt:
            return 0.0, 0.0, vt, alpha

        if vds < vgs - vt and vgs > vt:
            ids = alpha * (vgs - vt - vds / 2) * vds
            gds = alpha * (vgs - vt) - alpha * vds  # Derivative d(Ids)/d(Vds)
            return ids, gds, vt, alpha

        ids = 0.5 * alpha * (vgs - vt) ** 2
        gds = 0.0
        return ids, gds, vt, alpha

    def pmos(self, vgs, vds):
        """Calculates PMOS I_sd and its derivative w.r.t. vsd (g_sd)."""
        vt_coef = [-0.25610349392710086, 0.27051216771368214]
        alpha_coef = [
            -0.0003577445606469842,
            -0.0008620153809796321,
            -0.00016848836814836602,
            -1.0800821774906936e-5,
        ]
        alpha = (
            alpha_coef[3] * vgs**3
            + alpha_coef[2] * vgs**2
            + alpha_coef[1] * vgs
            + alpha_coef[0]
        )
        vt = vt_coef[1] * vgs + vt_coef[0]

        if vgs >= vt:
            return 0.0, 0.0, vt, alpha

        if vds >= vgs - vt and vgs < vt:
            ids = -alpha * (vgs - vt - vds / 2) * vds * (1 - self.delta * vds)
            gds = -alpha * (
                3 * self.delta * vds**2 / 2
                - (2 * self.delta * (vgs - vt) + 1) * vds
                + vgs
                - vt
            )
            return ids, gds, vt, alpha

        ids = -0.5 * alpha * ((vgs - vt) ** 2) * (1 - self.delta * vds)
        gds = 0.5 * alpha * self.delta * (vgs - vt) ** 2
        return ids, gds, vt, alpha

    def solve(self, x: float) -> float:
        vin = x + self.bias
        vout = self.V_dd / 2.0

        for _ in range(5):
            # NMOS:
            vgs_n = vin
            vds_n = vout

            # PMOS:
            vgs_p = vin - self.V_dd
            vds_p = vout - self.V_dd

            # Get currents and their derivatives
            ids_n, gds_n, _, _ = self.nmos(vgs_n, vds_n)
            ids_p, gds_p, _, _ = self.pmos(vgs_p, vds_p)

            # The function we want to find the root of (KCL)
            f_x = ids_n + ids_p

            # The derivative of the function w.r.t. vout
            # d(ids_n)/d(vout) = gds_n
            # d(isd_p)/d(vout) = d(isd_p)/d(vsd_p) * d(vsd_p)/d(vout) = gsd_p * (-1) = -gsd_p
            f_prime_x = gds_n + gds_p

            # Newton-Raphson update step
            vout = vout - f_x / (f_prime_x + 1e-9)  # Add epsilon for stability

            # Clamp vout to the supply rails for stability during iteration
            vout = np.clip(vout, 0, self.V_dd)

        return 1 - 2 * vout / self.V_dd


def poly_exp(x, *coefficients):
    P = 0
    for i, a in enumerate(coefficients):
        P += a * x**i
    return np.exp(P)


if __name__ == "__main__":
    model = ModelLlama()
    vin = np.linspace(-10, 10, 1000)
    vout = np.array([model.solve(v) for v in vin])

    # bias = 3.44
    # fs = 44100
    # frequency = 440
    # t = np.linspace(0, 3 / frequency, fs)
    # y = np.sin(2 * np.pi * frequency * t)
    # yp = [model.solve(bias + sample) for sample in y]

    # modelp = ModelLlama(V_dd=9, delta=0.06)
    # xp = np.linspace(4.5 - 0.7, 4.5 + 0.7, 5000)
    # yp = np.array([modelp.solve(v) for v in xp])

    # amp_in = np.linspace(0, 5, 5000)
    # amp_out = np.array(
    #     [model.solve(max(3.8, 4.5 - a)) - model.solve(4.5 + a) for a in amp_in]
    # )

    # p0 = [0, 0, 0, 0, 0, 0, 0]
    # params, _ = curve_fit(poly_exp, x, y, p0=p0, maxfev=10000)
    # print(params)
    # x_fit = np.linspace(3, 20, 10000)
    # y_fit = poly_exp(x_fit, *params)
    #
    # for x, y in zip(x_fit, y_fit):
    #     if y < 1e-7:
    #         print(f"x: {x}, y: {y}")
    #         break

    plt.figure(figsize=(10, 6))
    plt.plot(vin, vout)
    # plt.plot(t, y)
    # plt.plot(t, yp)
    # plt.plot(t, yp, color="blue", alpha=0.5)
    plt.show()
