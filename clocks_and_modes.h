#ifndef CLOCKS_AND_MODES_H_
#define CLOCKS_AND_MODES_H_

#include "S32K144.h"
#include "device_registers.h"
#include "sdk_project_config.h"

/* Khởi tạo thạch anh ngoài 8MHz */
void SOSC_init_8MHz(void);

void FIRC_init_48MHz(void);

/* Khởi tạo bộ nhân tần PLL lên 160MHz */
void SPLL_init_160MHz(void);

/* Cấu hình chế độ RUN: Core 80MHz, Bus 40MHz, Flash 26.67MHz */
void NormalRUNmode_80MHz(void);

#endif /* CLOCKS_AND_MODES_H_ */
