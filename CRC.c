/* --- START OF FILE CRC32.c --- */
#include "CRC.h"

uint32_t CRC_Calc(const uint8_t* data, size_t len) {
    uint32_t crc = 0xFFFFFFFFU;

    for (size_t i = 0; i < len; i++) {
        uint32_t byte = data[i];
        crc ^= byte;
        for (uint32_t b = 0; b < 8; b++) {
            uint32_t mask = (uint32_t)-(int32_t)(crc & 1U);
            crc = (crc >> 1) ^ (0xEDB88320U & mask);
        }
    }

    return ~crc;
}
