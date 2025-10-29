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
        # self.bias = 3.45
        # self.V_dd = 9.0
        # self.delta = 0.06
        # self.r = 330e3
        # self.c = 22e-12
        # self.prev_vout = 0
        # self.prev_vin = 0
        self.idss = 5e-3
        self.vp_n = -0.5
        self.vp_p = 0.5
        self.V_dd = 9.0

    def njfet(self, vgs, vds):
        beta = 2 * self.idss / (self.vp_n**2)

        if vgs <= -self.vp_n:
            return 0.0, 0.0

        if vds < vgs - self.vp_n and vgs > self.vp_n:
            ids = beta * ((vgs + self.vp_n) * vds - 0.5 * vds**2)
            gds = beta * ((vgs + self.vp_n) - vds)
            return ids, gds

        ids = 0.5 * beta * (vgs + self.vp_n) ** 2
        gds = 0.0
        return ids, gds

    def pjfet(self, vgs, vds):
        beta = 2 * self.idss / (self.vp_p**2)

        if vgs >= self.vp_p:
            return 0.0, 0.0

        if vds >= vgs - self.vp_p and vgs < self.vp_p:
            gds = -beta * ((vgs - self.vp_p) - vds)
            ids = -beta * ((vgs - self.vp_p) * vds - 0.5 * vds**2)
            return ids, gds
        ids = -0.5 * beta * (vgs - self.vp_p) ** 2
        gds = 0.0
        return ids, gds


    def solve(self, vin: float) -> float:
        vout = self.V_dd / 2
        for i in range(20):
            # NMOS:
            vgs_n = vin
            vds_n = vout

            # PMOS:
            vgs_p = vin - self.V_dd
            vds_p = vout - self.V_dd

            # Get currents and their derivatives
            ids_n, gds_n = self.njfet(vgs_n, vds_n)
            ids_p, gds_p = self.pjfet(vgs_p, vds_p)

            # The function we want to find the root of (KCL)
            f_x = ids_n + ids_p
            f_prime_x = gds_n + gds_p
            vout = vout - f_x / (f_prime_x + 1e-9)  # Add epsilon for stability
            vout = np.clip(vout, 0, self.V_dd)

        return vout


if __name__ == "__main__":
    model = ModelLlama()
    vin = np.linspace(-10, 15, 1000)
    vout = np.array([model.solve(v) for v in vin])

    vg = np.linspace(-1, 1, 100)

    plt.figure(figsize=(10, 6))
    plt.plot(vin, vout)
    plt.show()
