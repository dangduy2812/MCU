#ifndef FLASH_DRV_H
#define FLASH_DRV_H

#include <stdint.h>
#include <stdbool.h>

bool Flash_EraseSector(uint32_t address);
bool Flash_ProgramPhrase(uint32_t address, uint64_t data);
void Flash_Read(uint32_t address, uint8_t *buf, uint32_t len);

#endif
