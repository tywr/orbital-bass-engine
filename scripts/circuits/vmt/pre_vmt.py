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
    number_of_points=100,
    variation="dec",
)

# --- Plot the Results ---
figure, axes = plt.subplots(2, figsize=(12, 8))
plt.suptitle("Frequency Response of the Circuit", fontsize=16)

# Plot Magnitude in dB
axes[0].semilogx(analysis.frequency, 20 * np.log10(np.absolute(analysis.output)))
axes[0].set_title("Magnitude Response")
axes[0].set_xlabel("Frequency [Hz]")
axes[0].set_ylabel("Magnitude [dB]")
axes[0].grid(which="both", linestyle="--", linewidth=0.5)
axes[0].minorticks_on()

# Plot Phase in degrees
axes[1].semilogx(analysis.frequency, np.angle(analysis.n2, deg=True))
axes[1].set_title("Phase Response")
axes[1].set_xlabel("Frequency [Hz]")
axes[1].set_ylabel("Phase [°]")
axes[1].grid(which="both", linestyle="--", linewidth=0.5)
axes[1].minorticks_on()

plt.tight_layout(rect=[0, 0.03, 1, 0.95])
plt.show()
