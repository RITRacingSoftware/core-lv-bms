#include "BMS.h"
#include <stdbool.h>

#include "gpio.h"
#include "clock.h"
#include "rtt.h"

#include "AppGPIO.h"

#include "M17.h"
#include "ADES.h"

/** Tasks:
    - Undervoltage + overcurrent (detected in driver?)
    - Handle startup + shutdown --> look at prev. BMS
*/

int num_readings = 0;

// Function stubs
uint16_t get_voltage(int cell);
uint16_t get_temp(int cell);

bool BMS_100Hz(void)
{ // frequency?

    // if (!bms_init()) return false;
    
}


bool LVBMS_init() 
{

    if (!core_clock_init()) return false;
    core_RTT_init();
    GPIO_init();

    if (!M17_init()) return false;
    if (!ADES_init()) return false;
    
    return true;

}

void LVBMS_Task_Update()
{
    // update BMS stuff
    // update CAN
    // update GPIO
    // USART?
}

bool check_voltage() 
{
    uint16_t volt = get_voltage(num_readings)
    
}

bool check_temp()
{

}

bool check_current() 
{

}

bool check_fault() 
{
    // ?
}

