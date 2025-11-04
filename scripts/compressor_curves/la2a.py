import numpy as np
import matplotlib.pyplot as plt


x = np.linspace(-12, 60, 1000)

thr = 0
mu = 1.9
w = 5


for r in [2, 4, 8, 12, 16, 20]:
    a = w / np.log(1 + mu)
    db = a * np.log(1 + mu * np.exp((x - thr) / w))
    kr = 1 - 1 / r
    y = x - kr * db
    plt.plot(x, y, label=f"Ratio {r}:1")


plt.plot(x, 0.5*x, "k--", label="2*Unity Gain")
plt.plot(x, 0.25 * x, "k--", label="Unity Gain")
plt.legend()
plt.show()
