import numpy as np
import matplotlib.pyplot as plt


x = np.linspace(-12, 60, 1000)


def db_1176(x, r):
    thr = 0
    w = 5
    if x <= thr - w:
        return x
    elif thr - w < x < thr + w:
        return x + (1 / r - 1) * ((x - thr + w) ** 2) / (4 * w)
    else:
        return thr + (x - thr) / r


for r in [2, 4, 8, 12, 16, 20]:
    y = np.array([db_1176(xi, r) for xi in x])
    plt.plot(x, y, label=f"Ratio {r}:1")


plt.plot(x, 0.5 * x, "k--", label="2*Unity Gain")
plt.plot(x, 0.25 * x, "k--", label="Unity Gain")
plt.legend()
plt.show()
