#ifndef MAIN_UPDATE_H
#define MAIN_UPDATE_H

#include "flash_drv.h"
#include "mem_map.h"

void MainApp_Erase(void);
void Copy_Temp_To_Main(uint32_t fw_size);
void MainApp_SetValid(void);

#endif
