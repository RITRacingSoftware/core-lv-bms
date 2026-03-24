main
- Create tasks for all functions

BMS

PackMonitor
- Monitors voltage
- Monitors temp
- Fault if overtemp
- Fault if undervoltage

FaultManager
- ! Sets faults
- Header file stores fault codes

CAN
- Sends messages intermittently with cell/chip info and fault/err vectors

GPIO
- Init user LEDs