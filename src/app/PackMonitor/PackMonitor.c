#include "PackMonitor.h"
#include "timeout.h"
#include "ADES.h"
#include "config.h"

float cell_volts[NUM_CELLS];
float chip_volts[NUM_CHIPS];
float cell_temps[NUM_TEMPS];

// init vars?

float sum_cell_volt; // vars for min & max?
float avg_cell_volt;

float max_cell_volt;
float min_cell_volt;
float cell_volt_diff;

float min_chip_volt;
float max_chip_volt;

float sum_temp; // vars for min & max?
float avg_temp;


static core_timeout_t cell_volt_irr_timeout; // also check cell volt against chip volts (threshold for error?)
static core_timeout_t voltage_diff_timeout;
static core_timeout_t chip_volt_irr_timeout;
static core_timeout_t sum_volt_compare_timeout;
static core_timeout_t out_of_juice_timeout;
static core_timeout_t temp_irr_timeout;

static uint16_t raw_cell_volts[NUM_CELLS];
static uint16_t raw_chip_volts[NUM_CELLS]; // need an array for 1 val?
static uint16_t raw_temps[NUM_TEMPS];

bool PackMonitor_init() { // return bool or void?

    cell_volt_irr_timeout.module = null;
    cell_volt_irr_timeout.ref = FAULT_CELL_VOLT_IRR;
    cell_volt_irr_timeout.timeout = CELL_VOLT_IRR_TIMEOUT_MS;
    cell_volt_irr_timeout.callback = timeout_callback; // timeout_callback?
    cell_volt_irr_timeout.single_shot = 0;
    core_timeout_insert(&cell_volt_irr_timeout);

    voltage_diff_timeout.module = null;
    voltage_diff_timeout.ref = FAULT_VOLTAGE_DIFF;
    voltage_diff_timeout.timeout = VOLTAGE_DIFF_TIMEOUT_MS;
    voltage_diff_timeout.callback = timeout.callback;
    voltage_diff_timeout.single_shot = 0;
    core_timeout_insert(&voltage_diff_timeout)

    chip_volt_irr_timeout.module = null;
    chip_volt_irr_timeout.ref = FAULT_CHIP_VOLT_IRR;
    chip_volt_irr_timeout.timeout = FAULT_CHIP_VOLT_IRR;
    chip_volt_irr_timeout.callback = timeout.callback;
    chip_volt_irr_timeout.single_shot = 0;
    core_timeout_insert(&chip_volt_irr_timeout);

    sum_volt_compare_timout.module = null;
    sum_volt_compare_timeout.ref = FAULT_SUM_VOLT_COMPARISON;
    sum_volt_compare_timeout.timeout = SUM_VOLT_COMPARISON_TIMEOUT;
    sum_volt_compare_timeout.callback = timeout.callback;
    sum_volt_compare_timeout.single_shot = 0;
    core_timeout_insert(&sum_volt_compare_timeout);

    out_of_juice_timeout.module = null;
    out_of_juice_timeout.ref = FAULT_OUT_OF_JUICE;
    out_of_juice_timeout.timeout = OUT_OF_JUICE_TIMEOUT_MS;
    out_of_juice_timeout.callback = timeout_callback;
    out_of_juice_timeout.single_shot = 0;
    core_timeout_insert(&out_of_juice_timeout);

    temp_irr_timeout.module = null;
    temp_irr_timeout.ref = FAULT_TEMP_IRR;
    temp_irr_timeout.timeout = TEMP_IRR_TIMEOUT_MS;
    temp_irr_timeout.callback = timeout_callback;
    temp_irr_timeout.single_shot = 0;
    core_timeout_insert(&temp_irr_timeout);

}

bool PackMonitor_Task_Update() { // return bool?

    cell_volts();
    chip_volt();
    cell_temps();

}

static void cell_volts() {
    bool rational = true;

    for (int i = 0; i < NUM_CELLS; i++) 
    {

        float cell_v = (ADES_CELL_ADC_RANGE_V * (raw_cell_voltages[cell] >> 16));

        if (cell_volt > CELL_IRR_HIGH_V || cell_volt < CELL_IRR_LOW_V) rational = false;
        else {

            cell_volts[i] = cell_volt;

            if (cell_volt > max_cell_volt) max_cell_volt = cell_volt;
            else if (cell_volt < min_cell_volt) min_cell_volt = cell_volt;

            if (cell_volt >= CELL_EMPTY_VOLTAGE) core_timeout_reset(&out_of_juice_timeout);

            sum_cell_volt += cell_volt; // "Add from array so as not to include irrational values." <- ?
        }

        if (rational) core_timeout_reset(&cell_volt_irr_timeout);

    }

    avg_cell_volt = sum_cell_volt/NUM_CELLS;
}

static void cell_volt_diff() 
{
    cell_volt_diff = max_cell_volt - min_cell_volt;
    if (cell_volt_diff <= CELL_VOLTAGE_DIFFERENCE_IRRATIONAL) core_timeout_reset(&voltage_diff_timeout);
}

static void chip_volts() 
{ // compare w/ cell volts
    bool rational = true;

    for (int i = 0; i < NUM_CHIPS; i++) {

        // *From core-bms
        // Convert register read to voltage. 14 bit register, so need to right shift
        // ADC measurement by 14. Uses MSBs of the register, so need to right shift
        // by another 2 bits to get the scaling correct. Multiply by the ADES
        // ADC range.
        float chip_volt = (ADES_VBLK_ADC_RANGE_V * (rxBuf[chip] >> 16)); // chip_volt is called "curr" in core-bms, why?

        if ((chip_volt < CHIP_VOLT_IRRATIONAL_LOW) || (chip_volt > CHIP_VOLT_IRRATIONAL_HIGH)) irrational = true;
        else chip_volts[i] = chip_volt;
    }
}

static void sum_volt_compare() 
{
    if ((sum_cell_volt > (chip_volt - SUM_VOLT_COMPARE_TOLERANCE)) || (sum_cell_volt < (chip_volt + SUM_VOLT_COMPARE_TOLERANCE))) {
        core_timeout_reset(&sum_volt_compare_timeout);
    }
}

static void cell_temps() // core-bms calculates max and min terms, what for?
{
    bool rational = true;

    for (int i = 0; i < NUM_TEMPS; i++) {
        
        // *From core-bms:
        // Convert register read to voltage. 14 bit register, so need to right shift
        // ADC measurement by 14. Uses MSBs of the register, so need to right shift
        // by another 2 bits to get the scaling correct. Multiply by the ADES
        // ADC range
        float voltage = (ADES_AUX_ADC_RANGE_V * (rxBuf[therm] >> 16));
        float res = (voltage * THERM_R1_R)/(ADES_THERM_V - voltage)             // Transfer function for Voltage -> Resistance
        float temp = (3892.0f)/(13.054-logf(THERM_NOMINAL_R/res)) - 273.15f;    // Transfer function for Resistance -> Temperature

        if (temp < (TEMP_IRRATIONAL_LOW) || (temp > (TEMP_IRRATIONAL_HIGH))) rational = false;
        else {
            cell_temps[therm] = temp;
            sum_temp += temp;
        }

        if (rational) core_timeout_reset(&temp_irr_timeout);
    }

    avg_temp = sum_temps/NUM_TEMPS;

}