/*** ARCHITECTURE LAYOUT ***/
#define NUM_CELLS_PER_SEGMENT 2
#define NUM_THERMS_PER_SEGMENT 6 // change?
#define NUM_SEGMENTS 2
#define NUM_CHIPS_PER_SEGMENT 1 // ??
#define NUM_CHIPS (NUM_CHIPS_PER_SEGMENT * NUM_SEGMENTS) // Maximum of 32 for ADES175x
#define NUM_CELLS (NUM_CELLS_PER_SEGMENT * NUM_SEGMENTS)
#define NUM_CELLS_PER_CHIP  {14, 13}
#define NUM_THERMS_PER_CHIP {3, 3}
#define NUM_THERMS (NUM_THERMS_PER_SEGMENT * NUM_SEGMENTS)
#define THERM_R1_R 10000.0f                     // Resistor value of top resistor in thermistor voltage divider
#define THERM_NOMINAL_R 10000.0f                // Nominal resistance of thermistor, usually at 25C

/*** ADES175x CONSTANTS ***/
#define MAX_CELLS_PER_CHIP 14
#define MAX_THERMS_PER_CHIP 6
#define MAX_CHIPS 32
#define ADES_CELL_ADC_RANGE_V 5.0f
#define ADES_THERM_V 1.8f
#define ADES_AUX_ADC_RANGE_V 1.8f
#define ADES_VBLK_ADC_RANGE_V 65.0f
#define ADES_ADC_RANGE 16383.0f

/*** SPI CONFIG ***/
#define SPI_INIT_WAIT_MS 100

/*** CAN CONFIG ***/
#define CAN_SEC FDCAN2

/*** TIMEOUTS ***/
#define ADES_WAKE_TIMEOUT_MS 1000
#define ADES_RESTART_TIME 1000
#define ADES_TX_CONFIRM_TIMEOUT_MS 50           // Amount of time to wait after sending a message to see it in the UART RX queue
#define ADES_TX_TIMEOUT_MS 100                  // Amount of time to wait after sending a register read command to receive the data
#define ADES_RX_TIMEOUT_MS 1000                 // Timeout for receiving a message from the ADES after transmitting a register read
#define ADES_SCAN_TIMEOUT_MS 2000               // Amount of time to wait for the data to be ready after requesting a scan
#define M17_LDQ_TIMEOUT_MS 500
#define OUT_OF_JUICE_TIMEOUT_MS 5000
#define CELL_VOLT_IRR_TIMEOUT_MS 5000
#define TEMP_IRR_TIMEOUT_MS 5000
#define VOLTAGE_DIFF_TIMEOUT_MS 5000
#define CHIP_VOLT_IRR_TIMEOUT_MS 5000           // Timeout for irrational chip voltage (required?)
#define SUM_VOLT_COMPARISON_TIMEOUT_MS 5000     // Timeout for difference too large difference between chip volt and sum cell volts
#define OVERCURRENT_TIMEOUT_MS 5000             // confm val
#define CURRENT_IRR_TIMEOUT_MS 5000

/*** MAX17851 CONFIG ***/
#define M17_MAX_RET 3                           // Maximum number of times the STM can unsuccessfully configure a parameter on the MAX17851

/*** MEASUREMENTS ***/
#define CELL_VOLTAGE_IRRATIONAL_LOW 2.0f
#define CELL_EMPTY_VOLTAGE 3.0f
#define CELL_FULL_MIN_V 4.34f
#define CELL_FULL_MAX_V 4.35f
#define CELL_VOLTAGE_IRRATIONAL_HIGH 4.5f
#define TEMP_IRRATIONAL_LOW 0.0f
#define TEMP_IRRATIONAL_HIGH 100.0f
#define CHIP_VOLTAGE_IRRATIONAL_LOW 30.0f       // Change value from BMS VBLK to LVBMS chip voltage
#define CHIP_VOLTAGE_IRRATIONAL_HIGH 70.0f      // Change value from BMS VBLK to LVBMS chip voltage
#define CELL_VOLTAGE_DIFFERENCE_IRRATIONAL 0.5f
#define SUM_VOLT_COMPARE_TOLERANCE 0.5f         // Ask (in questions)

/*** CURRENT ***/
#define CS_OFFSET_VOLTAGE 2.5f
#define CS_GAIN 250.0f
#define CURRENT_IRRATIONAL_HIGH 500
#define OVERCURRENT_POSITIVE 350.0f              // Max current when not charging (cnfm val)
#define OVERCURRENT_NEGATIVE -41.0f              // Max current when charging (cnfm val)