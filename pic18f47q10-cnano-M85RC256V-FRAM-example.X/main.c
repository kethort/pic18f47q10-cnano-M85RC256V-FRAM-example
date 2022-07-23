#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/drivers/uart.h"
#include "MB85RC256V.h"

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    printf("Hello world!\n");
            
    uint8_t framDataArray[DATA_NUM_BYTES];
    
    // put random data into an array
    for(uint16_t i = 0; i < DATA_NUM_BYTES; i++) {
        if((i % 2) == 0) {
            framDataArray[i] = 0xAA;
        } else {
            framDataArray[i] = 0xBB;
        }
    }
    
    FRAM_WriteNBytes(0x00, &framDataArray[0], DATA_NUM_BYTES); 
    
    memset(&framDataArray, 0, sizeof(framDataArray));

    FRAM_ReadNBytes(0x00, &framDataArray[0], DATA_NUM_BYTES);

    for (uint16_t i = 0; i < DATA_NUM_BYTES; i++) {
        printf("framDataArray[%d]: ", framDataArray[i]);
    }
    
    while(1);
}