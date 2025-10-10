import os
import matplotlib.pyplot as plt
import numpy as np

import PySpice.Logging.Logging as Logging
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# --- IMPORTANT NOTE ON LIBRARY PATH ---
# Make sure the "TL074.lib" file is in the same directory as this script,
# or provide the correct full path to it.
# For example: spice_library_path = '/path/to/your/libs'
spice_library_path = 'scripts/spice' # Assumes library is in the current directory

# Setup the logger
logger = Logging.setup_logging()

# --- Analysis Settings ---
pot_settings = {"Min (10%)": 0.0005, "Mid (50%)": 0.5, "Max (90%)": 0.9}

# Setup the plot
plt.figure(figsize=(10, 6))
ax = plt.gca()
ax.set_title("Frequency Response of Active Filter")
ax.set_xlabel("Frequency [Hz]")
ax.set_ylabel("Gain [dB]")
ax.grid(which="both", axis="both")

# --- Loop through settings and simulate ---
for label, setting in pot_settings.items():
    circuit = Circuit(f"Filter at {label}")

    # Include the op-amp model library
    circuit.include(os.path.join(spice_library_path, "TL074.lib"))

    # --- Power Supplies ---
    # Power for the "Drive" pot's reference voltage
    circuit.V("vdd", "vdd", circuit.gnd, 9 @ u_V)
    # Power for the Op-Amp
    circuit.V("cc", "vcc", circuit.gnd, 15 @ u_V)
    circuit.V("ee", "vee", circuit.gnd, -15 @ u_V)

    # --- Input Signal ---
    # The input signal goes to the non-inverting input (pin 5)
    circuit.V("input", "in", circuit.gnd, "DC 0V AC 1V")

    # <<< FIX 1: Correct Op-Amp Subcircuit Call >>>
    # The standard pin order is: IN+, IN-, V+, V-, OUT
    # We need to define a node for the inverting input (pin 6) -> 'inv_in'
    circuit.X(
        "IC1B",
        "TL074",
        "in",           # IN+ (Pin 5)
        "inv_in",       # IN- (Pin 6)
        "vcc",          # V+
        "vee",          # V-
        "opamp_out",    # OUT (Pin 7)
    )
    circuit.C(13, "opamp_out", "node_after_c13", 220 @ u_nF)
    circuit.R(16, "node_after_c13", "out", 6.65 @ u_kOhm)
    circuit.R('load', 'out', circuit.gnd, 1@u_MOhm)
    circuit.R(15, "opamp_out", "feedback_node", 220 @ u_kOhm)
    circuit.C(10, "opamp_out", "feedback_node", 220 @ u_pF)
    circuit.R(13, "feedback_node", "inv_in", 10 @ u_kOhm) # ASSUMED 10k VALUE

    # --- Drive Control Section ---
    # This section now correctly connects to the 'feedback_node'.
    circuit.R(17, "feedback_node", "node_after_r17", 3.3 @ u_kOhm)
    circuit.C(27, "node_after_r17", "pot_node", 220 @ u_nF)

    pot_total_resistance = 100e3
    drive_resistance = (1.0 - setting) * pot_total_resistance + 1e-3
    circuit.R("drive", "pot_node", "vdd", drive_resistance)

    # --- Simulation ---
    simulator = circuit.simulator(temperature=25, nominal_temperature=25)
    analysis = simulator.ac(
        start_frequency=10 @ u_Hz,
        stop_frequency=100 @ u_kHz, # Adjusted for better view of audio range
        number_of_points=100,
        variation="dec",
    )
    
    # Plot the final output after the C13/R16 filter
    ax.semilogx(analysis.frequency, 20 * np.log10(np.abs(analysis.out)), label=label)

# --- Finalize and show the plot ---
ax.legend()
plt.tight_layout()
plt.show()
