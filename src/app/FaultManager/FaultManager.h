#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FAULT_CELL_VOLT_IRR         0x00000001
#define FAULT_VOLTAGE_DIFF          0x00000002
#define FAULT_CHIP_VOLT_IRR         0x00000004
#define FAULT_SUM_VOLT_COMPARISON   0x00000008
#define FAULT_OUT_OF_JUICE          0x00000010 // not A?
#define FAULT_TEMP_IRR              0x00000020
#define FAULT_PDU_SHUTDOWN          0x00000040


void FaultManager_init();
void FaultManager_set_fault(uint64_t faultCode);
void FaultManager_set_err(uint64_t errorCode)
bool FaultManager_read(uint64_t faultCode);
void FaultManager_reset(uint16_t faultCode);