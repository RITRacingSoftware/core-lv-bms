#include "ADES.h"
#include <stdint.h>
#include <stdbool.h>

#include "stm32g4xx_hal.h"
#include "gpio.h"
#include "AppGPIO.h"

#include "config.h"
#include "FaultManager.h"
#include "rtt.h"
#include "M17.h"

static bool ADES_collect_voltages();
static bool ADES_collect_temps();

const uint8_t num_cells_per_chip[NUM_CHIPS] = NUM_CELLS_PER_CHIP;

uint16_t voltages[NUM_CELLS];
uint16_t temps[NUM_THERMS];

bool ADES_init()
{
    rprintf("Begin ADES init\n");
    rprintf("num_active_chips: %d\n", M17_num_active_chips());
    
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_DEVCFG1, ALIVECNT)) return false;


    // Enable the block voltage measurement and every chip that's activated
    for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        if (!M17_write_ADES_reg(ADES_WRITEDEV(chip), ADES_MEASUREEN1, ((1 << num_cells_per_chip[chip]) - 1 ) | BLOCKEN)) return false;
    }

    // Enable thermistor measurements
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_MEASUREEN2, 0x003F)) return false;
    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_AUXGPIOCFG, 0x00)) return false;
    

    uint16_t rxBuf[2];
    rprintf("Finished ADES init\n");

    return true;
}

bool ADES_collect_all()
{
    bool ready = false;
    uint8_t curr_cell = 0;
    uint16_t rxBuf;
    uint32_t t = HAL_GetTick();

    if (!M17_write_ADES_reg(ADES_WRITEALL, ADES_SCANCTRL, SCAN)) return false;

    while (!ready)
    {
        if (HAL_GetTick() - t >= ADES_SCAN_TIMEOUT_MS) {
            FaultManager_set_err(ERR_ADES_SCAN_TIMEOUT);
            return false;
        }
        if (!M17_read_ADES_reg(ADES_READALL, ADES_SCANCTRL, &rxBuf, 1)) return false;
        ready = rxBuf & DATARDY;
    }

    rprintf("Scan complete\n");

    /*
    if (!M17_read_ADES_reg(ADES_READDEV(0), ADES_CELLREG(0), &rxBuf, 1)) return false;
    voltages[curr_cell] = (5000L * rxBuf >> 16);
    if (!M17_read_ADES_reg(ADES_READDEV(0), ADES_AUXREG(0), temps, 1)) return false;
    */


    // Collect all voltages and temps
    for (int chip = 0; chip < M17_num_active_chips(); chip++) {
        for (int cell = 0; cell < num_cells_per_chip[cell]; cell++) {
            if (!M17_read_ADES_reg(ADES_READDEV(chip), ADES_CELLREG(cell), &rxBuf, 1)) return false;
            voltages[curr_cell] = ((5000 * rxBuf) >> 16);
            curr_cell++;
        }
        for (int therm = 0; therm < NUM_THERMS_PER_CHIP; therm++) {
            if (!M17_read_ADES_reg(ADES_READDEV(chip), ADES_AUXREG(therm), &rxBuf, 1)) return false;
            temps[(chip * NUM_THERMS_PER_CHIP) + therm] = ((1800 * rxBuf) >> 16);
        }
    }

    // ADES_collect_voltages();
    for (int i = 0; i < NUM_CELLS; i++) {
        rprintf("Cell %d: %d\n", i, voltages[i]);
    }

    for (int i = 0; i < NUM_THERMS; i++) {
        rprintf("Temp: %d: %d\n", i, temps[i]);
    }
}

static bool ADES_collect_voltages()
{
    if (!M17_read_ADES_reg(ADES_READALL, ADES_CELLREG(0), voltages, 1)) return false;
    if (!M17_read_ADES_reg(ADES_READALL, ADES_CELLREG(1), voltages + 1, 1)) return false;
    if (!M17_read_ADES_reg(ADES_READALL, ADES_CELLREG(2), voltages + 2, 1)) return false;
    if (!M17_read_ADES_reg(ADES_READALL, ADES_CELLREG(3), voltages + 3, 1)) return false;
    for (int i = 0; i < NUM_CELLS; i++)
    {
        rprintf("Cell %d: %d\n", i, (5000L*voltages[i])>>16 );
    }
    return true;
}

static bool ADES_collect_temps()
{
    if (!M17_read_ADES_reg(ADES_READALL, ADES_AUXREG(0), temps, 1)) return false;
    if (!M17_read_ADES_reg(ADES_READALL, ADES_AUXREG(1), temps + 1, 1)) return false;
    if (!M17_read_ADES_reg(ADES_READALL, ADES_AUXREG(2), temps + 2, 1)) return false;
    for (int i = 0; i < 3; i++)
    {
        rprintf("Temp: %d: %d\n", i, (temps[i]));
    }
}
