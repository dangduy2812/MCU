/* --- START OF FILE CRC32.h --- */
#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>
#include <stddef.h>

/* CRC-32/IEEE (polynomial 0xEDB88320, init 0xFFFFFFFF, xorout 0xFFFFFFFF) */
uint32_t CRC_Calc(const uint8_t* data, size_t len);

#endif /* CRC_H_ */
