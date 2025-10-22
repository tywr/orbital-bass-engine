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

# Setup the plot
plt.figure(figsize=(10, 6))
ax = plt.gca()
ax.set_title("Frequency Response of Active Filter")
ax.set_xlabel("Frequency [Hz]")
ax.set_ylabel("Gain [dB]")
ax.grid(which="both", axis="both")

# --- Loop through settings and simulate ---
for c in [4.7, 22, 220]:
    circuit = Circuit(f"Filter at {c}nF")

    amplitude_in = 0.1
    circuit.V("input", "in", circuit.gnd, f"DC 4.5V AC 1V")
    circuit.V("vb", "vb", circuit.gnd, "DC 4.5V")

    circuit.C(9, "in", "middle", c @ u_nF)
    circuit.R(1, "middle", "out", 6.8 @ u_kOhm)
    circuit.R(13, "out", "vb", 1 @ u_MOhm)
    circuit.R(14, "out", circuit.gnd, 1 @ u_MOhm)

    simulator = circuit.simulator(temperature=25, nominal_temperature=25)
    analysis = simulator.ac(
        start_frequency=20 @ u_Hz,
        stop_frequency=100 @ u_kHz,  # Adjusted for better view of audio range
        number_of_points=100,
        variation="dec",
    )

    # Plot the final output after the C13/R16 filter
    out_db = 20 * np.log10(np.abs(analysis.out))
    ax.semilogx(analysis.frequency, out_db)

# --- Finalize and show the plot ---
ax.legend()
plt.tight_layout()
plt.show()
