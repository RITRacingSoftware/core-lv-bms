#include "BMS.h"
#include "core_config.h"
#include <stm32g4xx_hal.h>

#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"
#include "clock.h"
#include "rtt.h"
#include "can.h"
#include "timeout.h"
#include "boot.h"


#include "AppGPIO.h"
#include "AppCAN.h"
#include "FaultManager.h"
#include "PackMonitor.h"

#include "M17.h"
#include "ADES.h"

const uint8_t num_cells_per_chip[NUM_CHIPS] = NUM_CELLS_PER_CHIP;
const uint8_t num_therms_per_chip[NUM_THERMS] = NUM_THERMS_PER_CHIP;

bool LVBMS_init() 
{
    core_heartbeat_init(LED1_PORT, LED1_PIN);
    core_GPIO_set_heartbeat(true); // this is present in VC init but not BMS init. required? purpose?
    
    if (!core_clock_init()) return false;
    if (!CAN_init()) return false;
    core_boot_init();
    
    core_RTT_init();
    GPIO_init();

    // init SPI?

    if (!M17_init()) return false;
    if (!ADES_init()) return false;

    rprintf("Inits\n");

    core_timeout_start_all(); // present in VC init, what for?
    
    return true;

}

void LVBMS_Task_Update()
{
    PackMonitor_Task_Update();
    CAN_Task_Update();
    GPIO_Task_Update();
}

// toggle heartbeat functions