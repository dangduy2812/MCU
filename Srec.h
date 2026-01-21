/* --- START OF FILE Srec.h --- */
#ifndef SREC_H_
#define SREC_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SREC_OK = 0,
    SREC_ERR_NO_START_S,
    SREC_ERR_INVALID_TYPE,
    SREC_ERR_INVALID_HEX,
    SREC_ERR_LENGTH,
    SREC_ERR_CHECKSUM
} SrecStatus_t;

typedef struct {
    uint8_t  type;
    uint8_t  byte_count;
    uint32_t address;
    uint8_t  data[64];
    uint32_t data_len;
} SrecLine_t;

SrecStatus_t SREC_ParseLine(const char* line, SrecLine_t* record);
const char* SREC_GetErrorString(SrecStatus_t status);

#endif /* SREC_H_ */
