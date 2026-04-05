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

#include "config.h"
#include "AppGPIO.h"
#include "AppCAN.h"
#include "FaultManager.h"
#include "PackMonitor.h"
#include "CurrentMonitor.h"
#include "ChargeMonitor.h"

#include "M17.h"
#include "ADES.h"

bool LVBMS_init()
{
    core_heartbeat_init(HEARTBEAT_PORT, HEARTBEAT_PIN);
    core_GPIO_set_heartbeat(true);
    
    if (!core_clock_init()){
        return false;
    }    
    
    // core_boot_init();
    
    core_RTT_init();

    GPIO_init();

    if (!M17_init()) return false;

    if (!ADES_init()) return false;

    core_timeout_start_all();
    
    return true;


}

void LVBMS_1Hz()
{
    PackMonitor_Task_Update();
    CurrentMonitor_Task_Update();
}

void LVBMS_100Hz()
{
    // PackMonitor_Task_Update();
    // ChargeMonitor_Task_Update();
    GPIO_Task_Update();
}