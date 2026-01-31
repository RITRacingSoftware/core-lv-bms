#include "M17.h"
#include <stdbool.h>
#include <stdint.h>

#include "spi.h"
#include "rtt.h"
#include "gpio.h"

#include "config.h"
#include "common_macros.h"
#include "FaultManager.h"
#include "AppGPIO.h"
#include "ADES.h"

#ifdef BMS_TEST
#include <stdlib.h>
#include <stdio.h>
#endif

static uint8_t num_active_chips;


static bool transmit_ADES_message_raw(uint8_t *msg, uint8_t len);
static bool wake_daisy_chain();
static bool command_hello_all();
static uint8_t calculate_PEC(uint8_t *msg, uint8_t len);
static bool check_rx_errors();
static void send_M17_command_1byte(uint8_t addr);
static void send_M17_command_2byte(uint8_t addr);
static bool reg_write(uint8_t addr, uint8_t msg);
static uint8_t reg_read(uint8_t addr);
static bool read_rx_buf(uint8_t *rxBuf, uint8_t len);

bool M17_init()
{
    num_active_chips = 0;
    core_SPI_init(M17_SPI, CS_PORT, CS_PIN);

    /*** ADES RESTART ***/ 
    /* The ADES chips are kept alive by the SHDNL signal, which is connected to a capacitor
     * and is charged via the UART input. When SHDNL drops too low, the ADES chip goes into shutdown
     * to preserve battery life. If the ADES is already initialized and we try to go through the
     * initialization sequence again it will error out. This is the case if we initialize, then 
     * reset the STM but keep power on for the M17 chip. To avoid this, we turn off the keep-alive
     * from the M17 and wait for ADES_RESTART_TIME to ensure the chip is asleep before we try to 
     * initialize again.
     */
    // if (!reg_write(M17_CONFIG_GEN3, 0x0F)) return false; // Disable keep-alive
    unsigned long ADES_restartTime = HAL_GetTick();
    while (HAL_GetTick() - ADES_restartTime < ADES_RESTART_TIME) {}

    /*** M17 CONFIG ***/
    // if (!reg_write(M17_CONFIG_GEN0, 1)) return false;                                    // CONFIG_GEN0 - Set number of chips
    if (!reg_write(M17_CONFIG_GEN1, BAUD_RATE_2M)) return false;                                    // CONFIG_GEN1 - Set baudrate to 2Mbps and differential UART
    if (!reg_write(M17_CONFIG_GEN3, 0x0101)) return false;                         // CONFIG_GEN3 - Set keep-alive to 1.28ms
    if (!reg_write(M17_CONFIG_GEN4, MS_EN_MASTER_SINGLE_UART | DC_EN_DATA_RX | ALIVECOUNT_EN)) return false;     // CONFIG_GEN4 - Set single UART, enable data check byte
    if (!reg_write(M17_CONFIG_GEN5, ALRTPCKT_DBNC_16)) return false;                                // CONFIG_GEN5 - Allow 16 consecutive errors before error is flagged
    if (!reg_write(M17_CONFIG_COMM, COMM_TO_DLY_DISABLE)) return false;                             // CONFIG_COMM - Disable comm timeout
    if (!check_rx_errors()) return false;

    if (!wake_daisy_chain()) return false;

    send_M17_command_2byte(M17_CLR_RXBUF);
    send_M17_command_2byte(M17_CLR_TXBUF);
    if (!command_hello_all()) return false;
    if (!check_rx_errors()) return false;

    return true;
}

bool M17_write_ADES_reg(uint8_t dest, uint8_t reg_addr, uint16_t msg)
{
    // Transmit command
    uint8_t txBuf[7];
    txBuf[0] = 6;                                   // Message length, it is always 6
    txBuf[1] = dest;                                // Which device to send to
    txBuf[2] = reg_addr;                            // Which register to write to
    txBuf[3] = BYTE(0, msg);                        // LSB of message
    txBuf[4] = BYTE(1, msg);                        // MSB of message
    txBuf[5] = calculate_PEC(txBuf + 1, 4);         // PEC byte for txBuf[1] -> txBuf[4]
    txBuf[6] = 0x00;                                // Alive byte

    /*
    rprintf("Write begin\n");
    for (int i = 0; i < 7; i++) { rprintf("%d, %02x\n", i, txBuf[i]); }
    */
    if (!transmit_ADES_message_raw(txBuf, 7)) return false;

    // Receive transmission
    uint8_t tempBuf[6];
    if (!read_rx_buf(tempBuf, 6)) return false;
    if (!check_rx_errors()) return false;
    /*
    rprintf("Read1 begin\n");
    for (int i = 0; i < 7; i++) { rprintf("%d, %02x\n", i, tempBuf[i]); }
    */

    /*
    // Check the register was actually written to
    uint8_t rxLen = 1;
    if (dest == ADES_WRITEALL) rxLen = num_active_chips;
    else rxLen = 1;
    uint16_t rxBuf[rxLen];
    if (!M17_read_ADES_reg(ADES_READALL, reg_addr, rxBuf, rxLen)) return false;
    rprintf("Read2 begin\n");
    for (int i = 0; i < rxLen; i++) { rprintf("%d, %04x\n", i, rxBuf[i]); }

    // Check all of the registers against what should be there
    for (int i = 0; i < rxLen; i++) {
        if (rxBuf[i] != msg) return false;
    }
    */

    return true;
}

bool M17_read_ADES_reg(uint8_t dest, uint8_t reg_addr, uint16_t *rxBuf, uint8_t rxLen)
{
    uint8_t fullLen = (rxLen * 2) + 5;                // Length of rxLen + 5
    uint8_t txBuf[6];
    txBuf[0] = fullLen;                         // Message length. 5 extra bytes are destination, address, data check, alive counter, and LSSM
    txBuf[1] = dest;                            // Which device to send to
    txBuf[2] = reg_addr;                        // Which register to read from
    txBuf[3] = 0x00;                            // Data check byte
    txBuf[4] = calculate_PEC(txBuf + 1, 3);     // PEC for txBuf[1] -> txBuf[3]
    txBuf[5] = 0x00;                            // Alive byte

    // rprintf("Read request begin\n");
    // for (int i = 0; i < 6; i++) { rprintf("%d, %02x\n", i, txBuf[i]); }
    transmit_ADES_message_raw(txBuf, 6);

    WAIT_TIMEOUT((reg_read(M17_STATUS_RX) & RX_STOP), ADES_RX_TIMEOUT_MS, ERR_NO_RX);
    
    uint8_t buf[fullLen + 4];
    if (!read_rx_buf(buf, fullLen + 4)) return false;
    /*
    rprintf("Inside M17_read_ADES_reg\n");
    for (int i = 0; i < fullLen + 4; i++)
    {
        rprintf("%d, %02x\n", i, buf[i]);
    }
    */


    for (int i = 0; i < (rxLen * 2); i++) { ((uint8_t *)rxBuf)[i] = buf[i + 2]; }
    return true;
}

uint8_t M17_num_active_chips() {return num_active_chips;}

static bool transmit_ADES_message_raw(uint8_t *msg, uint8_t len)
{

    // Wait until there's a free load queue
    WAIT_TIMEOUT(!(reg_read(M17_STATUS_TX) & TX_FULL), M17_LDQ_TIMEOUT_MS, ERR_LDQ_FULL_TIMEOUT);

    // Load the queue
    uint8_t txBuf = M17_LDQ;
    core_SPI_start(M17_SPI);
    core_SPI_read_write(M17_SPI, &txBuf, 1, NULL, 0);
    core_SPI_read_write(M17_SPI, msg, len, NULL, 0);
    core_SPI_stop(M17_SPI);

    // Change LDQ_PTR to point back to first byte of queue we just wrote data to
    reg_write(M17_LDQ_PTR, 0x00);

    // Read back the queue
    uint8_t rxBuf[len];
    txBuf += 1;     // Read address for load queue
    core_SPI_start(M17_SPI);
    core_SPI_read_write(M17_SPI, &txBuf, 1, NULL, 0);
    core_SPI_read_write(M17_SPI, NULL, 0, rxBuf, len);
    core_SPI_stop(M17_SPI);

    for (int i = 0; i < len; i++) {
        if (rxBuf[i] != msg[i]) { FaultManager_set_err(ERR_LDQ_READ); return false; }
    }

    send_M17_command_1byte(M17_NXT_LDQ);
    return true;
}

static bool wake_daisy_chain()
{
    uint8_t rx_status;
    uint32_t wake_time = HAL_GetTick();
    if (!reg_write(M17_CONFIG_GEN2, TX_PREAMBLES | TX_QUEUE)) return false;          // CONFIG_GEN2 - Enable preamble transmit to wake up ADES chips
    while (HAL_GetTick() - wake_time < ADES_WAKE_TIMEOUT_MS)
    {
        rx_status = reg_read(M17_STATUS_RX); // RX status register
        if (rx_status == (RX_BUSY | RX_EMPTY))            // 0x21 = ( RX_BUSY(0x20) | RX_EMPTY(0x01) )
        {
            reg_write(M17_CONFIG_GEN2, TX_QUEUE);          // CONFIG_REG2 - Disable preamble transmit 
            // core_GPIO_digital_write(LED3_PORT, LED3_PIN, false);
            return true;
        }
    }
    rprintf("rx_status: %x\n", rx_status);
    FaultManager_set_fault(FAULT_M17_CONFIG);
    return false;
}

static bool command_hello_all()
{
    uint8_t msg[4] = {0x03, M17_HELLO_ALL, 0x00, 0x00};
    // core_GPIO_digital_write(LED3_PORT, LED3_PIN, true);

    if (!transmit_ADES_message_raw(msg, 4)) return false;

    // Read the message
    uint8_t rxBuf[4];
    read_rx_buf(rxBuf, 4);

    num_active_chips = rxBuf[2];
    
    // Set the number of active chips
    if (!reg_write(M17_CONFIG_GEN0, num_active_chips)) return false;

    return true;
}

static uint8_t calculate_PEC(uint8_t *msg, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t byte = 0; byte < len; byte++)
    {
        crc ^= msg[byte];
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x01) crc = ((crc >> 1) ^ CRC_POLY);
            else crc = (crc >> 1);
        }
    }
    return crc;
}

static bool check_rx_errors()
{
    // Check for UART RX errors
    uint8_t rxAlert = reg_read(M17_ALERT_RX);
    if (rxAlert & RX_ERR_ALRT) {
        FaultManager_set_err(ERR_ALERT_RX);
        return false;
    }
    
    if (rxAlert & RX_OVRFLW_ERR_ALRT) {
        FaultManager_set_err(ERR_ALERT_RX_OVERFLOW);
        return false;
    }

    return true;
}


/**************** M17 REGISTER FUNCTIONS ****************/
/********************************************************/
static void send_M17_command_1byte(uint8_t addr)
{
    core_SPI_start(M17_SPI);
    core_SPI_read_write(M17_SPI, &addr, 1, NULL, 0);
    core_SPI_stop(M17_SPI);
}

static void send_M17_command_2byte(uint8_t addr)
{
    uint8_t buf = 0;
    core_SPI_start(M17_SPI);
    core_SPI_read_write(M17_SPI, &addr, 1, NULL, 0);
    core_SPI_read_write(M17_SPI, &buf, 1, NULL, 0);
    core_SPI_stop(M17_SPI);
}

static bool reg_write(uint8_t addr, uint8_t msg)
{
    uint8_t num_retransmits = 0;
    uint8_t txBuf[2] = {addr, msg};
    uint8_t rxBuf;

    while (num_retransmits < M17_MAX_RET)
    {
        // Send initial config
        core_SPI_start(M17_SPI);
        core_SPI_read_write(M17_SPI, txBuf, 2, NULL, 0);
        core_SPI_stop(M17_SPI);

        // Check register
        if (reg_read(addr + 1) == msg) return true;
        else num_retransmits++;
    }
    FaultManager_set_fault(FAULT_M17_CONFIG);
    return false;
}

static uint8_t reg_read(uint8_t addr)
{
    uint8_t txBuf = addr;
    uint8_t rxBuf;
    core_SPI_start(M17_SPI);
    core_SPI_read_write(M17_SPI, &txBuf, 1, NULL, 0);
    core_SPI_read_write(M17_SPI, NULL, 0, &rxBuf, 1);
    core_SPI_stop(M17_SPI);
    return rxBuf;
}

static bool read_rx_buf(uint8_t *rxBuf, uint8_t len)
{
    // Wait for RX STOP
    WAIT_TIMEOUT((reg_read(M17_STATUS_RX) & RX_STOP), ADES_RX_TIMEOUT_MS, ERR_RX_TIMEOUT);


    uint8_t txBuf = M17_RX_RD_NXT_MSG;
    core_SPI_start(M17_SPI);
    core_SPI_read_write(M17_SPI, &txBuf, 1, NULL, 0);
    core_SPI_read_write(M17_SPI, NULL, 0, rxBuf, len);
    core_SPI_stop(M17_SPI);

    for (int i = 0; i < len; i++) {
        // rprintf("%d: %02x\n", i, rxBuf[i]);
    }

    // Read LSSM byte
    if (rxBuf[len - 1] != M17_NORMAL_LSSM) {
        FaultManager_LSSM(rxBuf[len - 2]);
        // core_GPIO_digital_write(LED2_PORT, LED2_PIN, true);
        // rprintf("LSSM: %02x\n", rxBuf[len-1]);
        // return false;
    }
    
    uint8_t rx_status = reg_read(M17_STATUS_RX);
    if (rx_status & (0x80) || rx_status & (0x08))
    {
        rprintf("rx_status: %02x\n", rx_status);
        FaultManager_set_err(ERR_RX_STATUS);
    }

    return true;
}

/********************************************************/
/********************************************************/
