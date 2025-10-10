import matplotlib.pyplot as plt
import numpy as np

# Import PySpice libraries
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# --- Circuit Definition ---
# Create a new circuit object
circuit = Circuit('RC Bridged-T Filter')

# Define the input voltage source. 
# For AC analysis, we use an AC magnitude of 1V for easy gain calculation.
# It is connected between the input node 'in' and ground.
circuit.V('input', 'in', circuit.gnd, 'DC 0V AC 1V')

# Add the components based on the schematic
# Note: 8k2 = 8.2 kOhms, 2n2 = 2.2 nF
circuit.R(25, 'in', 'node1', 8.2@u_kOhm)
circuit.C(18, 'in', 'node2', 2.2@u_nF)
circuit.R(26, 'node1', 'node2', 150@u_kOhm)
circuit.C(22, 'node1', circuit.gnd, 22@u_nF)
circuit.R(27, 'node2', 'out', 10@u_kOhm)

# --- Simulation Setup ---
# Create a simulator instance
simulator = circuit.simulator(temperature=25, nominal_temperature=25)

# Perform an AC analysis (frequency sweep)
# from 1 Hz to 1 MHz with 100 points per decade
analysis = simulator.ac(start_frequency=1@u_Hz, 
                        stop_frequency=1@u_MHz, 
                        number_of_points=100, 
                        variation='dec')

# --- Plotting Results ---
# Create a figure with two subplots (for magnitude and phase)
figure, axes = plt.subplots(2, figsize=(10, 8), sharex=True)

# Plot Magnitude (Gain in dB)
axes[0].set_title('Bode Plot - Magnitude')
# The gain in dB is 20 * log10(|V_out/V_in|). Since V_in is 1V, this is 20*log10(|V_out|)
axes[0].semilogx(analysis.frequency, 20 * np.log10(np.absolute(analysis.out)))
axes[0].set_ylabel('Gain [dB]')
axes[0].grid(which='both', linestyle='--')

# Plot Phase
axes[1].set_title('Bode Plot - Phase')
axes[1].semilogx(analysis.frequency, np.angle(analysis.out, deg=True))
axes[1].set_xlabel('Frequency [Hz]')
axes[1].set_ylabel('Phase [°]')
axes[1].grid(which='both', linestyle='--')

# Display the plot
plt.tight_layout()
plt.show()
