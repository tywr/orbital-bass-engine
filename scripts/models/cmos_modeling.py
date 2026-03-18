import numpy as np
import matplotlib.pyplot as plt


class CMOS_SH:
    def __init__(self):
        self.V_dd = 9.0
        self.kn = 1.0e-3
        self.vth_n = 0.5
        self.kp = 0.4e-3
        self.vth_p = -0.5
        self.delta = 0.06

    def nmos(self, vgs, vds):
        vt = self.vth_n
        if vgs <= vt:
            return 0.0, 0.0
        if vds < vgs - vt:
            ids = self.kn * (vgs - vt - vds / 2) * vds
            gds = self.kn * (vgs - vt) - self.kn * vds
            return ids, gds
        ids = 0.5 * self.kn * (vgs - vt) ** 2 * (1 + self.delta * vds)
        gds = 0.5 * self.kn * (vgs - vt) ** 2 * self.delta
        return ids, gds

    def pmos(self, vgs, vds):
        vt = self.vth_p
        if vgs >= vt:
            return 0.0, 0.0
        if vds >= vgs - vt:
            ids = -self.kp * (vgs - vt - vds / 2) * vds
            gds = -self.kp * (vgs - vt) + self.kp * vds
            return ids, gds
        ids = -0.5 * self.kp * (vgs - vt) ** 2 * (1 + self.delta * vds)
        gds = -0.5 * self.kp * (vgs - vt) ** 2 * self.delta
        return ids, gds

    def solve(self, vin: float) -> float:
        """Compute Vout for a given Vin (both in volts, 0 to V_dd)."""
        vout = self.V_dd / 2

        for _ in range(10):
            vgs_n = vin
            vds_n = vout
            vgs_p = vin - self.V_dd
            vds_p = vout - self.V_dd

            ids_n, gds_n = self.nmos(vgs_n, vds_n)
            ids_p, gds_p = self.pmos(vgs_p, vds_p)

            f_x = ids_n + ids_p
            f_prime_x = gds_n + gds_p

            # Add a small dampening factor to ensure stability of the solving
            vout = vout - f_x / (f_prime_x + 1e-3)
            vout = np.clip(vout, 0, self.V_dd)

        return vout


if __name__ == "__main__":
    model = CMOS_SH()

    # --- VTC: Vin vs Vout (volts) ---
    vin = np.linspace(0, model.V_dd, 100)
    vout = np.array([model.solve(v) for v in vin])

    fig, ax = plt.subplots(1, 1, figsize=(14, 6))

    ax.plot(vin, vout)
    ax.set_xlabel("Vin (V)")
    ax.set_ylabel("Vout (V)")
    ax.set_title("CMOS Inverter Transfer Characteristic")
    ax.set_xlim(0, 9)
    ax.set_ylim(0, 9)
    ax.set_aspect("equal")
    ax.grid(True)

    plt.tight_layout()
    plt.show()
    plt.savefig("cmos_transfer.svg", format="svg", transparent=True)
    print(list([float(x) for x in vin]))
    print(list(float(x) for x in vout))
