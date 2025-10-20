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
spice_library_path = "scripts/spice"  # Assumes library is in the current directory

# Setup the logger
logger = Logging.setup_logging()

# --- Analysis Settings ---
pot_settings = {
    str(x): x
    for x in [
        0,
        0.00001,
        0.0001,
        0.001,
        0.01,
        0.1,
        0.5,
        0.9,
        0.99,
        0.999,
        0.9999,
        0.99999,
    ]
}

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

    circuit.V("va", "va", circuit.gnd, 9 @ u_V)
    circuit.V("vb", "vb", circuit.gnd, 4.5 @ u_V)
    circuit.V("ee", "vee", circuit.gnd, circuit.gnd)

    amplitude_in = 0.001
    circuit.V("input", "in", circuit.gnd, f"DC 4.5V AC 0.001V")

    circuit.R(12, "in", circuit.gnd, 470 @ u_kOhm)
    circuit.C(9, "in", "opamp_in", 100 @ u_nF)
    circuit.R(13, "opamp_in", "vb", 1 @ u_MOhm)

    circuit.X(
        "IC1B",
        "TL074",
        "opamp_in",  # IN+ (Pin 5)
        "feedback_node",  # IN- (Pin 6)
        "va",  # V+
        "vee",  # V-
        "opamp_out",  # OUT (Pin 7)
    )
    circuit.R(14, "opamp_out", "feedback_node", 330 @ u_kOhm)
    circuit.C(10, "opamp_out", "feedback_node", 22 @ u_pF)
    circuit.R(15, "feedback_node", "n01", 3.3 @ u_kOhm)
    circuit.C(27, "n01", "pot_node", 220 @ u_nF)
    circuit.C(13, "opamp_out", "n11", 220 @ u_nF)
    circuit.R(16, "n11", "out", 6.65 @ u_kOhm)

    circuit.R(100, "out", circuit.gnd, 1 @ u_MOhm)
    circuit.R(101, "out", "vb", 1 @ u_MOhm)

    pot_total_resistance = 100e3
    drive_resistance = (1.0 - setting) * pot_total_resistance + 1e-3
    circuit.R("drive", "pot_node", "vb", drive_resistance @ u_Ohm)

    simulator = circuit.simulator(temperature=25, nominal_temperature=25)
    analysis = simulator.ac(
        start_frequency=20 @ u_Hz,
        stop_frequency=100 @ u_kHz,  # Adjusted for better view of audio range
        number_of_points=100,
        variation="dec",
    )

    # Plot the final output after the C13/R16 filter
    out_db = 20 * np.log10(np.abs(analysis.out))
    ax.semilogx(analysis.frequency, out_db, label=label)

# --- Finalize and show the plot ---
ax.legend()
plt.tight_layout()
plt.show()
