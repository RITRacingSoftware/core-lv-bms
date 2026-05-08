#pragma once

#include <stdbool.h>
#include "sensor_dbc.h"
#include "main_dbc.h"
#include "ChargeMonitor.h"

void CAN_error_handler();
bool CAN_init();
bool CAN_tx();
bool CAN_send_pack_data();
bool CAN_pack_and_send_current(float current);
bool CAN_send_faults_errors(uint32_t faultList, uint32_t errorCodes);

typedef struct 
{
    struct sensor_dbc_lvbms_overview_d cell_overview;
    struct sensor_dbc_lvbms_fault_vector_d fault_vector;
    struct sensor_dbc_lvbms_error_codes_d error_codes;
    struct sensor_dbc_lvbms_current_d current;
    struct sensor_dbc_lvbms_pack_data_d pack_data;
    struct sensor_dbc_lvbms_voltages_d voltages;
    struct sensor_dbc_lvbms_temperatures_d temperatures;
} SEC_BUS;

extern SEC_BUS sec_bus;