#include <stdbool.h>

#include "gpio.h"

#include "AppGPIO.h"

void GPIO_init()
{
    core_GPIO_init(LED1_PORT, LED1_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(LED2_PORT, LED2_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(STM_ENA_PORT, STM_ENA_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(LV_ENA_PORT, LV_ENA_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(CHG_ENA_PORT, CHG_ENA_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN); // where in BMS code? Actually for enabling charging?
    core_GPIO_init(CHG_IN_PORT, CHG_IN_PIN, GPIO_MODE_INPUT, GPIO_PULLDOWN);
    
    core_GPIO_digital_write(LED1_PORT, LED1_PIN, false);
    core_GPIO_digital_write(LED2_PORT, LED2_PIN, false);
    core_GPIO_digital_write(CHG_ENA_PORT, CHG_ENA_PIN, false);
    core_GPIO_digital_write(LV_ENA_PORT, LV_ENA_PIN, true);
    core_GPIO_digital_write(STM_ENA_PORT, STM_ENA_PIN, true);
}

void GPIO_Task_Update()
{
    
}

void GPIO_set_shutdown_pin(bool state) { core_GPIO_digital_write(LV_ENA_PIN, LV_ENA_PIN, state); }
