import matplotlib.pyplot as plt
import numpy as np
import os
import PySpice.Logging.Logging as Logging
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# Suppress PySpice warnings for cleaner output
logger = Logging.setup_logging()

# --- Create the main circuit
circuit = Circuit("Active Tone Control Filter with TL074 Model")

script_path = "scripts/spice"
circuit.include(os.path.join(script_path, "TL074.lib"))

circuit.V("cc", "vcc", circuit.gnd, 0 @ u_V)
circuit.V("ee", "vee", circuit.gnd, 0)

circuit.V("input", "in_node", circuit.gnd, "DC 0V AC 1V")
# circuit.V("input", "in_amp", circuit.gnd, "DC 0V AC 1V")

# circuit.X(
#     "U1",
#     "TL074",
#     "in_amp",  # 1: Non-inverting input (IN+)
#     "in_node",  # 2: Inverting input (IN-), connected to output for buffer config
#     "vcc",  # 3: Positive power supply (V+)
#     circuit.gnd,  # 4: Negative power supply (V-)
#     "in_node",  # 5: Output
# )

circuit.R(20, "in_node", "middle_node", 100 @ u_kOhm)
circuit.C(17, "middle_node", circuit.gnd, 22 @ u_nF)
circuit.R(21, "middle_node", "era_node", 33 @ u_kOhm)
circuit.C(16, "in_node", "out_node", 680 @ u_pF)

pot_settings = {
    "Minimum (0%)": 0.1 @ u_Ohm,
    "Minimum (25%)": 25 @ u_kOhm,
    "Middle (50%)": 50 @ u_kOhm,
    "Middle (75%)": 75 @ u_kOhm,
    "Maximum (100%)": 100 @ u_kOhm,
}

if __name__ == "__main__":
    figure, ax = plt.subplots(1, 1, figsize=(10, 8), sharex=True)
    plt.suptitle("Era Knob EQ", fontsize=16)
    ax.set_ylabel("Gain [dB]")
    ax.grid(which="both", linestyle="--", linewidth=0.5)

GAIN_DB = []
for label, resistance in pot_settings.items():
    run_circuit = circuit.clone()
    run_circuit.R("pot", "era_node", "out_node", resistance)

    simulator = run_circuit.simulator(temperature=25, nominal_temperature=25)
    analysis = simulator.ac(
        start_frequency=1 @ u_Hz,
        stop_frequency=100 @ u_kHz,
        number_of_points=100,
        variation="dec",
    )
    # --- Plotting the results
    freq = analysis.frequency
    gain_db = 20 * np.log10(np.abs(analysis.out_node)) - 87.87 - 21
    if __name__ == "__main__":
        ax.semilogx(freq, 110 + gain_db - 1.05, label=label)
    FREQUENCY = freq
    GAIN_DB.append(gain_db)


if __name__ == "__main__":
    ax.legend()
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()
