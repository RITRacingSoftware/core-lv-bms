#pragma once

#include <stdbool.h>

void CAN_error_handler();
bool CAN_init();
bool CAN_tx();
void CAN_task_update();

