
#ifndef TIMER_LIB_H_
#define TIMER_LIB_H_

#include "S32K144.h"
#include "device_registers.h"
#include "sdk_project_config.h"
#include "Nvic.h"

/* ==========================================
 *  DEFINITIONS & MACROS
 * ========================================== */

/* --- LPIT Settings --- */
#define PCC_SRC_SPLL_DIV2 6				/* Nguồn Clock cho LPIT từ SPLL DIV2 */
#define LPIT_CLK_FREQ 40000000U			/* Giả định tần số đầu vào là 40MHz */
#define LPIT_TRIGGER_VALUE_1S 40000000U /* 40M ticks = 1 giây */

/* --- FTM Settings --- */
#define PCC_SRC_SOSC_DIV1 1		/* Nguồn Clock cho FTM từ SOSC (8MHz) */
#define FTM_PRESCALER_128 7		/* PS = 7 tương ứng chia 128 */
#define FTM_MOD_VALUE_1S 62499U /* (8MHz / 128) - 1 = 62499 */

/* ==========================================
 *  FUNCTION PROTOTYPES
 * ========================================== */

/* --- LPIT Functions --- */
/**
 *  Khởi tạo LPIT Channel 0
 * @param enable_irq: 1 để bật ngắt NVIC, 0 để chạy chế độ Polling
 */
void LPIT0_init(uint8_t enable_irq);

/**
 *  Kiểm tra cờ ngắt của LPIT Channel 0 (Dùng cho Polling)
 *  1 nếu đã hết giờ (timeout), 0 nếu chưa
 */
uint8_t LPIT0_Check_Timeout_Flag(void);

/**
 *  Xóa cờ ngắt LPIT Channel 0 (Dùng trong hàm IRQ)
 */
void LPIT0_Clear_Flag(void);

/* --- FTM Functions --- */
/**
 *  Khởi tạo FTM0 ở chế độ đếm thời gian (Timer)
 */
void FTM0_init_1s(void);

/**
 *  Bắt đầu cho phép FTM0 đếm
 */
void FTM0_Start_Counter(void);

/**
 *  Dừng FTM0
 */
void FTM0_Stop_Counter(void);

/**
 *  Kiểm tra cờ tràn (Overflow) của FTM0
 *  1 nếu đã tràn (đủ 1s), 0 nếu chưa
 */
uint8_t FTM0_Check_Overflow_Flag(void);

#endif /* TIMER_LIB_H_ */
