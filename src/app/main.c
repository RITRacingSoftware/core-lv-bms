#include <stdbool.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "rtt.h"

#include <stm32g4xx_hal.h>

#include "can.h"
#include "clock.h"
#include "gpio.h"
#include "error_handler.h"
#include "boot.h"
#include "core_config.h"

#include "main.h"
#include "AppCAN.h"
#include "BMS.h"
#include "PackMonitor.h"


#define LVBMS_1HZ_PRIORITY (tskIDLE_PRIORITY + 1)
#define LVBMS_100HZ_PRIORITY (tskIDLE_PRIORITY + 1)
#define CAN_TX_PRIORITY (tskIDLE_PRIORITY + 2)
#define HEARTBEAT_PRIORITY (tskIDLE_PRIORITY + 4)

void task_CAN_tx(void *pvParameters)
{
    (void) pvParameters;
    CAN_Task_Update();
    if (!CAN_tx()) hardfault_error_handler();
}

void task_100Hz(void *pvParameters)
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();
    while(true)
    {
        LVBMS_100Hz();
        vTaskDelayUntil(&next_wake_time, 10);
    }
}

void task_1Hz(void *pvParameters)
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();
    while(true)
    {
        LVBMS_1Hz();
        vTaskDelayUntil(&next_wake_time, 1000);
    }
}

void task_heartbeat(void *pvParameters)
{
    (void) pvParameters;
    TickType_t next_wake_time = xTaskGetTickCount();
    while(true) {
        core_GPIO_toggle_heartbeat();
        vTaskDelayUntil(&next_wake_time, 200);
    }
}

int main(void) 
{
    if (!LVBMS_init()) {
        hardfault_error_handler();
    }
    
    rprintf("BMS init");

    int err;
    
    // err = xTaskCreate(task_CAN_tx, // task code
    //     "CAN_tx", // pcName
    //     1000, // uxStackDepth
    //     NULL, // pvParameters
    //     CAN_TX_PRIORITY, // uxPriority
    //     NULL); // pass handle
        
    //     if (err != pdPASS) {
    //         rprintf("CAN task failed created");
    //         hardfault_error_handler();
    //     }
        
    err = xTaskCreate(task_heartbeat,
        "task_heartbeat",
        1000,
        NULL,
        HEARTBEAT_PRIORITY,
        NULL);
        
    if (err != pdPASS) {
        hardfault_error_handler();
    }

        
    err = xTaskCreate(task_100Hz,
        "100hz_task",
        1000,
        NULL,
        LVBMS_100HZ_PRIORITY,
        NULL);
        
    if (err != pdPASS) {
        hardfault_error_handler();
    }

    err = xTaskCreate(task_1Hz,
        "1hz_task",
        1000,
        NULL,
        LVBMS_1HZ_PRIORITY,
        NULL);
    
    if (err != pdPASS){
        hardfault_error_handler();
    }

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    vTaskStartScheduler();


    hardfault_error_handler();

    return 1;
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
        core_GPIO_toggle_heartbeat();
        for (unsigned long long  i = 0; i < 200000; i++);
    }
}

void stack_overflow_error_handler()
{
    while(1)
    {
        core_GPIO_toggle_heartbeat();
        for (unsigned long long  i = 0; i < 400000; i++);
    }
}
