import matplotlib.pyplot as plt
import numpy as np
import PySpice.Logging.Logging as Logging
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# --- Setup Logger ---
# This helps in debugging if there are any issues with the simulation.
logger = Logging.setup_logging()

# --- Define the Circuit ---
# Based on the provided schematic image.
circuit = Circuit("Analog Filter Circuit")

# Define the input voltage source.
# We use an AC source with a magnitude of 1V to perform frequency analysis.
circuit.V("input", "input", circuit.gnd, "DC 0 AC 1")

# Components connected to the input node
circuit.R(8, "input", "middle", 261 @ u_kOhm)
circuit.R(11, "middle", "output", 470 @ u_kOhm)

circuit.C(5, "input", "n1", 22 @ u_nF)
circuit.C(6, "n1", "n2", 22 @ u_nF)
circuit.C(7, "n2", "middle", 22 @ u_nF)
circuit.C(8, "middle", "output", 220 @ u_pF)

circuit.R(9, "n1", circuit.gnd, 6.65 @ u_kOhm)
circuit.R(10, "n2", circuit.gnd, 22 @ u_kOhm)
circuit.R(12, "output", circuit.gnd, 470 @ u_kOhm)


# --- Print the Circuit Netlist ---
print("Generated Netlist:\n")
print(circuit)

# --- Set up the Simulation ---
# We will perform an AC analysis (frequency sweep).
simulator = circuit.simulator(temperature=25, nominal_temperature=25)
# Analysis parameters: start=1Hz, stop=100kHz, 100 points per decade, using a decade scale.
analysis = simulator.ac(
    start_frequency=1 @ u_Hz,
    stop_frequency=100 @ u_kHz,
    number_of_points=1000,
    variation="dec",
)

f0 = 288  # Notch frequency in Hz
Q = 0.175
gain_dB = -34.55  # Peak gain in dB
frequencies = np.array(analysis.frequency)

# Notch filter transfer function: H(s) = (s^2 + w0^2) / (s^2 + (w0/Q)*s + w0^2)
# For frequency response: s = j*2*pi*f
# w0 = 2 * np.pi * f0
# w = 2 * np.pi * frequencies

w0 = 2 * np.pi * f0
w = 2 * np.pi * frequencies

# Linear gain
A = 10 ** (gain_dB / 20)

# Transfer function magnitude for peaking filter
# H(s) = (s^2 + (A*w0/Q)*s + w0^2) / (s^2 + (w0/Q)*s + w0^2)
numerator_real = w0**2 - w**2
numerator_imag = (A * w0 / Q) * w
numerator_mag = np.sqrt(numerator_real**2 + numerator_imag**2)

denominator_real = w0**2 - w**2
denominator_imag = (w0 / Q) * w
denominator_mag = np.sqrt(denominator_real**2 + denominator_imag**2)

H_peak = numerator_mag / denominator_mag
H_peak_dB = 20 * np.log10(H_peak + 1e-10)

# Second filter for difference
f0 = 80  # Notch frequency in Hz
Q = 0.5
gain_dB = -2  # Peak gain in dB
frequencies = np.array(analysis.frequency)
w0 = 2 * np.pi * f0
w = 2 * np.pi * frequencies
A = 10 ** (gain_dB / 20)
numerator_real = w0**2 - w**2
numerator_imag = (A * w0 / Q) * w
numerator_mag = np.sqrt(numerator_real**2 + numerator_imag**2)

denominator_real = w0**2 - w**2
denominator_imag = (w0 / Q) * w
denominator_mag = np.sqrt(denominator_real**2 + denominator_imag**2)

H2_peak = numerator_mag / denominator_mag
H2_peak_dB = 20 * np.log10(H2_peak + 1e-10)


# Low shelf
f0 = 1510  # Shelf frequency in Hz
Q = 0.4  # Q factor (0.707 is typical for shelf filters)
gain_dB = -8  # Shelf gain in dB

frequencies = np.array(analysis.frequency)
w0 = 2 * np.pi * f0
w = 2 * np.pi * frequencies
A = 10 ** (gain_dB / 40)  # Amplitude factor for shelving

# Low-shelf transfer function
# H(s) = A * (s^2 + (sqrt(A)/Q)*s + A*w0^2) / (A*s^2 + (sqrt(A)/Q)*s + w0^2)
numerator_real = A * w0**2 - w**2
numerator_imag = np.sqrt(A) * w0 * w / Q
numerator_mag = np.sqrt(numerator_real**2 + numerator_imag**2)

denominator_real = w0**2 - A * w**2
denominator_imag = np.sqrt(A) * w0 * w / Q
denominator_mag = np.sqrt(denominator_real**2 + denominator_imag**2)

H3 = numerator_mag / denominator_mag
H3_lowshelf_dB = 20 * np.log10(H3 + 1e-10)

H_dB = -4 + H_peak_dB + H2_peak_dB + H3_lowshelf_dB


# --- Plot the Results ---
figure, ax = plt.subplots(1, figsize=(12, 8))
plt.suptitle("Frequency Response of the Circuit", fontsize=16)

ax.semilogx(
    frequencies,
    H_dB,
    label="Notch Filter Response",
    color="red",
    linewidth=2,
)

# Plot Magnitude in dB
ax.semilogx(frequencies, 20 * np.log10(np.absolute(analysis.output)))
ax.semilogx(frequencies, H_dB - 20 * np.log10(np.absolute(analysis.output)))
ax.set_title("Magnitude Response")
ax.set_xlabel("Frequency [Hz]")
ax.set_ylabel("Magnitude [dB]")
ax.grid(which="both", linestyle="--", linewidth=0.5)
ax.minorticks_on()

plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.show()
