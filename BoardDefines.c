#include "BoardDefines.h"
#include "pins_driver.h"  /* Thư viện bạn vừa gửi */

void Board_Init(void) {
	PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    /* 1. Cấu hình Mux chân cho LED (PTD) */
    /* PORT_MUX_AS_GPIO có giá trị là 1U trong pins_driver.h */
    PINS_DRV_SetMuxModeSel(PORTD, LED_RED_PIN,   PORT_MUX_AS_GPIO);
    PINS_DRV_SetMuxModeSel(PORTD, LED_GREEN_PIN, PORT_MUX_AS_GPIO);
    PINS_DRV_SetMuxModeSel(PORTD, LED_BLUE_PIN,  PORT_MUX_AS_GPIO);

    /* 2. Cấu hình Mux chân cho Button (PTC13) */
    PINS_DRV_SetMuxModeSel(PORTC, BTN_PIN, PORT_MUX_AS_GPIO);

    /* 3. Cấu hình Mux chân cho UART 1 (LPUART 1) */
    /* Giả định UART1 dùng chân PTC6 (RX) và PTC7 (TX) trên S32K144 */
    PINS_DRV_SetMuxModeSel(PORTC, 6U, PORT_MUX_ALT2); /* LPUART1_RX */
    PINS_DRV_SetMuxModeSel(PORTC, 7U, PORT_MUX_ALT2); /* LPUART1_TX */

    /* 4. Đặt hướng chân (Direction) */
    /* LED là Output (1U), Button là Input (0U) */
    PINS_DRV_SetPinDirection(PTD, LED_RED_PIN,   1U);
    PINS_DRV_SetPinDirection(PTD, LED_GREEN_PIN, 1U);
    PINS_DRV_SetPinDirection(PTD, LED_BLUE_PIN,  1U);
    PINS_DRV_SetPinDirection(PTC, BTN_PIN,       0U);

    /* Tắt tất cả LED ban đầu (Set mức cao vì Active Low) */
    PINS_DRV_SetPins(PTD, (1 << LED_RED_PIN) | (1 << LED_GREEN_PIN) | (1 << LED_BLUE_PIN));
}
