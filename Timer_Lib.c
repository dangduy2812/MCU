#include "Timer_Lib.h"

/* ==========================================
 *  LPIT IMPLEMENTATION
 * ========================================== */

void LPIT0_init(uint8_t enable_irq)
{
	/* 1. Cấp Clock cho LPIT */
	PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(PCC_SRC_SPLL_DIV2);
	PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

	for (volatile int i = 0; i < 100; i++)
		;

	/* 2. Kích hoạt Module */
	LPIT0->MCR = LPIT_MCR_M_CEN_MASK;

	/* 3. Thiết lập chu kỳ */
	LPIT0->TMR[0].TVAL = LPIT_TRIGGER_VALUE_1S;

	/* 4. Cấu hình Control & Interrupt */
	if (enable_irq)
	{
		/* A. Bật ngắt: Ghi vào thanh ghi MIER (Module Interrupt Enable Register) */
		/* TIE0 nằm ở bit 0 của thanh ghi MIER */
		LPIT0->MIER |= LPIT_MIER_TIE0_MASK;

		/* B. Bật Timer: Ghi vào TCTRL (Chỉ bật Timer, ko bật ngắt ở đây) */
		LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_T_EN_MASK;
		;

		/* C. Cấu hình NVIC */
		NVIC_SetPriority(LPIT0_Ch0_IRQn, 1);
		NVIC_EnableInterrupt(LPIT0_Ch0_IRQn);
	}
	else
	{
		/* Nếu không dùng ngắt, TẮT bit trong MIER */
		LPIT0->MIER &= ~LPIT_MIER_TIE0_MASK;

		/* Chỉ bật Timer */
		LPIT0->TMR[0].TCTRL = LPIT_TMR_TCTRL_T_EN_MASK;
	}
}

uint8_t LPIT0_Check_Timeout_Flag(void)
{
	/* Kiểm tra cờ ngắt Channel 0 Timeout Flag (TIF0) */
	if (LPIT0->MSR & LPIT_MSR_TIF0_MASK)
	{
		LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Xóa cờ bằng cách ghi 1 (W1C) */
		return 1;
	}
	return 0;
}

void LPIT0_Clear_Flag(void)
{
	LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Xóa cờ TIF0 */
}

/* ==========================================
 *  FTM IMPLEMENTATION
 * ========================================== */

void FTM0_init_1s(void)
{
	/* 1. Cấp clock cho FTM0 dùng SOSCDIV1 (8MHz) */
	/* Lưu ý: Phải clear bit CGC trước khi đổi nguồn clock */
	PCC->PCCn[PCC_FTM0_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_FTM0_INDEX] = PCC_PCCn_PCS(PCC_SRC_SOSC_DIV1) | PCC_PCCn_CGC_MASK;

	/* 2. Vô hiệu hóa bảo vệ ghi để sửa được thanh ghi MOD, CNT... */
	FTM0->MODE |= FTM_MODE_WPDIS_MASK;

	/* 3. Tắt counter trước khi cấu hình (CLKS = 00) */
	FTM0->SC = 0;

	/* 4. Cấu hình Prescaler = 128 */
	FTM0->SC |= FTM_SC_PS(FTM_PRESCALER_128);

	/* 5. Đặt MOD (Modulo) để định thời 1 giây */
	FTM0->MOD = FTM_MOD_VALUE_1S;

	/* 6. Reset bộ đếm về 0 */
	FTM0->CNT = 0;
}

void FTM0_Start_Counter(void)
{
	/*
	 * Chọn nguồn Clock: System Clock (01)
	 * Lưu ý: "System Clock" ở đây nghĩa là dùng nguồn clock được cấp bởi PCC
	 * (tức là SOSCDIV1 8MHz đã cấu hình ở bước init).
	 * CLKS(3) là External Clock (xung từ chân Pin bên ngoài).
	 */
	FTM0->SC |= FTM_SC_CLKS(1);
}

void FTM0_Stop_Counter(void)
{
	/* CLKS = 00: No Clock Selected (Dừng đếm) */
	FTM0->SC &= ~FTM_SC_CLKS_MASK;
}

uint8_t FTM0_Check_Overflow_Flag(void)
{
	/* Kiểm tra cờ tràn Timer Overflow Flag (TOF) */
	if (FTM0->SC & FTM_SC_TOF_MASK)
	{
		FTM0->SC &= ~FTM_SC_TOF_MASK; /* Xóa cờ TOF bằng cách ghi 0 (Logic của FTM hơi khác LPIT) */
		/* Lưu ý: Với một số biến thể FTM, cần đọc SC rồi ghi 0 vào TOF, dòng trên đã thỏa mãn */
		return 1;
	}
	return 0;
}
