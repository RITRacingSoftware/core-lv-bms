#pragma once

#include <stdbool.h>

#define CHG_IN_PORT GPIOB
#define CHG_IN_PIN GPIO_PIN_10

#define CHG_ENA_PORT GPIOA
#define CHG_ENA_PIN GPIO_PIN_7

typedef enum  {
    ChargingState_DISCONNECTED,
    ChargingState_CHARGING,
    ChargingState_BALANCING,
    ChargingState_COMPLETE,
    ChargingState_FAULT,
} ChargingState_e;

void ChargeMonitor_init();
void ChargeMonitor_Task_Update();

ChargingState_e getState();