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

from cmos_modeling_llama import ModelLlama
from diode_modeling import DiodeClipper


# --- Demonstration of the function ---
if __name__ == "__main__":
    print("Demonstrating the DAFx 2020 CMOS Inverter Model.")

    sample_rate = 44100
    model = ModelLlama(V_dd=9.0, delta=0.06)
    diode_plus = DiodeClipper(sample_rate, side="up")
    diode_minus = DiodeClipper(sample_rate, side="down")

    frequency = 440
    max_t = 10 / frequency
    dt = 1.0 / sample_rate
    t = np.arange(0, max_t, dt)

    input = .5 * np.sin(2 * np.pi * frequency * t)
    output_diode = np.array(
        [diode_minus.process_sample(.5 * s + .5 * diode_plus.process_sample(s)) for s in input]
    )
    output_diode_offset = 4.5 + output_diode
    output_cmos = np.array([model.solve(v) for v in output_diode])
    output_cmos = np.array([model.solve(v) for v in input])

    plt.plot(t, input, color="blue", alpha=0.5, label="Input")
    plt.plot(t, output_diode, color="green", alpha=0.5, label="Diode Clipping Vout")
    plt.show()
    # plt.plot(t, output_cmos, color="red", alpha=0.5, label="CMOS Inverter Out")

    # plt.plot(input, output_cmos, alpha=0.5)
    #
    # plt.grid(True)
    # plt.legend()
    # plt.show()

    # Sine wave processing
    # SAMPLE_RATE = 48000
    # duration = 0.02
    # frequency = 500
    # t = np.linspace(0.0, duration, int(SAMPLE_RATE * duration), endpoint=False)
    #
    # # Input sine wave centered around Vdd/2 with large amplitude to show clipping
    # input_sine = 4.5 + 5 * np.sin(2.0 * np.pi * frequency * t)
    #
    # output_sine = np.array([model_dafx_cmos(s) for s in input_sine])
    #
    # plt.figure(figsize=(12, 7))
    # plt.plot(t, input_sine, label="Input Signal (Vin)")
    # plt.plot(t, output_sine, label="Output Signal (Vout)")
    # plt.title("CMOS Inverter Processing a Sine Wave with Red LLAMA")
    # plt.xlabel("Time (s)")
    # plt.ylabel("Voltage (V)")
    # plt.grid(True)
    # plt.legend()
    # plt.show()

    # n = len(output_cmos)
    # Y = np.fft.fft(output_cmos)
    # xf = np.fft.fftfreq(n, 1 / sample_rate)
    # xf_pos = xf[: n // 2]
    # Y_abs = np.abs(Y[: n // 2])
    # Y_norm = Y_abs / n
    # Y_norm[1:] = Y_norm[1:] * 2
    #
    # plt.plot(xf_pos, Y_norm, label="Output Spectrum", alpha=0.5)
    # plt.show()
