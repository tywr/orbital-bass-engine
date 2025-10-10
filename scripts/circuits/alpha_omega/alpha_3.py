import os
import matplotlib.pyplot as plt
import numpy as np

# Import PySpice libraries
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

script_path = "scripts/spice"

# --- Circuit Definition ---
# Create a new circuit object
circuit = Circuit("Active Filter with Op-Amp")
circuit.include(os.path.join(script_path, "TL074.lib"))

# Define power supplies for the op-amp.
# The schematic shows pin 4 (VEE) to GND and pin 8 (VCC) to VA.
# This suggests a single-supply configuration. Let's assume VA is +5V.
circuit.V("supply", "vcc", circuit.gnd, 9 @ u_V)

# Define the input voltage source.
# For AC analysis, we use an AC magnitude of 1V for easy gain calculation.
circuit.V("input", "in", circuit.gnd, "DC 0V AC 1V")

# Add the passive components based on the schematic
circuit.R(24, "in", "filtered_node", 47 @ u_kOhm)  # R28
circuit.C(30, "filtered_node", circuit.gnd, 1 @ u_nF)  # C21

# This capacitor creates the feed-forward path from input to output
circuit.C(29, "in", "opamp_out", 1 @ u_nF)  # C19

# Output resistor
circuit.R(38, "opamp_out", "final_out", 10 @ u_kOhm)  # R23

circuit.X(
    "U1",
    "TL074",
    "filtered_node",  # 1: Non-inverting input (IN+)
    "opamp_out",
    "vcc",  # 3: Positive power supply (V+)
    circuit.gnd,  # 4: Negative power supply (V-)
    "opamp_out",  # 5: Output
)

simulator = circuit.simulator(temperature=25, nominal_temperature=25)

analysis = simulator.ac(
    start_frequency=1 @ u_Hz,
    stop_frequency=10 @ u_kHz,
    number_of_points=100,
    variation="dec",
)


# --- Plotting Results ---
# We will plot the response at the final output node ('final_out')
output_node = analysis.final_out

# Create a figure with two subplots (for magnitude and phase)
figure, axes = plt.subplots(2, figsize=(10, 8), sharex=True)

# Plot Magnitude (Gain in dB)
axes[0].set_title("Bode Plot - Magnitude")
axes[0].semilogx(analysis.frequency, 20 * np.log10(np.absolute(output_node)))
axes[0].set_ylabel("Gain [dB]")
axes[0].grid(which="both", linestyle="--")

# Plot Phase
axes[1].set_title("Bode Plot - Phase")
axes[1].semilogx(analysis.frequency, np.angle(output_node, deg=True))
axes[1].set_xlabel("Frequency [Hz]")
axes[1].set_ylabel("Phase [°]")
axes[1].grid(which="both", linestyle="--")

# Display the plot
plt.tight_layout()
plt.show()
