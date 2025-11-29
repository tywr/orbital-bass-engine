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

circuit = Circuit("Filter")

# Include the op-amp model library
circuit.include(os.path.join(spice_library_path, "TL074.lib"))

circuit.V("va", "va", circuit.gnd, 9 @ u_V)
circuit.V("vb", "vb", circuit.gnd, 9 @ u_V)
circuit.V("ee", "vee", circuit.gnd, circuit.gnd)

circuit.V("input", "in", circuit.gnd, "DC 0V AC 1V")

circuit.X(
    "U1",
    "TL074",
    "nf",
    "out",
    "vb",
    circuit.gnd,
    "out",
)


circuit.R(1, "in", "va", 1 @ u_MOhm)
# circuit.R(2, "in", circuit.gnd, 1 @ u_MOhm)

circuit.R(24, "in", "n1", 22 @ u_kOhm)
circuit.C(20, "n1", "out", 2.2 @ u_nF)
circuit.R(25, "n1", "nf", 100 @ u_kOhm)
circuit.C(21, "nf", circuit.gnd, 1 @ u_nF)


simulator = circuit.simulator(temperature=25, nominal_temperature=25)
analysis = simulator.ac(
    start_frequency=20 @ u_Hz,
    stop_frequency=100 @ u_kHz,  # Adjusted for better view of audio range
    number_of_points=1000,
    variation="dec",
)

# Plot the final output after the C13/R16 filter
out_db = 20 * np.log10(np.abs(analysis.out))
ax.semilogx(analysis.frequency, out_db)

# --- Finalize and show the plot ---
ax.legend()
plt.tight_layout()
plt.show()
