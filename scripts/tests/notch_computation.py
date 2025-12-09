import numpy as np


def summary(era):
    c16 = 1e-9
    c17 = 4.7e-9
    r21 = 10e3
    r20 = 100e3
    req = r21 + 100e3 * era

    f0 = 1 / (2 * np.pi * np.sqrt(c16 * c17 * r20 * req))
    q = np.sqrt(c16 * c17 * r20 * req) / (c16 * (r20 + req) + c17 * r20)
    print(f0, q)


summary(0)
