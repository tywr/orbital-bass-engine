import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
from scipy.interpolate import CubicSpline


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
        vout = self.bias

        for i in range(5):
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

            vout = vout - f_x / (f_prime_x + 1e-9)  # Add epsilon for stability

            # Clamp vout to the supply rails for stability during iteration
            vout = np.clip(vout, 0, self.V_dd)

        return 1 - 2 * vout / self.V_dd
        # return vout


def lut_model():
    model = ModelLlama()
    vmin = -1.8
    tn = np.linspace(0, 1, 2048)
    vn = vmin * (1 - tn) * (1 - tn)

    vmax = 5.1
    tp = np.linspace(0, 1, 4096)
    vp = vmax * tp * tp

    vin = np.concatenate([vn, vp[1:]])
    vout = np.array([model.solve(v) for v in vin])
    return vin, vout


if __name__ == "__main__":
    model = ModelLlama()
    vin, vout = lut_model()

    lut_interp = interp1d(vin, vout, kind="linear", fill_value="extrapolate")
    # lut_interp = CubicSpline(vin, vout)
    vout_lut = lut_interp(vin)

    vin_t = np.linspace(-1.8, 5.1, 10000)
    vout_t_interp = lut_interp(vin_t)
    vout_t_model = np.array([model.solve(v) for v in vin_t])

    diff = np.abs((vout_t_interp - vout_t_model) / vout_t_model)
    print("max error:", max(diff))
    print("avg error:", np.average(diff))

    plt.figure(figsize=(10, 6))
    plt.plot(vin, vout)
    plt.plot(vin_t, diff)
    plt.show()
