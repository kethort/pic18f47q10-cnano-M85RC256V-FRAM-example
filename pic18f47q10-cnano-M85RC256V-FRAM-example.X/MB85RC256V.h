
#ifndef __FRAM_H__
#define	__FRAM_H__

#define DATA_NUM_BYTES             256
#define DATA_CHUNK_DIV               4  // use a divider to chunkize sequential reads and writes 

void FRAM_WriteNBytes(uint16_t startAddr, uint8_t* data, size_t len);
void FRAM_WriteByte(uint16_t writeAddr, uint8_t data);
void FRAM_ReadNBytes(uint16_t startAddr, uint8_t *receiveBuffer, size_t numBytes);
uint8_t FRAM_ReadByte(uint16_t readAddr);

#endif	/* __FRAM_H__ */

