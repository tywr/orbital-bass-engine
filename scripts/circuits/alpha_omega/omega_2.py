import os
import matplotlib.pyplot as plt
import numpy as np

# Import PySpice libraries
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

circuit = Circuit("RC Bridged-T Filter")


script_path = "scripts/spice"
circuit = Circuit("Active Filter with Op-Amp")
circuit.include(os.path.join(script_path, "TL074.lib"))

circuit.V("input", "input", circuit.gnd, "DC 4.5V AC 0.1V")
circuit.V("va", "va", circuit.gnd, 9 @u_V)
circuit.V("vb", "vb", circuit.gnd, 9 @u_V)

circuit.X(
    "U1",
    "TL074",
    "op+",
    "op-",
    "va",
    circuit.gnd,
    "out",
)


circuit.R("jumper", "input", "op+", 0 @ u_Ohm)
circuit.R(54, "vb", "n1", 10 @ u_kOhm)
circuit.C(6, "n1", "n2", 120 @ u_nF)
circuit.R(53, "n1", "n2", 10 @ u_kOhm)
circuit.C(20, "n2", "input", 6.8 @ u_nF)
circuit.R(52, "input", "op-", 10 @ u_kOhm)
circuit.R(46, "op-", "out", 10 @ u_kOhm)

simulator = circuit.simulator(temperature=25, nominal_temperature=25)

analysis = simulator.ac(
    start_frequency=1 @ u_Hz,
    stop_frequency=1 @ u_MHz,
    number_of_points=100,
    variation="dec",
)

figure, axes = plt.subplots(2, figsize=(10, 8), sharex=True)
axes[0].set_title("Bode Plot - Magnitude")
axes[0].semilogx(analysis.frequency, 20 * np.log10(np.absolute(analysis.out)))
axes[0].set_ylabel("Gain [dB]")
axes[0].grid(which="both", linestyle="--")

# Plot Phase
axes[1].set_title("Bode Plot - Phase")
axes[1].semilogx(analysis.frequency, np.angle(analysis.out, deg=True))
axes[1].set_xlabel("Frequency [Hz]")
axes[1].set_ylabel("Phase [°]")
axes[1].grid(which="both", linestyle="--")

# Display the plot
plt.tight_layout()
plt.show()
