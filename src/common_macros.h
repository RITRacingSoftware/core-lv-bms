#pragma once

#define BYTE(n, msg) (((uint8_t *)&msg)[n])
#define MASK(n) ((1 << n) - 1)

#ifndef BMS_TEST
#define WAIT_TIMEOUT(expr, t, fault, err) do {uint32_t t0=HAL_GetTick(); while(!(expr)) {if (HAL_GetTick()-t0 >= t) {FaultManager_set_err(err); return false;}}} while(0)
#else
#define WAIT_TIMEOUT(expr, t, fault, err)
#endif