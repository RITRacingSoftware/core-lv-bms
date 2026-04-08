#pragma once

#include <stdbool.h>

typedef enum  {
    ChargeState_DISCONNECTED,
    ChargeState_CONNECTED,
    ChargeState_CONNECTED_CHARGING,
    ChargeState_CONNECTED_BALANCING,
    ChargeState_CONNECTED_COMPLETE,
    ChargeState_FAULTED
} ChargeState_e;

bool ChargeMonitor_init();
bool ChargeMonitor_task_update();
void ChargeMonitor_set_state(ChargeState_e);
ChargeState_e ChargeMonitor_get_state();
