#pragma once

#include "FaultManager.h" // why? from f29BmsConfig.h

// time intervals for things? how does the timing work?
// timing tolerances implemented in driver code?

// CAN

#define NUM_IDS 5 // change val
#define NUM_CELLS
#define NUM_CHIPS 1

#define ADES_CELL_ADC_RANGE_V 5.0f
#define ADES_AUX_ADC_RANGE_V 1.8f
#define ADES_THERM_V // value?

#define CELL_CAPACITY //
#define BATTERY_CAPACITY //

#define CAN_BUS FDCAN1

#define CAN_ERRORS_FRAME_ID 1 // This is a placeholder for the values in CAN errors array in AppCAN.c

#define CELL_VOLTAGE_IRRATIONAL_HIGH 4.5
#define CELL_FULL_VOLATGE_HIGH 4.35
#define CELL_FULL_VOLTAGE_LOW 4.34
#define CELL_EMPTY_VOLTAGE 3.0
#define CELL_VOLTAGE_IRRATIONAL_LOW 2.0

#define CELL_VOLTAGE_DIFFERENCE_IRRATIONAL

#define CHIP_VOLTAGE_IRRATIONAL_HIGH
#define CHIP_VOLTAGE_IRRATIONAL_LOW

#define SUM_VOLT_COMPARE_TOLERANCE

#define CURRENT_IRRATIONAL_HIGH 500
#define OVERCURRENT 350
#define OVERCURRENT_CHARGING -41
#define CURRENT_REJECTION_THRESHOLD // required? from BMS

#define TEMP_IRRATIONAL_HIGH
#define TEMP_IRRATIONAL_LOW
