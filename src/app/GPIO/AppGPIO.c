#include <stdbool.h>

#include "gpio.h"

#include "AppGPIO.h"

// GPIO functions?

void GPIO_init()
{
    core_GPIO_init(HEARTBEAT_PORT, HEARTBEAT_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(LED1_PORT, LED1_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(LED2_PORT, LED2_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);

    core_GPIO_init(STM_ENA_PORT, STM_ENA_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    
    core_GPIO_digital_write(HEARTBEAT_PORT, HEARTBEAT_PIN, false);
    core_GPIO_digital_write(LED1_PORT, LED1_PIN, true);
    core_GPIO_digital_write(LED2_PORT, LED2_PIN, false);
    
    core_GPIO_digital_write(STM_ENA_PORT, STM_ENA_PIN, true);
}

void GPIO_Task_Update()
{
    
}