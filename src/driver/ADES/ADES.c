#include "ADES.h"

// Standard includes
#include <stdint.h>
#include <stdbool.h>

// Core includes
#include "gpio.h"
#include "rtt.h"

// Common includes
#include "config.h"
#include "common_macros.h"

// App includes
#include "AppGPIO.h"
#include "FaultManager.h"
#include "BMS.h"

// Driver includes
#include "M17.h"

static void poop_loop();

bool ADES_init()
{
    rprintf("Begin ADES init\n");
    rprintf("num_active_chips: %d\n", M17_num_active_chips());
    
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_DEVCFG1, ALIVECNT | ALERTEN | UARTCFG_EXT)) return false;
    rprintf("Init config\n");
    // if (!M17_write_ADES_reg(ADES_WRITEDEV(1), 0x6, 1 << 15)) return false;
    
    // Enable the block voltage measurement and every chip that's activated
    for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_MEASUREEN1, (MASK(NUM_CELLS) | BLOCKEN))) return false;
    }
    rprintf("Init block voltages\n");

    // Enable the thermistor measurements on every chip that's activated
    for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_MEASUREEN2, MASK(NUM_THERMS))) return false;
    }
    rprintf("Init therm voltages\n");

    // Set all AUX/GPIO pins to analog in
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_AUXGPIOCFG, 0x0000)) return false;
    rprintf("Init aux inputs\n");

    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_AUXTIMEREG, 500)) return false;
    rprintf("Init aux settling time\n");


    poop_loop();

    //
    
/*    
    // BALSWEN
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALSWCTRL, (1 << 2))) return false;
    // CBDUTY
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALCTRL, (MASK(4) << 4))) return false;
    // if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALCTRL, (0b011 << 11) | (MASK(4) << 4))) return false;
    // CBEXP1
    if (!M17_write_ADES_reg(ADES_WRITEDEV(0), ADES_BALEXP(0), 0x3FF)) return false;
    // CBMODE
    // core_GPIO_digital_write(LED3_PORT, LED3_PIN, true);
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALCTRL, (0b011 << 11) | (MASK(4) << 4))) return false;
    */
    
    rprintf("Finished ADES init\n");

    return true;
}

bool ADES_collect_all(uint16_t *raw_cell_voltages, uint16_t *raw_chip_voltages, uint16_t *raw_temps)
{
    bool scanning = true;
    uint8_t curr_cell = 0;
    uint16_t scanBuf;
    // Allocate array memory for the biggest possible, which would be all chips for block voltages
    uint16_t rxBuf[NUM_CHIPS] = {0};
    uint32_t t = HAL_GetTick();

    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_SCANCTRL, SCAN)) {__BKPT(0); return false;}

    // Request a scan of the ADCs, for both voltages and temps.
    // Wait until every chip is done scanning and ready to read data
    // while (scanning)
    // {
    //     // Make sure it isn't taking too long to scan
    //     if (HAL_GetTick() - t >= ADES_SCAN_TIMEOUT_MS) {
    //         FaultManager_set_err(ERR_ADES_SCAN_TIMEOUT);
    //         return false;
    //     }
    //     if (!M17_read_ADES_reg(ADES_READALL, ADES_SCANCTRL, &scanBuf, NUM_CHIPS)) {__BKPT(1); return false;}
    //     if (scanBuf & DATARDY) scanning = false;
    // }

    uint8_t voltages_read = 0;
    uint8_t temps_read = 0;
    uint8_t rxLen = 0;
    for (int chip = 0; chip < M17_num_active_chips(); chip++)
    {
        if (!M17_read_ADES_block(chip, ADES_CELLREG(0), raw_cell_voltages + voltages_read, NUM_CELLS)) return false;
        voltages_read += rxLen;  // Increment pointer to how many cells have been read
 
        if (!M17_read_ADES_block(chip, ADES_AUXREG(0), raw_temps + temps_read, NUM_THERMS)) return false;
        temps_read += rxLen;    // Increment pointer to how many temps have been read
    }

    uint16_t rx;

    if (!M17_read_ADES_reg(ADES_READALL, ADES_BLOCKREG, raw_chip_voltages, M17_num_active_chips())) {__BKPT(0); return false;};
    if (!M17_read_ADES_reg(ADES_READDEV(0), ADES_CELLREG(5), &rx, 1));

    rprintf("Raw cell 5: %d\n", rx);
    
    /*
    for (int i = 0; i < NUM_CELLS; i++) {
        rprintf("Raw cell volts %d: %d\n", i, raw_cell_voltages[i]);
    }

    
    for (int i = 0; i < NUM_CELLS; i++) {
        rprintf("Cell %d: %d\n", i, cell_voltages[i]);
    }

    for (int i = 0; i < NUM_CHIPS; i++) {
        rprintf("Chip %d: %d\n", i, chip_voltages[i]);
    }

    for (int i = 0; i < NUM_THERMS; i++) {
        rprintf("Temp: %d: %d\n", i, temps[i]);
    }
    */
}

bool ADES_force_balance(uint8_t chip, uint8_t cell, bool reset)
{
    if (reset) {
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALSWCTRL, (0))) return false;
    }
    else {
        // BALSWEN
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALSWCTRL, (1 << cell))) return false;
    }
    // CBDUTY
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALCTRL, (MASK(4) << 4))) return false;
    // CBEXP1
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALEXP(0), 0x3FF)) return false;
    // CBMODE
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALCTRL, (0b010 << 11) | (MASK(4) << 4))) return false;
}

bool ADES_init_balancing()
{
    // for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        // if !(M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALSWCTRL, )) return false;
    // }

    // Set to balance until cells hit (BAL_UV_THRESHOLD_V), as defined in config.h.
    // Pack the float into a 14bit ADC value, with a reference voltage of 5V
    uint16_t packed_balance_v = (BAL_UV_THRESHOLD_V * ADES_ADC_RANGE)/ADES_CELL_ADC_RANGE_V;

    // Left shift by 2 because the value occupies bits [15:2] of 16bit ADES register
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALAUTOUVTHR, (packed_balance_v << 2))) return false;

    // Set balancing duty cycle (CBDUTY) and enable cell-balancing measurements (CBMEASEN)
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALCTRL, (BAL_DUTY_CYCLE_COEFF << 4) | 0b10 )) return false;

    // When balancing, every nth sample will be replaced with an ADC calibration to keep measurement accurate while pack heats up.
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_BALDLYCTRL, BAL_CAL_PERIOD)) return false;
}

void poop_loop()
{
    for (int chip = 0; chip < NUM_CHIPS; chip++) {
        for (int cell = 0; cell < NUM_CELLS; cell++) {
            ADES_force_balance(chip, cell, false);
            HAL_Delay(POOP_LOOP_DELAY_MS);
            ADES_force_balance(chip, 0, true);
        }
    }
}
