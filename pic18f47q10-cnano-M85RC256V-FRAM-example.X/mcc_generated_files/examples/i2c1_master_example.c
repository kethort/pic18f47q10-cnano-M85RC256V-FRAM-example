/**
  I2C1 Generated Example Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c1.c

  @Summary
    This is the generated driver examples implementation file for the I2C1 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides implementations for driver APIs for I2C1.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.79.0
        Device            :  PIC18F47Q10
        Driver Version    :  1.0.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.10 and above or later
        MPLAB             :  MPLAB X 5.30
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "i2c1_master_example.h"
#include "string.h"
#include "stdlib.h"

const i2c1_address_t FRAM_ADDRESS = 0x50;

typedef struct
{
    size_t len;
    uint8_t *data;
}i2c1_buffer_t;

static i2c1_operations_t rd1RegCompleteHandler(void *ptr);
static i2c1_operations_t wr1RegCompleteHandler(void *ptr);
static i2c1_operations_t rdBlkRegCompleteHandler(void *ptr);

void I2C1_WriteNBytes(uint16_t startAddr, uint8_t* data, size_t len)
{
    // the start address can be 16-bits 
    uint8_t* bfr = (uint8_t*)malloc(len + 2);
    bfr[0] = (startAddr >> 8);
    bfr[1] = startAddr & 0xFF;
    
    memcpy(bfr + 2, data, len);

    I2C1_Open(FRAM_ADDRESS);
    I2C1_SetBuffer(bfr, len + 2);
    I2C1_SetAddressNackCallback(NULL, NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}

void I2C1_WriteByte(uint16_t writeAddr, uint8_t data)
{
    // the start address can be 16-bits 
    uint8_t* bfr = (uint8_t*)malloc(2);
    bfr[0] = (writeAddr >> 8);
    bfr[1] = writeAddr & 0xFF;
        
    I2C1_Open(FRAM_ADDRESS); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(wr1RegCompleteHandler, &data);
    I2C1_SetBuffer(bfr, 2);
    I2C1_SetAddressNackCallback(NULL, NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}

void I2C1_ReadNBytes(uint16_t startAddr, uint8_t *receiveBuffer, size_t numBytes)
{
    i2c1_buffer_t bufferBlock; 
    bufferBlock.data = receiveBuffer;
    bufferBlock.len = numBytes;

    I2C1_Open(FRAM_ADDRESS); 
    
    // the start address can be up to 16-bits so, the write command
    // should be: | device address | read startAddress High Byte | read startAddress Low Byte | 
    // sequential reads is just the device address with a length in the read buffer
    uint8_t* msgHdr = (uint8_t*)malloc(2);
    msgHdr[0] = (startAddr >> 8);
    msgHdr[1] = startAddr & 0xFF;
    
    I2C1_SetDataCompleteCallback(rdBlkRegCompleteHandler, &bufferBlock);
    I2C1_SetBuffer(msgHdr, 2);
    I2C1_SetAddressNackCallback(NULL, NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}

uint8_t I2C1_ReadByte(uint16_t readAddr)
{
    uint8_t returnValue = 0x00;
    
    I2C1_Open(FRAM_ADDRESS);

    // the start address can be 16-bits 
    uint8_t* msgHdr = (uint8_t*)malloc(2);
    msgHdr[0] = (readAddr >> 8);
    msgHdr[1] = readAddr & 0xFF;
     
    I2C1_SetDataCompleteCallback(rd1RegCompleteHandler, &returnValue);
    I2C1_SetBuffer(msgHdr, 2);
    I2C1_SetAddressNackCallback(NULL, NULL); //NACK polling?
    I2C1_MasterWrite();
    I2C1_MasterRead();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.

    return returnValue;
}

static i2c1_operations_t rd1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr,1);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    return I2C1_RESTART_READ;
}

static i2c1_operations_t wr1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr, 1);
    I2C1_SetDataCompleteCallback(NULL,NULL);
    return I2C1_CONTINUE;
}

static i2c1_operations_t rdBlkRegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(((i2c1_buffer_t *)ptr)->data,((i2c1_buffer_t*)ptr)->len);
    I2C1_SetDataCompleteCallback(NULL, NULL);
    return I2C1_RESTART_READ;
}