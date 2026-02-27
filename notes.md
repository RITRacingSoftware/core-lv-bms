/** Tasks:
    - Undervoltage + overcurrent (detected in driver?)
    - Handle startup + shutdown --> look at prev. BMS
*/

# INFO:

## Charging State Machine
- Detect charging? at startup with pin
Charging state:
    - Balancing parameters
- Output state:
Always monitor everything no matter the state


## FaultManager 
- Fault vector: 
    - Bit mask
    - Represents errors/things to do
    - Sent over CAN, communicates to other components for shutting down and such.
- Fault codes:
    - Decimal codes indicating more specific errors than the fault vectors, so that faults can be discerened in deeper layer stacks.
    - Take up half space in fault message.
    - Not implemented anywhere but in core-bms.

## Sampling logic
- ADES_collect_all() takes readings and stores them in STM registers, where they can be  for processing
- How many values per sample?
- Irrationality checking (timeout)
- Overcurr/undervolt/overtemp checking (timeout?)
- Calculate average current/voltage/temp

# QUESTIONS:

- [ ] Copied over the config file because driver code uses it as well.  Changing constant names to those in my code, descriptive but don't align with core-bms style. Should I be changing mine to those in the config file for consistency?

- [ ] Asked to compare sum of cell volts w/ chip volt. Tolerance for difference? Enter in config file

- [ ] How to communicate with charger for charging and discharging?

- [X] Chip vs. pack? e.g. parse voltage functions in core-bms packmonitor.
    - Chip for group of cells, pack for all chips. LV BMS only has 1 chip so chip volt = pack volt

- [X] Borrow BMS test board? (can't write the driver code, so point?)
    - Two power supplies required
    - Cycle power after flashing (keep cells connected)

## Comm to cells (UART?)
- [X] How to control cells?
  - [X] Drain
  - [X] Other stuff?
- [X] Driver function vs. application? Will the driver define functions to control the cells an if so, how high-level?

## BMS logic
- [X] Frequency of monitoring functions? 10Hz? 100Hz?
    - Voltage + temps 10Hz

## CAN:
- [X] Which CAN bus does this communicate over?
    - Secondary
- [X] What data is transmitted over CAN? Same as old BMS code?
    - Voltages, current, temps, fault vectors (at different rate?), charging (yes/no) -- similar to main BMS.
- [X] Is data from the LVBMS packed the same way as the HV by the DBC?
    - New packing functions generated from writing in DBC files (new tool used)

## GPIO
- [X] LED functions except debugging? (x2)
    - Just for debugging

NOTES:
- Keep tolerances in mind. Find in BMS/src/app/DriveMonitor.c in prev. code
