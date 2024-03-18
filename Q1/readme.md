Imagine a situation where you have a circuit able to activate and deactivate a purely resistive load
with a microcontroller. It is required to control the load power over time depending on the situation
and the power cannot be instantly changed from 0% to 100%. Explain in simple words a method
to achieve such effect, looking at both hardware and firmware aspects of the system. If an
inductive load is used instead of a resistive load, can the same triggering method be used or do
special precautions need to be taken?

    A simple solution would be to connect a logic level MOSFET to control load's power through current.
    The method consists on connecting the load between power supply and drain pin, source pin to ground and gate pin
    to the microcontroller's PWM or DAC pin. To attend the no instantaneous power change requisite, the MOSFET must be
    operated in linear's region and a reconstruction filter would be required in the PWM's case.

    This solution requires low effort on firmware, being very straight foward, all it needs is to output on gate pin the desired
    voltage (percentage of VDD) to control the same percent of load's power. Simple control methods might be implemented to 
    smooth transitions between power levels.

    Considering the inductive load, this solution may work under some adjustments, despite not being the most recommended solution.
    As inductive loads works by AC, a sine wave generator parametrized by amplitude and frequency would be mandatory to control the 
    microcontroller's output. 