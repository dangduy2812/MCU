#include "LPUART.h"

static LineQueue_t rx_queue;
static volatile uint8_t rx_index = 0;
static volatile bool rx_overflow_flag = false;
static char isr_line_buffer[MAX_BUFFER_SIZE];

/* ==========================================================
 * LPUART0: 115200 Baud | Clock: 48MHz (FIRC) | Pins: PTB0, PTB1
 * ========================================================== */
void LPUART0_init(void) {
    PCC->PCCn[PCC_LPUART0_INDEX] &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_PCS(3); /* FIRCDIV2 (48MHz) */
    PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_CGC_MASK;

    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;
    PORTB->PCR[0] = PORT_PCR_MUX(2);
    PORTB->PCR[1] = PORT_PCR_MUX(2);

    LPUART0->BAUD = LPUART_BAUD_OSR(15) | LPUART_BAUD_SBR(26);
    LPUART0->CTRL = LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;
}

void LPUART0_transmit_char(char c) {
    while(!(LPUART0->STAT & LPUART_STAT_TDRE_MASK));
    LPUART0->DATA = c;
}

void LPUART0_transmit_string(char* msg) {
    while(*msg) {
        LPUART0_transmit_char(*msg++);
    }
}

/* ==========================================================
 * LPUART1: 9600 Baud | Clock: 8MHz (SOSC) | Pins: PTC6, PTC7
 * Lưu ý: PTC6/7 nối trực tiếp chip OpenSDA (USB Micro B)
 * ========================================================== */
void LPUART1_init(void) {
    PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(3);
    PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    PORTC->PCR[6] = PORT_PCR_MUX(2);
    PORTC->PCR[7] = PORT_PCR_MUX(2);

    LPUART1->BAUD = LPUART_BAUD_OSR(15) | LPUART_BAUD_SBR(26);
    LPUART1->CTRL = LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;
}

void LPUART1_transmit_char(char c) {
    while(!(LPUART1->STAT & LPUART_STAT_TDRE_MASK));
    LPUART1->DATA = c;
}

void LPUART1_transmit_string(const char* msg) {
    while(*msg) {
        LPUART1_transmit_char(*msg++);
    }
}

void LPUART1_init_interrupt(void) {
    LPUART1_init();
    LPUART1->CTRL |= LPUART_CTRL_RIE_MASK;
    NVIC_SetPriority(LPUART1_RxTx_IRQn, 1);
    NVIC_EnableInterrupt(LPUART1_RxTx_IRQn);

    Queue_Init(&rx_queue);
    rx_index = 0;
    rx_overflow_flag = false;
}

void LPUART1_RxTx_IRQHandler(void)
{
    if (LPUART1->STAT & LPUART_STAT_RDRF_MASK)
    {
        char rx_char = (char)LPUART1->DATA;

        if (rx_char == '\n') {
            if (rx_index > 0) {
                isr_line_buffer[rx_index] = '\0';
                if (!Queue_Push(&rx_queue, isr_line_buffer, rx_index)) {
                    rx_overflow_flag = true;
                }
                rx_index = 0;
            }
        } else if (rx_char != '\r') {
            if (rx_index < MAX_BUFFER_SIZE - 1) {
                isr_line_buffer[rx_index++] = rx_char;
            } else {
                rx_overflow_flag = true;
            }
        }
    }

    if (LPUART1->STAT & LPUART_STAT_OR_MASK)
    {
        LPUART1->STAT |= LPUART_STAT_OR_MASK;
    }
}

bool LPUART1_ReadLine(char* out, size_t* out_len) {
    return Queue_Pop(&rx_queue, out, out_len);
}

bool LPUART1_HasOverflow(void) {
    return rx_overflow_flag;
}

void LPUART1_ClearOverflow(void) {
    rx_overflow_flag = false;
}
