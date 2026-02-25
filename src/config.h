/*** ARCHITECTURE LAYOUT ***/
#define NUM_CELLS_PER_SEGMENT 4
#define NUM_SEGMENTS 2
#define NUM_CHIPS_PER_SEGMENT 1
#define NUM_CHIPS (NUM_CHIPS_PER_SEGMENT * NUM_SEGMENTS) // Maximum of 32 for ADES175x
#define NUM_CELLS (NUM_CELLS_PER_SEGMENT * NUM_SEGMENTS)
#define NUM_CELLS_PER_CHIP {4, 4}
#define NUM_THERMS_PER_CHIP 3
// #define NUM_THERMS (NUM_THERMS_PER_CHIP * NUM_CHIPS_PER_SEGMENT * NUM_SEGMENTS)
#define NUM_THERMS 6
#define THERM_R1_R 10000.0f                     // Resistor value of top resistor in thermistor voltage divider
#define THERM_NOMINAL_R 10000.0f                        // Nominal resistance of thermistor, usually at 25C

/*** ADES175x CONSTANTS ***/
#define MAX_CELLS_PER_CHIP 14
#define MAX_THERMS_PER_CHIP 6
#define MAX_CHIPS 32
#define ADES_CELL_ADC_RANGE_V 5.0f
#define ADES_THRM_V 1.8f
#define ADES_AUX_ADC_RANGE_V 1.8f
#define ADES_VBLK_ADC_RANGE_V 65.0f

/*** SPI CONFIG ***/
#define M17_SPI SPI2
#define SPI_INIT_WAIT_MS 100

/*** CAN CONFIG ***/
#define CAN_PRI FDCAN1
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
#define CELL_IRR_TIMEOUT_MS 5000

/*** MAX17851 CONFIG ***/
#define M17_MAX_RET 3 // Maximum number of times the STM can unsuccessfully configure a parameter on the MAX17851

/*** MEASUREMENTS ***/
#define CELL_IRR_LOW_V 2.0f
#define CELL_MIN_V 3.0f
#define CELL_FULL_MIN_V 4.34f
#define CELL_FULL_MAX_V 4.35f
#define CELL_IRR_HIGH_V 4.5f
#define TEMP_IRR_LOW_C 0.0f
#define TEMP_IRR_HIGH_C 100.0f
#define VBLK_IRR_LOW_V 30.0f
#define VBLK_IRR_HIGH_V 70.0f