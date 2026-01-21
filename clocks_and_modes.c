#include "clocks_and_modes.h"

void SOSC_init_8MHz(void) {
    /* Cấu hình chia clock SOSC: DIV1 và DIV2 không chia (chia 1) */
    SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1);

    /* Cấu hình thạch anh: Dải trung bình (1MHz-8MHz), External Crystal */
    SCG->SOSCCFG = SCG_SOSCCFG_RANGE(2) | SCG_SOSCCFG_EREFS(1);

    /* Đợi cho đến khi thanh ghi SOSCCSR được mở khóa */
    while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK);

    /* Bật SOSC */
    SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN(1);

    /* Đợi SOSC hoạt động ổn định (Valid) */
    while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK));
}

void FIRC_init_48MHz(void) {
    /* Enable FIRC and set FIRCDIV2 to divide-by-1 so PCC can use 48MHz */
    while (SCG->FIRCCSR & SCG_FIRCCSR_LK_MASK);
    SCG->FIRCCSR = SCG_FIRCCSR_FIRCEN(1);
    SCG->FIRCDIV = SCG_FIRCDIV_FIRCDIV2(1);
    while (!(SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK));
}

void SPLL_init_160MHz(void) {
    /* Đảm bảo SPLL đang tắt để cấu hình */
    while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);
    SCG->SPLLCSR = 0x00000000;

    /* Chia nguồn đầu vào PLL: DIV1 chia 2, DIV2 chia 4 */
    SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(2) | SCG_SPLLDIV_SPLLDIV2(3);

    /* Cấu hình PLL: Nguồn là SOSC, Nhân 40 (MULT=24 tương ứng nhân 40) */
    /* Công thức: 8MHz / 1 (PREDIV) * 40 / 2 = 160 MHz */
    SCG->SPLLCFG = SCG_SPLLCFG_MULT(24);

    /* Mở khóa và bật PLL */
    while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK);
    SCG->SPLLCSR = SCG_SPLLCSR_SPLLEN(1);

    /* Đợi PLL hoạt động ổn định (Lock/Valid) */
    while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK));
}

void NormalRUNmode_80MHz(void) {
    /* Thiết lập nguồn System Clock là SPLL (6) */
    /* Core = 160MHz / 2 = 80MHz */
    /* Bus  = 80MHz / 2  = 40MHz */
    /* Flash = 80MHz / 3 = 26.67MHz */
    SCG->RCCR = SCG_RCCR_SCS(6)
                 | SCG_RCCR_DIVCORE(1)
                 | SCG_RCCR_DIVBUS(1)
                 | SCG_RCCR_DIVSLOW(2);

    /* Đợi hệ thống chuyển đổi nguồn thành công sang SPLL (SCS=6) */
    while (((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6);
}
