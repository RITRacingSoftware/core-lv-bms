#include "AppGPIO"
#include "gpio.h"


#include <stdbool.h>

// GPIO functions?

bool GPIO_init()
{
    core_GPIO_init(LED1_PORT, LED1_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);
    core_GPIO_init(LED2_PORT, lED2_PIN, GPIO_MODE_OUTPUT_PP, GPIO_PULLDOWN);

    core_GPIO_digital_write(LED1_PORT, LED1_PIN, false);
    core_GPIO_digital_write(LED2_PORT, LED2_PIN, false);

}

void GPIO_Task_Update()
{
    
}