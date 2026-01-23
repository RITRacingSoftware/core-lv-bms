#include "main.h"

#include <stdbool.h>
#include <stdio.h>

#include "can.h"
#include "clock.h"
#include "gpio.h"
#include "error_handler.h" //?
#include "boot.h"
#include "core_config.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <stm32g4xx_hal.h>

#define BMS_100HZ_PRIORITY (tskIDLE_PRIORITY + 1)
#define CAN_RX_PRIORITY (tskIDLE_PRIORITY + 2)
#define CAN_TX_PRIORITY (tskIDLE_PRIORITY + 2)
#define TASK_PRIORITY_HEARTBEAT (tskIDLE_PRIORITY + 4)

MAIN_BUS mainBus = {0};

void hardfault_error_handler();

void task_CAN_tx_main(void)
{
    (void) pvParameters;
    CAN_tx_main();
    if (!CAN_tx_main()) hardfault_error_handler();
}

void task_CAN_rx_main(void *pvParameters)
{
    while(true) {CAN_rx_main();}
}

void task_100Hz(void *pvParameters)
{
    void (*pvParameters)
}

void heartbeat_task(void *pvParameters) 
{
    (void) pvParameters;
    while(true) {
        core_GPIO_toggle_heartbeat();
        vTaskDelay(100 * portTICK_PERIOD_MS);
    }
}

int main(void) 
{
    if (!bms_init)
}

// Called when stack overflows from rtos
// Not needed in header, since included in FreeRTOS-Kernel/include/task.h
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName) 
{
    (void) xTask;
    (void) pcTaskName;

    hardfault_error_handler();
}

void hardfault_error_handler()
{
    while(1)
    {
        toggle_heartbeat();
        for (unsigned long long  i = 0; i < 200000; i++);
    }
}