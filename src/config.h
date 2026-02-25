#pragma once

#include "FaultManager.h" // why? from f29BmsConfig.h

// time intervals for things? how does the timing work?
// timing tolerances implemented in driver code?

// Battery configuration

#define NUM_IDS 5 // value?
#define NUM_CELLS 4
#define NUM_CHIPS 1
#define NUM_TEMPS 4 // value?

// ADES

#define MAX_CELLS_PER_CHIP 14
#define NUM_THERMS_PER_CHIP 6
#define MAX_CHIPS 32
#define ADES_CELL_ADC_RANGE_V 5.0f
#define ADES_THERM_V 1.8f
#define ADES_AUX_ADC_RANGE_V 1.8f
#define ADES_VBLK_ADC_RANGE_V 65.0f

// Capacities

#define CELL_CAPACITY 10 // placeholder val
#define BATTERY_CAPACITY // placeholder val

// CAN

#define CAN_BUS FDCAN1 // placeholder val
#define CAN_ERRORS_FRAME_ID 1 // This is a placeholder for the values in CAN errors array in AppCAN.c

// Voltage/temp values

#define CELL_VOLTAGE_IRRATIONAL_HIGH 4.5
#define CELL_FULL_VOLATGE_HIGH 4.35
#define CELL_FULL_VOLTAGE_LOW 4.34
#define CELL_EMPTY_VOLTAGE 3.0
#define CELL_VOLTAGE_IRRATIONAL_LOW 2.0

#define CELL_VOLTAGE_DIFFERENCE_IRRATIONAL 1 // placeholder val

#define CHIP_VOLTAGE_IRRATIONAL_HIGH 1 // placeholder val
#define CHIP_VOLTAGE_IRRATIONAL_LOW 1 // placeholder val

#define SUM_VOLT_COMPARE_TOLERANCE 1 // placeholder val

#define CURRENT_IRRATIONAL_HIGH 500
#define OVERCURRENT 350
#define OVERCURRENT_CHARGING -41
#define CURRENT_REJECTION_THRESHOLD // required? from BMS

#define TEMP_IRRATIONAL_HIGH 1 // placeholder val
#define TEMP_IRRATIONAL_LOW 1 // placeholder val

#define THERM_R1_R 10000.0f                     // Resistor value of top resistor in thermistor voltage divider
#define THERM_NOMINAL_R 10000.0f                // Nominal resistance of thermistor, usually at 25C

// Timeouts

#define CELL_VOLT_IRR_TIMEOUT_MS 5 // change all values
#define VOLTAGE_DIFF_TIMEOUT_MS 5 
#define CHIP_VOLT_IRR_TIMEOUT_MS 5 
#define SUM_VOLT_COMPARISON_TIMEOUT_MS 5 
#define OUT_OF_JUICE_TIMEOUT_MS 5 
#define TEMP_IRR_TIMEOUT_MS 5 

// Errors from core-bms

#define ERR_TX_NOT_RECEIVED                    1
#define ERR_TX_LOOPBACK_MISMATCH               2
#define ERR_NUM_ACTIVE_CHIP_MISMATCH           3
#define ERR_LSSM_BYTE                          4
#define ERR_LSSM_COMM_ERR                      5
#define ERR_ALERT_RX                           6
#define ERR_ALERT_RX_OVERFLOW                  7
#define ERR_LDQ_READ                           8
#define ERR_LDQ_FULL                           9
#define ERR_RX_TIMEOUT                        10
#define ERR_NO_RX                             11
#define ERR_ADES_SCAN_TIMEOUT                 12
#define ERR_RX_STATUS                         13
#define ERR_TX_NOT_WRITTEN                    14
#define ERR_NO_WAKE                           15


