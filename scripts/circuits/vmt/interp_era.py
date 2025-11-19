import numpy as np
from matplotlib import pyplot as plt

x = [0, 0.25, 0.5, 0.75, 1.0]
y = [700, 801, 951, 1260, 2360]
y2 = [700 * np.exp(1.21533 * i * i) for i in x]

plt.plot(x, y, marker='o')
plt.plot(x, y2)
plt.title('VMT Circuit Era Interpolation')
plt.show()


