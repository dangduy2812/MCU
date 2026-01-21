#ifndef TEMP_APP_H
#define TEMP_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "flash_drv.h"
#include "mem_map.h"

void TempApp_Prepare(void);
void TempApp_Write(uint32_t srec_addr, uint8_t *data, uint32_t len);
void TempApp_SetComplete(void);
bool TempApp_IsComplete(void);

#endif
