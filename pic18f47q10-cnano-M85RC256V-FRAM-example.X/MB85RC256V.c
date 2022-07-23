#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/i2c1_master.h"
#include "string.h"
#include "stdlib.h"
#include "MB85RC256V.h"

const i2c1_address_t MB85RC256V_I2C_ADDR = 0x50;

uint8_t seqReadBuffer[DATA_NUM_BYTES];
uint8_t seqWriteBuffer[DATA_NUM_BYTES + 2];

typedef struct
{
    size_t len;
    uint8_t *data;
}i2c1_buffer_t;

static i2c1_operations_t rd1RegCompleteHandler(void *ptr);
static i2c1_operations_t rdBlkRegCompleteHandler(void *ptr);
static i2c1_operations_t wr1RegCompleteHandler(void *ptr);
   
/* 
 * Sequential write to the FRAM 
 * 
 * startAddr: the memory location to start to write at.
 *            If len > 1, then the fram will be written with the data
 *            in the array to each subsequent memory location following the 
 *            startAddr. 
 * 
 *            The startAddr is 16 bits because the MB85RC256V has a range
 *            of (0-7FFF) memory R/W locations.
 * 
 * data:      the data array to be written to FRAM
 * len:       the size of the data array
 */
void FRAM_WriteNBytes(uint16_t startAddr, uint8_t* data, size_t len)
{
    memset(seqWriteBuffer, 0, sizeof(seqWriteBuffer));

    seqWriteBuffer[0] = (startAddr >> 8);
    seqWriteBuffer[1] = startAddr & 0xFF;
    
    memcpy(seqWriteBuffer + 2, data, len);

    while(!I2C1_Open(MB85RC256V_I2C_ADDR));
    I2C1_SetBuffer(seqWriteBuffer, len + 2);
    I2C1_SetAddressNackCallback(NULL, NULL); // NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}


/* 
 * Writes one byte to the FRAM 
 * 
 * readAddr:  the memory location to write to.
 *             
 */
void FRAM_WriteByte(uint16_t writeAddr, uint8_t data)
{
    uint8_t bfr[2];
    bfr[0] = (writeAddr >> 8);
    bfr[1] = writeAddr & 0xFF;
        
    while(!I2C1_Open(MB85RC256V_I2C_ADDR)); // sit here until we get the bus..
    I2C1_SetDataCompleteCallback(wr1RegCompleteHandler, &data);
    I2C1_SetBuffer(bfr, 2);
    I2C1_SetAddressNackCallback(NULL, NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}

/* 
 * Sequential read from the FRAM 
 * 
 * startAddr: the memory location to start to read from.
 *            If numBytes > 1, then the fram will be read from into the 
 *            receiveBuffer from each subsequent memory location following the 
 *            startAddr. 
 * 
 *            The startAddr is 16 bits because the MB85RC256V has a range
 *            of (0-7FFF) memory R/W locations.
 * 
 * receiveBuffer:  the array to hold the bytes read from the FRAM
 * numBytes:       the number of bytes to read from the FRAM
 */
void FRAM_ReadNBytes(uint16_t startAddr, uint8_t *receiveBuffer, size_t numBytes)
{
    i2c1_buffer_t bufferBlock; 
    bufferBlock.data = receiveBuffer;
    bufferBlock.len = numBytes;

    while(!I2C1_Open(MB85RC256V_I2C_ADDR)); 
    
    // device address | read startAddress High Byte | read startAddress Low Byte
    uint8_t msgHdr[2]; 
    msgHdr[0] = (startAddr >> 8);
    msgHdr[1] = startAddr & 0xFF;
    
    I2C1_SetDataCompleteCallback(rdBlkRegCompleteHandler, &bufferBlock);
    I2C1_SetBuffer(msgHdr, 2);
    I2C1_SetAddressNackCallback(NULL, NULL); //NACK polling?
    I2C1_MasterWrite();
    while(I2C1_BUSY == I2C1_Close()); // sit here until finished.
}

/* 
 * Reads one byte from the FRAM 
 * 
 * readAddr:  the memory location to start to read from.
 *             
 * returns the byte read from the FRAM at the given readAddr
 */
uint8_t FRAM_ReadByte(uint16_t readAddr)
{
    uint8_t returnValue = 0x00;
    
    while(!I2C1_Open(MB85RC256V_I2C_ADDR));

    uint8_t msgHdr[2];
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

static i2c1_operations_t wr1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr, 1);
    I2C1_SetDataCompleteCallback(NULL, NULL);
    return I2C1_CONTINUE;
}

static i2c1_operations_t rd1RegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(ptr,1);
    I2C1_SetDataCompleteCallback(NULL, NULL);
    return I2C1_RESTART_READ;
}

static i2c1_operations_t rdBlkRegCompleteHandler(void *ptr)
{
    I2C1_SetBuffer(((i2c1_buffer_t *)ptr)->data,((i2c1_buffer_t*)ptr)->len);
    I2C1_SetDataCompleteCallback(NULL, NULL);
    return I2C1_RESTART_READ;
}