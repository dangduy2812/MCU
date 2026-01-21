#include "mem_map.h"
#include "sdk_project_config.h"
#include "interrupt_manager.h"   /* Chứa INT_SYS_DisableIRQGlobal */
#include "device_registers.h"    /* Chứa định nghĩa các thanh ghi core nhân ARM */
#include "pins_driver.h"
#include "BoardDefines.h"
#include "clocks_and_modes.h"
#include "LPUART.h"
#include "temp_app.h"
#include "main_update.h"
#include "Queue.h"
#include "Srec.h"
#include "SID_handle.h"

void JumpToUserApp(uint32_t address) {
    /* 1. Lấy Stack Pointer và Reset Handler trực tiếp từ địa chỉ 0x8000 */
    uint32_t stackPointer = *((volatile uint32_t*)address);
    uint32_t jumpAddress = *((volatile uint32_t*)(address + 4));

    if ((stackPointer & 0x2FFE0000U) != 0x20000000U)
	{
		return;
	}

    /* 2. Vô hiệu hóa ngắt */
    INT_SYS_DisableIRQGlobal();

    /* 3. Thiết lập lại Vector Table Offset Register (VTOR) */
    S32_SCB->VTOR = address;

    /* 4. Thiết lập Stack Pointer bằng lệnh Assembly (Đảm bảo Build được) */
    __asm volatile ("MSR msp, %0" : : "r" (stackPointer) : "sp");

    /* 5. Nhảy sang User App */
    void (*app_reset_handler)(void) = (void (*)(void))jumpAddress;
    app_reset_handler();
}

void System_Reset(void) {
	S32_SCB->AIRCR = S32_SCB_AIRCR_VECTKEY(0x05FA) | S32_SCB_AIRCR_SYSRESETREQ_MASK;
}

void Hardware_Init(void) {
    /* Khởi tạo Clock (Hàm trong clocks_and_modes.h của bạn) */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    FIRC_init_48MHz();
    NormalRUNmode_80MHz();

    /* Khởi tạo Pins và UART */
    Board_Init();
    LPUART1_init();
}

static void WDOG_disable(void) {
    WDOG->CNT = 0xD928C520;
    WDOG->TOVAL = 0x0000FFFF;
    WDOG->CS = 0x00002120;
}

void Bootloader_Handle(LineQueue_t *q) {
    char srec_line[QUEUE_LINE_MAX];
    size_t srec_len = 0;
    SrecLine_t record;
    SrecStatus_t status;

    if (!Queue_Pop(q, srec_line, &srec_len)) return;

    /* 1. XỬ LÝ LỆNH KẾT THÚC (END) - THỰC HIỆN DI TRÚ */
    if (strcmp(srec_line, "END") == 0) {
        /* Vùng găng: Tắt ngắt để thực hiện Migration quan trọng */
        INT_SYS_DisableIRQGlobal();

        TempApp_SetComplete();

        if (TempApp_IsComplete()) {

        	/* Bật đèn xanh dương để debug */
			PINS_DRV_SetPins(PTD, (1 << 0));
			PINS_DRV_SetPins(PTD, (1 << 15));
			PINS_DRV_SetPins(PTD, (1 << 16));
			PINS_DRV_TogglePins(PTD, (1 << 0));

            MainApp_Erase();
            Copy_Temp_To_Main(224 * 1024); /* Chép toàn bộ dải App */
            MainApp_SetValid();

            /* Gửi duy nhất 1 mã ACK báo cho PC Tool biết đã xong */
            LPUART1_transmit_char(SID_ACK);

            /* Reset hệ thống */
            System_Reset();
        } else {
            LPUART1_transmit_char(SID_NACK);
            INT_SYS_EnableIRQGlobal();
        }
        return;
    }

    /* 2. XỬ LÝ DÒNG DỮ LIỆU SREC */
    status = SREC_ParseLine(srec_line, &record);

    if (status == SREC_OK && record.type >= 1 && record.type <= 3) {
        /* Tắt ngắt tạm thời để ghi Flash Phrase (8-byte) */
        INT_SYS_DisableIRQGlobal();
        TempApp_Write(record.address, record.data, record.data_len);
        INT_SYS_EnableIRQGlobal();

        /* Trả về duy nhất 1 byte ACK (0x06) */
        LPUART1_transmit_char(SID_ACK);
    } else {
        /* Parse fail hoặc record không hợp lệ gửi NACK (0x15) */
        LPUART1_transmit_char(SID_NACK);
    }
}

int main(void) {
	WDOG_disable();
	Hardware_Init();

	/* Khởi tạo hàng đợi 4 phần tử */
	LineQueue_t srec_queue;
	Queue_Init(&srec_queue);

    /* Đọc trạng thái nút bấm SW2 (PTC12) */
    /* Lưu ý: Nút bấm trên board thường là Active High (Nhấn = 1) */
    uint32_t button = (PINS_DRV_ReadPins(PTC) >> 12) & 0x01;

    /* Cờ dùng để kiểm tra xem tại vùng MAIN đã có firmware hợp lệ chưa (kiểm tra flag "MAIN") */
	uint32_t *main_flag = (uint32_t *)STATUS_FLAG_ADDR;

    if (button == 0) {
        /* Nếu không nhấn button -> Nhảy sang User App */
		if (*main_flag == 0x4D41494E) {
			PINS_DRV_SetPins(PTD, (1 << 0));
			PINS_DRV_SetPins(PTD, (1 << 15));
			PINS_DRV_SetPins(PTD, (1 << 16));
			/* Bật đèn xanh dương để debug */
//			PINS_DRV_TogglePins(PTD, (1 << 0));
			JumpToUserApp(MAIN_APP_START_ADDR);
		}
    }
    else {
    	/* Khởi tạo ngắt UART1 */
    	LPUART1_init_interrupt();
    	LPIT0_init(0); // Timer 1s polling
    	TempApp_Prepare();
		SID_Init();    // Gửi REQ_SEND (0x3F)

    	/* Nếu nhấn button -> Nhảy sang Boot App */
    	LPUART1_transmit_string("BOOTLOADER ACTIVE. Send SREC now.\r\n");

    	char line_buffer[QUEUE_LINE_MAX];
		size_t len = 0;

        while (1) {
        	// Kiểm tra xem có tin nhắn (dòng) mới từ UART không
			if (LPUART1_ReadLine(line_buffer, &len)) {
				SID_Handle_Message(line_buffer, len, &srec_queue);
			}

			// Kiểm tra Timer cho Heartbeat và Timeout
			SID_Check_Timers();

			Bootloader_Handle(&srec_queue);

			/* Cho LED RED nháy chậm để biết đang ở chế độ chờ nạp */
			static uint32_t blink_cnt = 0;
			if (blink_cnt++ > 500000) {
				PINS_DRV_TogglePins(PTD, (1 << 15));
				blink_cnt = 0;
			}
		}
    }
}


