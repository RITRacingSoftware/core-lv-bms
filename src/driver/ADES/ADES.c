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

bool ADES_init()
{
    rprintf("Begin ADES init\n");
    rprintf("num_active_chips: %d\n", M17_num_active_chips());
    
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_DEVCFG1, ALIVECNT | ALERTEN | UARTCFG_DIFF)) return false;
    rprintf("Init config\n");
    // if (!M17_write_ADES_reg(ADES_WRITEDEV(1), 0x6, 1 << 15)) return false;
    
    // Enable the block voltage measurement and every chip that's activated
    for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_MEASUREEN1, (MASK(num_cells_per_chip[chip]) | BLOCKEN))) return false;
    }
    rprintf("Init block voltages\n");

    // Enable the thermistor measurements on every chip that's activated
    for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_MEASUREEN2, MASK(num_therms_per_chip[chip]))) return false;
    }
    rprintf("Init therm voltages\n");

    // Set all AUX/GPIO pins to analog in
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_AUXGPIOCFG, 0x0000)) return false;
    rprintf("Init aux inputs\n");
    
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
    uint16_t scanBuf[M17_num_active_chips()];
    // Allocate array memory for the biggest possible, which would be all chips for block voltages
    uint16_t rxBuf[NUM_CHIPS] = {0};
    uint32_t t = HAL_GetTick();

    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_SCANCTRL, SCAN)) return false;

    // Request a scan of the ADCs, for both voltages and temps.
    // Wait until every chip is done scanning and ready to read data
    while (!scanning)
    {
        // Make sure it isn't taking too long to scan
        if (HAL_GetTick() - t >= ADES_SCAN_TIMEOUT_MS) {
            FaultManager_set_err(ERR_ADES_SCAN_TIMEOUT);
            return false;
        }
        if (!M17_read_ADES_reg(ADES_READALL, ADES_SCANCTRL, scanBuf, M17_num_active_chips())) return false;
        for (int chip = 0; chip < M17_num_active_chips(); chip++) {
            if (!(scanBuf[chip] & DATARDY)) scanning = false;
        }
    }

    uint8_t voltages_read = 0;
    uint8_t temps_read = 0;
    uint8_t rxLen = 0;
    for (int chip = 0; chip < M17_num_active_chips(); chip++)
    {
        rxLen = num_cells_per_chip[chip];
        if (!M17_read_ADES_block(chip, ADES_CELLREG(0), raw_cell_voltages + voltages_read, rxLen)) return false;
        voltages_read += rxLen;  // Increment pointer to how many cells have been read
        
        rxLen = num_therms_per_chip[chip];
        if (!M17_read_ADES_block(chip, ADES_AUXREG(0), raw_temps + temps_read, rxLen)) return false;
        temps_read += rxLen;    // Increment pointer to how many temps have been read
    }

    uint16_t reversed_raw_chips[M17_num_active_chips()];
    if (!M17_read_ADES_reg(ADES_READALL, ADES_BLOCKREG, reversed_raw_chips, M17_num_active_chips())) return false;
    for (int i = 0; i < M17_num_active_chips(); i++) {
        raw_chip_voltages[i] = reversed_raw_chips[M17_num_active_chips() - 1 - i];
    }

    /*
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

bool ADES_force_balance(uint8_t chip, uint8_t cell)
{
    // BALSWEN
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALSWCTRL, (1 << cell))) return false;
    // CBDUTY
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALCTRL, (MASK(4) << 4))) return false;
    // CBEXP1
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALEXP(0), 0x3FF)) return false;
    // CBMODE
    if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_BALCTRL, (0b010 << 11) | (MASK(4) << 4))) return false;
}
