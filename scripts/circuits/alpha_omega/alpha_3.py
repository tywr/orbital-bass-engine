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

circuit.V("supply", "vcc", circuit.gnd, 9 @ u_V)

circuit.V("input", "in", circuit.gnd, "DC 4.5V AC 0.1V")

circuit.R(24, "in", "filtered_node", 47 @ u_kOhm)
circuit.C(30, "filtered_node", circuit.gnd, 1 @ u_nF)
circuit.C(29, "in", "opamp_out", 1 @ u_nF)
circuit.R(23, "opamp_out", "final_out", 10 @ u_kOhm)
circuit.R(100, "final_out", circuit.gnd, 1 @ u_MOhm)
circuit.R(101, "final_out", "vb", 1 @ u_MOhm)

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


output_node = analysis.final_out
figure, ax = plt.subplots(1, figsize=(10, 8), sharex=True)
ax.set_title("Bode Plot - Magnitude")
ax.semilogx(analysis.frequency, 20 * np.log10(np.absolute(output_node)))
ax.set_ylabel("Gain [dB]")
ax.grid(which="both", linestyle="--")

plt.tight_layout()
plt.show()
