#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> 
#include <math.h>

#include "timeout.h"
#include "rtt.h"

#include "PackMonitor.h"
#include "BMS.h"
#include "ADES.h"
#include "config.h"
#include "FaultManager.h"

float chip_volt;

float max_cell_volt;
float min_cell_volt;
float cell_volt_diff;
float sum_cell_volt;
float avg_cell_volt;

float min_chip_volt;
float max_chip_volt;

float sum_temp; 
float min_temp;
float max_temp;
float avg_temp;

float cell_volt_arr[NUM_CELLS];
float chip_volt_arr[NUM_CHIPS];
float cell_temp_arr[NUM_THERMS];

static uint16_t raw_cell_volts[NUM_CELLS];
static uint16_t raw_chip_volts[NUM_CELLS]; // need an array for 1 val? 
static uint16_t raw_temps[NUM_THERMS];

static core_timeout_t cell_volt_irr_timeout;
static core_timeout_t voltage_diff_timeout;
static core_timeout_t chip_volt_irr_timeout;
static core_timeout_t sum_volt_compare_timeout; // unnecessary
static core_timeout_t out_of_juice_timeout;
static core_timeout_t temp_irr_timeout;

static void timeout_callback (core_timeout_t *timeout);

static void get_cell_volts();
static void get_chip_volt();
static void get_cell_volt_diff();
// static void sum_volt_compare();
static void get_cell_temps();


void PackMonitor_init() 
{
    cell_volt_irr_timeout.module = NULL;
    cell_volt_irr_timeout.ref = FAULT_CELL_VOLT_IRR;
    cell_volt_irr_timeout.timeout = CELL_VOLT_IRR_TIMEOUT_MS;
    cell_volt_irr_timeout.callback = timeout_callback;
    cell_volt_irr_timeout.single_shot = 0;
    core_timeout_insert(&cell_volt_irr_timeout);

    voltage_diff_timeout.module = NULL;
    voltage_diff_timeout.ref = FAULT_VOLTAGE_DIFF;
    voltage_diff_timeout.timeout = VOLTAGE_DIFF_TIMEOUT_MS;
    voltage_diff_timeout.callback = timeout_callback;
    voltage_diff_timeout.single_shot = 0;
    core_timeout_insert(&voltage_diff_timeout);

    chip_volt_irr_timeout.module = NULL;
    chip_volt_irr_timeout.ref = FAULT_CHIP_VOLT_IRR;
    chip_volt_irr_timeout.timeout = CHIP_VOLT_IRR_TIMEOUT_MS;
    chip_volt_irr_timeout.callback = timeout_callback;
    chip_volt_irr_timeout.single_shot = 0;
    core_timeout_insert(&chip_volt_irr_timeout);

    sum_volt_compare_timeout.module = NULL;
    sum_volt_compare_timeout.ref = FAULT_SUM_VOLT_COMPARISON;
    sum_volt_compare_timeout.timeout = SUM_VOLT_COMPARISON_TIMEOUT_MS;
    sum_volt_compare_timeout.callback = timeout_callback;
    sum_volt_compare_timeout.single_shot = 0;
    core_timeout_insert(&sum_volt_compare_timeout);

    out_of_juice_timeout.module = NULL;
    out_of_juice_timeout.ref = FAULT_OUT_OF_JUICE;
    out_of_juice_timeout.timeout = OUT_OF_JUICE_TIMEOUT_MS;
    out_of_juice_timeout.callback = timeout_callback;
    out_of_juice_timeout.single_shot = 0;
    core_timeout_insert(&out_of_juice_timeout);

    temp_irr_timeout.module = NULL;
    temp_irr_timeout.ref = FAULT_TEMP_IRR;
    temp_irr_timeout.timeout = TEMP_IRR_TIMEOUT_MS;
    temp_irr_timeout.callback = timeout_callback;
    temp_irr_timeout.single_shot = 0;
    core_timeout_insert(&temp_irr_timeout);

}

void PackMonitor_Task_Update() 
{
    get_cell_volts();
    get_chip_volt();
    get_cell_volt_diff();
    // sum_volt_compare();
    get_cell_temps();
}

static void timeout_callback (core_timeout_t *timeout)
{
    FaultManager_set_fault(timeout->ref);
}

static void get_cell_volts() 
{

    bool rational = true;
    float cell_volt = 0.0f;

    for (int cell = 0; cell < NUM_CELLS; cell++) 
    {

        cell_volt = (ADES_CELL_ADC_RANGE_V * (raw_cell_volts[cell] >> 2)/ADES_ADC_RANGE);

        rprintf("Cell %d: %d\n", cell, (int)(cell_volt * 1000));

        if (cell_volt > CELL_VOLTAGE_IRRATIONAL_HIGH || cell_volt < CELL_VOLTAGE_IRRATIONAL_LOW) rational = false;
        else {

            cell_volt_arr[cell] = cell_volt;

            if (cell_volt > max_cell_volt) max_cell_volt = cell_volt;
            else if (cell_volt < min_cell_volt) min_cell_volt = cell_volt;

            if (cell_volt >= CELL_EMPTY_VOLTAGE) core_timeout_reset(&out_of_juice_timeout);

        }
        
        sum_cell_volt += cell_volt;
        
    }

    rprintf("----------------------------------------\n\n");

    if (rational) core_timeout_reset(&cell_volt_irr_timeout);

    avg_cell_volt = sum_cell_volt/NUM_CELLS;
}

static void get_cell_volt_diff() 
{
    float cell_volt_diff = 0.0f;
    cell_volt_diff = max_cell_volt - min_cell_volt;
    if (cell_volt_diff <= CELL_VOLTAGE_DIFFERENCE_IRRATIONAL) core_timeout_reset(&voltage_diff_timeout);
}

static void get_chip_volt()
{ 
    bool rational = true;

        chip_volt = (ADES_VBLK_ADC_RANGE_V * (raw_chip_volts[0] >> 2));

        rprintf("Chip %d: %d\n", (int)(chip_volt * 1000));

        if ((chip_volt < CHIP_VOLTAGE_IRRATIONAL_LOW) || (chip_volt > CHIP_VOLTAGE_IRRATIONAL_HIGH)) rational = false;
        else chip_volt_arr[0] = chip_volt;
        
        rprintf("----------------------------------------\n\n");
}

// static void sum_volt_compare() 
// {
//     if ((sum_cell_volt > (chip_volt - SUM_VOLT_COMPARE_TOLERANCE)) || (sum_cell_volt < (chip_volt + SUM_VOLT_COMPARE_TOLERANCE))) core_timeout_reset(&sum_volt_compare_timeout);
// }

static void get_cell_temps()
{
    bool rational = true;

    for (int therm = 0; therm < NUM_THERMS; therm++) {
        
        // *From core-bms:
        // Convert register read to voltage. 14 bit register, so need to right shift
        // ADC measurement by 14. Uses MSBs of the register, so need to right shift
        // by another 2 bits to get the scaling correct. Multiply by the ADES
        // ADC range
        float voltage = (ADES_AUX_ADC_RANGE_V * (raw_temps[0] >> 2)/ADES_ADC_RANGE);
        float res = (voltage * THERM_R1_R)/(ADES_THERM_V - voltage);           // Transfer function for Voltage -> Resistance
        float temp = (3892.0f)/(13.054-logf(THERM_NOMINAL_R/res)) - 273.15f;    // Transfer function for Resistance -> Temperature
        rprintf("Therm %d: %d\n", therm, raw_temps[therm]);

        if (temp < (TEMP_IRRATIONAL_LOW) || (temp > (TEMP_IRRATIONAL_HIGH))) rational = false;
        else {
            cell_temp_arr[therm] = temp;
            if (temp > max_temp) max_temp = temp;
            else if (temp < min_temp) min_temp = temp;
            sum_temp += temp;
        }

    }

    if (rational) core_timeout_reset(&temp_irr_timeout);
    avg_temp = sum_temp/NUM_THERMS;
    rprintf("----------------------------------------\n\n");

}