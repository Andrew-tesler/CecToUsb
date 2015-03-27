/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// eusci_i2c.h - Driver for the EUSCI_I2C Module.
//
//*****************************************************************************

#ifndef __MSP430WARE_EUSCI_I2C_H__
#define __MSP430WARE_EUSCI_I2C_H__

#include "inc/hw_memmap.h"

#ifdef __MSP430_HAS_EUSCI_Bx__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//******************************************************************************
//
// The following is a struct that is passed to EUSCI_I2C_initMaster()
//
//******************************************************************************
typedef struct EUSCI_I2C_initMasterParam {
        uint8_t selectClockSource;
        uint32_t i2cClk;
        uint32_t dataRate;
        uint8_t byteCounterThreshold;
        uint8_t autoSTOPGeneration;
} EUSCI_I2C_initMasterParam;

//******************************************************************************
//
// The following is a struct that is passed to EUSCI_I2C_initSlave()
//
//******************************************************************************
typedef struct EUSCI_I2C_initSlaveParam {
        uint8_t slaveAddress;
        uint8_t slaveAddressOffset;
        uint32_t slaveOwnAddressEnable;
} EUSCI_I2C_initSlaveParam;

//*****************************************************************************
//
// The following are values that can be passed to the autoSTOPGeneration
// parameter for functions: EUSCI_I2C_masterInit().
//
//*****************************************************************************
#define EUSCI_I2C_NO_AUTO_STOP                                         UCASTP_0
#define EUSCI_I2C_SET_BYTECOUNT_THRESHOLD_FLAG                         UCASTP_1
#define EUSCI_I2C_SEND_STOP_AUTOMATICALLY_ON_BYTECOUNT_THRESHOLD       UCASTP_2

//*****************************************************************************
//
// The following are values that can be passed to the dataRate parameter for
// functions: EUSCI_I2C_masterInit().
//
//*****************************************************************************
#define EUSCI_I2C_SET_DATA_RATE_400KBPS                                  400000
#define EUSCI_I2C_SET_DATA_RATE_100KBPS                                  100000

//*****************************************************************************
//
// The following are values that can be passed to the selectClockSource
// parameter for functions: EUSCI_I2C_masterInit().
//
//*****************************************************************************
#define EUSCI_I2C_CLOCKSOURCE_ACLK                                 UCSSEL__ACLK
#define EUSCI_I2C_CLOCKSOURCE_SMCLK                               UCSSEL__SMCLK

//*****************************************************************************
//
// The following are values that can be passed to the slaveAddressOffset
// parameter for functions: EUSCI_I2C_slaveInit().
//
//*****************************************************************************
#define EUSCI_I2C_OWN_ADDRESS_OFFSET0                                      0x00
#define EUSCI_I2C_OWN_ADDRESS_OFFSET1                                      0x02
#define EUSCI_I2C_OWN_ADDRESS_OFFSET2                                      0x04
#define EUSCI_I2C_OWN_ADDRESS_OFFSET3                                      0x06

//*****************************************************************************
//
// The following are values that can be passed to the slaveOwnAddressEnable
// parameter for functions: EUSCI_I2C_slaveInit().
//
//*****************************************************************************
#define EUSCI_I2C_OWN_ADDRESS_DISABLE                                      0x00
#define EUSCI_I2C_OWN_ADDRESS_ENABLE                                     UCOAEN

//*****************************************************************************
//
// The following are values that can be passed to the mode parameter for
// functions: EUSCI_I2C_setMode() as well as returned by the
// EUSCI_I2C_getMode() function.
//
//*****************************************************************************
#define EUSCI_I2C_TRANSMIT_MODE                                            UCTR
#define EUSCI_I2C_RECEIVE_MODE                                             0x00

//*****************************************************************************
//
// The following are values that can be passed to the mask parameter for
// functions: EUSCI_I2C_enableInterrupt(), EUSCI_I2C_disableInterrupt(),
// EUSCI_I2C_clearInterruptFlag(), and EUSCI_I2C_getInterruptStatus() as well
// as returned by the EUSCI_I2C_getInterruptStatus() function.
//
//*****************************************************************************
#define EUSCI_I2C_NAK_INTERRUPT                                        UCNACKIE
#define EUSCI_I2C_ARBITRATIONLOST_INTERRUPT                              UCALIE
#define EUSCI_I2C_STOP_INTERRUPT                                        UCSTPIE
#define EUSCI_I2C_START_INTERRUPT                                       UCSTTIE
#define EUSCI_I2C_TRANSMIT_INTERRUPT0                                   UCTXIE0
#define EUSCI_I2C_TRANSMIT_INTERRUPT1                                   UCTXIE1
#define EUSCI_I2C_TRANSMIT_INTERRUPT2                                   UCTXIE2
#define EUSCI_I2C_TRANSMIT_INTERRUPT3                                   UCTXIE3
#define EUSCI_I2C_RECEIVE_INTERRUPT0                                    UCRXIE0
#define EUSCI_I2C_RECEIVE_INTERRUPT1                                    UCRXIE1
#define EUSCI_I2C_RECEIVE_INTERRUPT2                                    UCRXIE2
#define EUSCI_I2C_RECEIVE_INTERRUPT3                                    UCRXIE3
#define EUSCI_I2C_BIT9_POSITION_INTERRUPT                              UCBIT9IE
#define EUSCI_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT                          UCCLTOIE
#define EUSCI_I2C_BYTE_COUNTER_INTERRUPT                               UCBCNTIE

//*****************************************************************************
//
// The following are values that can be passed toThe following are values that
// can be returned by the EUSCI_I2C_isBusBusy() function.
//
//*****************************************************************************
#define EUSCI_I2C_BUS_BUSY                                              UCBBUSY
#define EUSCI_I2C_BUS_NOT_BUSY                                             0x00

//*****************************************************************************
//
// The following are values that can be passed toThe following are values that
// can be returned by the EUSCI_I2C_masterIsStopSent() function.
//
//*****************************************************************************
#define EUSCI_I2C_STOP_SEND_COMPLETE                                       0x00
#define EUSCI_I2C_SENDING_STOP                                          UCTXSTP

//*****************************************************************************
//
// The following are values that can be passed toThe following are values that
// can be returned by the EUSCI_I2C_masterIsStartSent() function.
//
//*****************************************************************************
#define EUSCI_I2C_START_SEND_COMPLETE                                      0x00
#define EUSCI_I2C_SENDING_START                                         UCTXSTT

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void EUSCI_I2C_initMaster(uint16_t baseAddress,
                                 EUSCI_I2C_initMasterParam *param);

extern void EUSCI_I2C_initSlave(uint16_t baseAddress,
                                EUSCI_I2C_initSlaveParam *param);

extern void EUSCI_I2C_enable(uint16_t baseAddress);

extern void EUSCI_I2C_disable(uint16_t baseAddress);

extern void EUSCI_I2C_setSlaveAddress(uint16_t baseAddress,
                                      uint8_t slaveAddress);

extern void EUSCI_I2C_setMode(uint16_t baseAddress,
                              uint8_t mode);

extern uint8_t EUSCI_I2C_getMode(uint16_t baseAddress);

extern void EUSCI_I2C_slaveDataPut(uint16_t baseAddress,
                                   uint8_t transmitData);

extern uint8_t EUSCI_I2C_slaveDataGet(uint16_t baseAddress);

extern uint16_t EUSCI_I2C_isBusBusy(uint16_t baseAddress);

extern uint16_t EUSCI_I2C_masterIsStopSent(uint16_t baseAddress);

extern uint16_t EUSCI_I2C_masterIsStartSent(uint16_t baseAddress);

extern void EUSCI_I2C_enableInterrupt(uint16_t baseAddress,
                                      uint16_t mask);

extern void EUSCI_I2C_disableInterrupt(uint16_t baseAddress,
                                       uint16_t mask);

extern void EUSCI_I2C_clearInterruptFlag(uint16_t baseAddress,
                                         uint16_t mask);

extern uint16_t EUSCI_I2C_getInterruptStatus(uint16_t baseAddress,
                                             uint16_t mask);

extern void EUSCI_I2C_masterSendSingleByte(uint16_t baseAddress,
                                           uint8_t txData);

extern uint8_t EUSCI_I2C_masterReceiveSingleByte(uint16_t baseAddress);

extern bool EUSCI_I2C_masterSendSingleByteWithTimeout(uint16_t baseAddress,
                                                      uint8_t txData,
                                                      uint32_t timeout);

extern void EUSCI_I2C_masterMultiByteSendStart(uint16_t baseAddress,
                                               uint8_t txData);

extern bool EUSCI_I2C_masterMultiByteSendStartWithTimeout(uint16_t baseAddress,
                                                          uint8_t txData,
                                                          uint32_t timeout);

extern void EUSCI_I2C_masterMultiByteSendNext(uint16_t baseAddress,
                                              uint8_t txData);

extern bool EUSCI_I2C_masterMultiByteSendNextWithTimeout(uint16_t baseAddress,
                                                         uint8_t txData,
                                                         uint32_t timeout);

extern void EUSCI_I2C_masterMultiByteSendFinish(uint16_t baseAddress,
                                                uint8_t txData);

extern bool EUSCI_I2C_masterMultiByteSendFinishWithTimeout(uint16_t baseAddress,
                                                           uint8_t txData,
                                                           uint32_t timeout);

extern void EUSCI_I2C_masterSendStart(uint16_t baseAddress);

extern void EUSCI_I2C_masterMultiByteSendStop(uint16_t baseAddress);

extern bool EUSCI_I2C_masterMultiByteSendStopWithTimeout(uint16_t baseAddress,
                                                         uint32_t timeout);

extern void EUSCI_I2C_masterReceiveStart(uint16_t baseAddress);

extern uint8_t EUSCI_I2C_masterMultiByteReceiveNext(uint16_t baseAddress);

extern uint8_t EUSCI_I2C_masterMultiByteReceiveFinish(uint16_t baseAddress);

extern bool EUSCI_I2C_masterMultiByteReceiveFinishWithTimeout(uint16_t baseAddress,
                                                              uint8_t *txData,
                                                              uint32_t timeout);

extern void EUSCI_I2C_masterMultiByteReceiveStop(uint16_t baseAddress);

extern void EUSCI_I2C_enableMultiMasterMode(uint16_t baseAddress);

extern void EUSCI_I2C_disableMultiMasterMode(uint16_t baseAddress);

extern uint8_t EUSCI_I2C_masterSingleReceive(uint16_t baseAddress);

extern uint32_t EUSCI_I2C_getReceiveBufferAddress(uint16_t baseAddress);

extern uint32_t EUSCI_I2C_getTransmitBufferAddress(uint16_t baseAddress);

//*****************************************************************************
//
// The following are deprecated APIs.
//
//*****************************************************************************
#define EUSCI_I2C_getTransmitBufferAddressForDMA                              \
        EUSCI_I2C_getTransmitBufferAddress

//*****************************************************************************
//
// The following are deprecated APIs.
//
//*****************************************************************************
#define EUSCI_I2C_getReceiveBufferAddressForDMA                               \
        EUSCI_I2C_getReceiveBufferAddress

//*****************************************************************************
//
// The following are deprecated APIs.
//
//*****************************************************************************
#define EUSCI_I2C_masterIsSTOPSent                   EUSCI_I2C_masterIsStopSent

//*****************************************************************************
//
// The following are deprecated APIs.
//
//*****************************************************************************
extern void EUSCI_I2C_masterInit(uint16_t baseAddress,
                                 uint8_t selectClockSource,
                                 uint32_t i2cClk,
                                 uint32_t dataRate,
                                 uint8_t byteCounterThreshold,
                                 uint8_t autoSTOPGeneration);

extern void EUSCI_I2C_slaveInit(uint16_t baseAddress,
                                uint8_t slaveAddress,
                                uint8_t slaveAddressOffset,
                                uint32_t slaveOwnAddressEnable);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
#endif // __MSP430WARE_EUSCI_I2C_H__
