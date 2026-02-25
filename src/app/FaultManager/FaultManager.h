#pragma once

#include <stdint.h>
#include <stdbool.h>

#define FAULT_CELL_VOLT_IRR         0x00000001
#define FAULT_M17                   0x00000002
#define FAULT_ADES                  0x00000004
#define FAULT_VOLTAGE_DIFF          0x00000008
#define FAULT_CHIP_VOLT_IRR         0x00000010
#define FAULT_SUM_VOLT_COMPARISON   0x00000020
#define FAULT_OUT_OF_JUICE          0x00000040
#define FAULT_TEMP_IRR              0x00000080
#define FAULT_PDU_SHUTDOWN          0x00000100

// From core-bms
#define ERR_TX_NOT_RECEIVED                    1
#define ERR_TX_LOOPBACK_MISMATCH               2
#define ERR_NUM_ACTIVE_CHIP_MISMATCH           3
#define ERR_LSSM_BYTE                          4
#define ERR_LSSM_COMM_ERR                      5
#define ERR_ALERT_RX                           6
#define ERR_ALERT_RX_OVERFLOW                  7
#define ERR_LDQ_READ                           8
#define ERR_LDQ_FULL                           9
#define ERR_RX_TIMEOUT                        10
#define ERR_NO_RX                             11
#define ERR_ADES_SCAN_TIMEOUT                 12
#define ERR_RX_STATUS                         13
#define ERR_TX_NOT_WRITTEN                    14
#define ERR_NO_WAKE                           15

void FaultManager_init();
void FaultManager_set_fault(uint64_t faultCode);
void FaultManager_set_err(uint64_t errorCode);
bool FaultManager_read(uint64_t faultCode);
void FaultManager_reset(uint16_t faultCode);

void FaultManager_LSSM(uint8_t lssmByte); // stubbed out