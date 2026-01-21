#ifndef BOARD_DEFINES_H
#define BOARD_DEFINES_H

#include "device_registers.h"
#include "pins_driver.h"

/* ================== LED CONFIGURATION ================== */
/* S32K144 EVB sử dụng PTD15, PTD16, PTD0 cho RGB LED */
#define LED_GPIO            PTD
#define LED_RED_PIN         15U
#define LED_GREEN_PIN       16U
#define LED_BLUE_PIN        0U

/* ================= BUTTON CONFIGURATION ================= */
/* SW2 trên Kit S32K144 EVB là PTC12 */
#define BTN_GPIO            PTC
#define BTN_PORT            PORTC
#define BTN_PIN             12U

/* ================== UART CONFIGURATION ================== */
/* UART 1 nối với OpenSDA (USB) trên PTC6 (RX) và PTC7 (TX) */
#define UART_INSTANCE       1U
#define UART_PORT           PORTC
#define UART_RX_PIN         6U
#define UART_TX_PIN         7U

/* ================= HARDWARE MACROS ===================== */
/* Macros để điều khiển LED (Active Low trên S32K144 EVB) */
#define LED_RED_ON()        PINS_DRV_ClearPins(LED_GPIO, (1 << LED_RED_PIN))
#define LED_RED_OFF()       PINS_DRV_SetPins(LED_GPIO, (1 << LED_RED_PIN))
#define LED_GREEN_ON()      PINS_DRV_ClearPins(LED_GPIO, (1 << LED_GREEN_PIN))
#define LED_GREEN_OFF()     PINS_DRV_SetPins(LED_GPIO, (1 << LED_GREEN_PIN))
#define LED_BLUE_ON()       PINS_DRV_ClearPins(LED_GPIO, (1 << LED_BLUE_PIN))
#define LED_BLUE_OFF()      PINS_DRV_SetPins(LED_GPIO, (1 << LED_BLUE_PIN))

/* Macro đọc trạng thái nút nhấn (Nút nhấn SW2 thường là Active Low) */
#define IS_BTN_BOOT_PRESSED()  ((PINS_DRV_ReadPins(BTN_GPIO) & (1 << BTN_PIN)) == 0)

/* ================= FUNCTION PROTOTYPES ================= */
void Board_Init(void);

#endif /* BOARD_DEFINES_H */
