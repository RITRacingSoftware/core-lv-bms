/** Tasks:
    - Undervoltage + overcurrent (detected in driver?)
    - Handle startup + shutdown --> look at prev. BMS
*/

# INFO:

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

## PackMonitor Logic

floats:
- cell voltage array
- chip voltage array
- temp array
- max temp
- min temp
- avg temp
- max cell
- min cell
- avg cell
- pack volt
- temp sum
- cell volt sum

core timeouts:
- cell irr (separate for curr and volt?)
- temp irr
- out of juice

uint16_t:
- raw cell voltage array
- raw chip voltage array
- raw temps array

init pack monitor:
- setup timeouts

pack monitor task update:
- set pack voltage to 0 (why here?)
- collect all
- call parse functions (no return, bool true/false for confirmation?)
- calculate avg cell sum
- calculate avg temp sum
- call CAN send data function
- set sums to 0

parse cell voltages:
- set rational boolean to true (local)
- iterate through cells by cell number (0 to cell number global const)
- parse cell voltage
- check rationality + set flag for false
- if true:
    - set max volt var to that cell volt
    - also same for min cell
    - check that it's greater than empty and reset the out of juice timeout


parse temps:
- set irrational if irrational


# QUESTIONS:

- [X] Chip vs. pack? e.g. parse voltage functions in core-bms packmonitor.
    - Chip for group of cells, pack for all chips. LV BMS only has 1 chip so chip volt = pack volt

- [X] Borrow BMS test board? (can't write the driver code, so point?)
    - Two power supplies required
    - Cycle power after flashing (keep cells connected)

## Comm to cells (UART?)
- [X] How to control cells?
  - [X] Drain
  - [X] Other stuff?
- [X] Driver function vs. application? Will the driver define functions to ontrol the cells an if so, how high-level?

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

1. Driver responsibilities 

The driver should do ONLY these things:

 ** Initialize the chips**

```c
ADES_init();
```

 ** Periodically trigger a scan**

```c
ADES_collect_all();
```

 ** Store the fresh results internally**

```c
static uint16_t voltages[NUM_CELLS];
static uint16_t temps[NUM_THERMS];
```

 ** Expose read-only getters**
    
Not direct array access, but safe functions:

```c
uint16_t ADES_get_voltage(int cell);
uint16_t ADES_get_temp(int therm);
int ADES_get_num_cells(void);
int ADES_get_num_therms(void);
```

This prevents the app code from depending on internals (good architecture).

---

What the app should NOT do

Your application **must NOT directly access**:

* `voltages[]`
* `temps[]`
* `num_cells_per_chip[]`
* Internal scaling logic
* Raw register reads

That would break layering.

---

2. Add these getter functions to the driver

 **In `ADES.h`:**

```c
uint16_t ADES_get_voltage(int cell);   // returns mV
uint16_t ADES_get_temp(int therm);     // returns mV or °C later
bool ADES_data_valid(void);            // optional: data freshness check
```

 **In `ADES.c`:**

```c
uint16_t ADES_get_voltage(int cell)
{
    if (cell < 0 || cell >= NUM_CELLS) return 0xFFFF; // invalid
    return voltages[cell];
}

uint16_t ADES_get_temp(int therm)
{
    if (therm < 0 || therm >= NUM_THERMS) return 0xFFFF;
    return temps[therm];
}
```

These will give your app layer stable, safe access.

---

3. How the app layer should access data

 You should have a 10–100 ms task like:

```c
void BatteryMonitor_task(void)
{
    // Trigger one full measurement
    ADES_collect_all();

    // Process results
    check_voltages();
    check_temps();
    check_current();
    check_for_faults();
    control_charging();
    send_can_messages();
}
```

---

4. How to implement each of your tasks

---

 **TASK 1: Monitor current / overcurrent**

Current probably comes from:

* INA233
* Shunt read through ADC
* ADES block voltage (difference over time)

Wherever current is read, your loop becomes:

```c
int16_t I = Current_get_mA();

if (I > CURRENT_LIMIT_CHARGE || I < -CURRENT_LIMIT_DISCHARGE) {
    FaultManager_set_err(ERR_OVERCURRENT);
}
```

---

 **TASK 2: Monitor cell voltages + irrational behavior**

```c
for (int i = 0; i < ADES_get_num_cells(); i++)
{
    uint16_t v = ADES_get_voltage(i);

    if (v == 0xFFFF) {
        FaultManager_set_err(ERR_BAD_VOLTAGE_READ);
        continue;
    }

    if (v < CELL_UV_THRESHOLD) {
        FaultManager_set_err(ERR_CELL_UNDERVOLT);
    }
    if (v > CELL_OV_THRESHOLD) {
        FaultManager_set_err(ERR_CELL_OVERVOLT);
    }

    // Irrational check: sudden jump
    if (abs(v - previous_voltages[i]) > IRRATIONAL_JUMP_THRESHOLD) {
        FaultManager_set_err(ERR_CELL_IRRATIONAL);
    }

    previous_voltages[i] = v;
}
```

---

 **TASK 3: Monitor temps + irrational values**

```c
for (int i = 0; i < ADES_get_num_therms(); i++)
{
    uint16_t t_mv = ADES_get_temp(i);

    if (t_mv == 0xFFFF) {
        FaultManager_set_err(ERR_BAD_TEMP_READ);
        continue;
    }

    float tempC = convert_thermistor(t_mv);

    if (tempC > MAX_TEMP_C) {
        FaultManager_set_err(ERR_OVERTEMP);
    }

    if (abs(tempC - previous_temp[i]) > IRRATIONAL_TEMP_JUMP) {
        FaultManager_set_err(ERR_TEMP_IRRATIONAL);
    }
}
```

---

 **TASK 4: Charging control logic**

Charging enable depends on:

* No faults
* Cells not full
* Temperature ok
* Input source detected (pack vs external)

Example:

```c
void ChargingControl_update(void)
{
    if (FaultManager_has_fault()) {
        Charger_disable();
        return;
    }

    if (all_cells_full()) {
        Charger_disable();
        return;
    }

    if (!external_source_present()) {
        Charger_disable();
        return;
    }

    Charger_enable();
}
```

---

 **TASK 5: Send meaningful CAN values**

Your CAN task should *never* read raw driver arrays — always go through getters.

Example:

```c
CANFrame_BMS_Status_t frame;

for (int i = 0; i < NUM_CELLS; i++)
    frame.cell_mv[i] = ADES_get_voltage(i);

for (int i = 0; i < NUM_THERMS; i++)
    frame.temp_C[i] = convert_thermistor(ADES_get_temp(i));

frame.pack_current_mA = Current_get_mA();

CAN_send(BMS_STATUS_ID, &frame, sizeof(frame));
```

---

 **TASK 6: Handle shutdown**

Triggered by:

* any fault
* pack unplug
* button press
* voltage < safe threshold

```c
if (FaultManager_has_fault()) {
    shutdown_system();
}
```

---

 **TASK 7: Detect / handle startup**

Example:

```c
void BMS_startup_sequence(void)
{
    if (!ADES_init())
        FaultManager_set_err(ERR_ADES_INIT_FAIL);

    delay(10);

    ADES_collect_all(); // first scan

    if (!voltages_valid_on_startup())
        FaultManager_set_err(ERR_STARTUP_VOLTAGE);
}
```

