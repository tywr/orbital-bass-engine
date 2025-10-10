import matplotlib.pyplot as plt
import numpy as np

import PySpice.Logging.Logging as Logging
from PySpice.Spice.Netlist import Circuit
from PySpice.Unit import *

# Setup the logger
logger = Logging.setup_logging()

# --- Create the circuit netlist ---
circuit = Circuit('Two-Stage JFET Amplifier')

# Define the +9V power source
circuit.V('dd', 'vdd', circuit.gnd, 9@u_V)

# Define the input signal source for the simulation.
# For transient analysis, we use a sine wave.
# For AC analysis, this will be treated as an AC source of 1V.
# Let's use a 1kHz sine wave with 100mV amplitude.
circuit.SinusoidalVoltageSource('input', 'in', circuit.gnd, 
                                amplitude=100@u_mV, 
                                frequency=1@u_kHz,
                                dc_offset=0@u_V)

# --- Add Components from the Schematic ---

# Input stage (Q1)
circuit.C(2, 'in', 'node_after_c2', 22@u_nF)
circuit.R(4, 'node_after_c2', 'gate_q1', 100@u_kOhm)
circuit.R(5, 'gate_q1', circuit.gnd, 1@u_MOhm)
circuit.R(6, 'source_q1', circuit.gnd, 3.3@u_kOhm) # 3k3 is 3.3k
circuit.C(3, 'source_q1', circuit.gnd, 220@u_nF)

# Output stage / Active Load (Q2)
circuit.R(9, 'vdd', 'gate_q2', 1@u_MOhm)
circuit.R(10, 'gate_q2', circuit.gnd, 1@u_MOhm)
circuit.C(4, 'gate_q2', 'out', 22@u_nF) # Feedback capacitor

# JFETs - Drain, Gate, Source
# Q1: Drain is 'out', Gate is 'gate_q1', Source is 'source_q1'
# Q2: Drain is 'vdd', Gate is 'gate_q2', Source is 'out'
circuit.J(1, 'out', 'gate_q1', 'source_q1', model='J201')
circuit.J(2, 'vdd', 'gate_q2', 'out', model='J201')

# --- Define the JFET Model ---
# This is a standard SPICE model for the J201 N-Channel JFET.
# Finding the right model is key for accurate simulation.
circuit.model('J201', 'NJF',
    VTO = -1.5,        # Threshold voltage
    BETA = 3e-3,       # Transconductance parameter
    LAMBDA = 2e-3,     # Channel-length modulation
    RD = 20,           # Drain resistance
    RS = 20,           # Source resistance
    CGS = 2.4e-12,     # Gate-source capacitance
    CGD = 2.4e-12,     # Gate-drain capacitance
    PB = 0.5,          # Gate junction potential
    FC = 0.5           # Forward-bias depletion capacitance coefficient
)

# --- Setup the Simulator ---
simulator = circuit.simulator(temperature=25, nominal_temperature=25)


# ==============================================================================
# --- Simulation 1: Transient Analysis ---
# ==============================================================================
print("Running Transient Analysis...")
analysis_transient = simulator.transient(step_time=1@u_us, end_time=3@u_ms)

# --- Plotting Transient Results ---
figure1, ax_transient = plt.subplots(figsize=(10, 6))
ax_transient.set_title('Transient Analysis of JFET Amplifier')
ax_transient.set_xlabel('Time [s]')
ax_transient.set_ylabel('Voltage [V]')
ax_transient.grid(True)
ax_transient.plot(analysis_transient.time, analysis_transient['in'], label='Input Signal (in)')
ax_transient.plot(analysis_transient.time, analysis_transient['out'], label='Output Signal (out)')
ax_transient.legend()
ax_transient.set_ylim(sorted(ax_transient.get_ylim())) # Fix potential inverted y-axis


# ==============================================================================
# --- Simulation 2: AC Analysis (Frequency Response) ---
# ==============================================================================
print("Running AC Analysis...")
# We use a decade sweep from 10 Hz to 1 MHz
analysis_ac = simulator.ac(start_frequency=10@u_Hz, 
                           stop_frequency=1@u_MHz, 
                           number_of_points=100,  
                           variation='dec')

# --- Plotting AC Results ---
figure2, (ax_gain, ax_phase) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

# Plot Gain in dB
ax_gain.set_title('AC Analysis - Frequency Response')
ax_gain.semilogx(analysis_ac.frequency, 20 * np.log10(np.abs(analysis_ac.out)), label='Gain')
ax_gain.set_ylabel('Gain [dB]')
ax_gain.grid(which='both', axis='both')
ax_gain.legend()

# Plot Phase
ax_phase.semilogx(analysis_ac.frequency, np.angle(analysis_ac.out, deg=True), label='Phase', color='orange')
ax_phase.set_xlabel('Frequency [Hz]')
ax_phase.set_ylabel('Phase [°]')
ax_phase.grid(which='both', axis='both')
ax_phase.legend()

# Display plots
plt.tight_layout()
plt.show()
