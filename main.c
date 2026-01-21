#include "mem_map.h"
#include "sdk_project_config.h"
#include "interrupt_manager.h"   /* Chá»©a INT_SYS_DisableIRQGlobal */
#include "device_registers.h"    /* Chá»©a Ä‘á»‹nh nghÄ©a cÃ¡c thanh ghi core nhÃ¢n ARM */
#include "pins_driver.h"
#include "BoardDefines.h"
#include "clocks_and_modes.h"
#include "LPUART.h"
#include "temp_app.h"
#include "main_update.h"
#include "Queue.h"
#include "Srec.h"
#include "SID_handle.h"

#define SID_ACK   0x06
#define SID_NACK  0x15

void JumpToUserApp(uint32_t address) {
    /* 1. Láº¥y Stack Pointer vÃ  Reset Handler trá»±c tiáº¿p tá»« Ä‘á»‹a chá»‰ 0x8000 */
    uint32_t stackPointer = *((volatile uint32_t*)address);
    uint32_t jumpAddress = *((volatile uint32_t*)(address + 4));

     /* CHECK MSP: pháº£i náº±m trong SRAM */
    if ((stackPointer & 0x2FFE0000U) != 0x20000000U)
    {
        return;   // MAIN khÃ´ng há»£p lá»‡ â†’ á»Ÿ láº¡i BOOT
    }

    /* 2. VÃ´ hiá»‡u hÃ³a ngáº¯t */
    INT_SYS_DisableIRQGlobal();

    /* 3. Thiáº¿t láº­p láº¡i Vector Table Offset Register (VTOR) */
    S32_SCB->VTOR = address;

    /* 4. Thiáº¿t láº­p Stack Pointer báº±ng lá»‡nh Assembly (Ä�áº£m báº£o Build Ä‘Æ°á»£c) */
    __asm volatile ("MSR msp, %0" : : "r" (stackPointer) : "sp");

    /* 5. Nháº£y sang User App */
    void (*app_reset_handler)(void) = (void (*)(void))jumpAddress;
    app_reset_handler();
}

void System_Reset(void) {
	S32_SCB->AIRCR = S32_SCB_AIRCR_VECTKEY(0x05FA) | S32_SCB_AIRCR_SYSRESETREQ_MASK;
}

void Hardware_Init(void) {
    /* Khá»Ÿi táº¡o Clock (HÃ m trong clocks_and_modes.h cá»§a báº¡n) */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    FIRC_init_48MHz();
    NormalRUNmode_80MHz();

    /* Khá»Ÿi táº¡o Pins vÃ  UART */
    Board_Init();
    LPUART1_init();
}

static void WDOG_disable(void) {
    WDOG->CNT = 0xD928C520;
    WDOG->TOVAL = 0x0000FFFF;
    WDOG->CS = 0x00002120;
}

//void Bootloader_Handle(LineQueue_t *q) {
//    char srec_line[QUEUE_LINE_MAX];
//    size_t srec_len = 0;
//    SrecLine_t record;
//    SrecStatus_t status;

//    if (Queue_Pop(q, srec_line, &srec_len)) {

//        /* 1. KIá»‚M TRA TÃ�N HIá»†U Káº¾T THÃšC Tá»ª Lá»šP SID */
//        if (strcmp(srec_line, "END") == 0) {
//            LPUART1_transmit_string("SREC Reception Finished. Verifying...\r\n");
//            TempApp_SetComplete();

//            if (TempApp_IsComplete()) {
//                LPUART1_transmit_string("Verification OK. Copying TEMP to MAIN...\r\n");
//                MainApp_Erase();
//                Copy_Temp_To_Main();
//                MainApp_SetValid();
//                LPUART1_transmit_string("Update Success! System ready to run new App.\r\n");
//                System_Reset();
//            } else {
//                LPUART1_transmit_string("Error: TEMP Verification failed!\r\n");
//            }
//            return; // ThoÃ¡t hÃ m sau khi xá»­ lÃ½ xong END
//        }

//        /* 2. Náº¾U KHÃ”NG PHáº¢I "END", TIáº¾N HÃ€NH GIáº¢I MÃƒ SREC NHÆ¯ BÃŒNH THÆ¯á»œNG */
//        status = SREC_ParseLine(srec_line, &record);

//        if (status == SREC_OK) {
//            if (record.type >= 1 && record.type <= 3) {
//                INT_SYS_DisableIRQGlobal();
//                TempApp_Write(record.address, record.data, record.data_len);
//                INT_SYS_EnableIRQGlobal();
//            }
//            // Báº¡n khÃ´ng cáº§n xá»­ lÃ½ S7/S8/S9 á»Ÿ Ä‘Ã¢y ná»¯a vÃ¬ Ä‘Ã£ cÃ³ SID_END quáº£n lÃ½
//        } else {
//            LPUART1_transmit_string(SREC_GetErrorString(status));
//            LPUART1_transmit_string("\r\n");
//        }
//    }
//}

void Bootloader_Handle(LineQueue_t *q)
{
    char srec_line[QUEUE_LINE_MAX];
    size_t srec_len = 0;
    SrecLine_t record;
    SrecStatus_t status;
 
    /* Không có dòng mới thì thoát */
    if (!Queue_Pop(q, srec_line, &srec_len))
        return;
 
    /* ================= END COMMAND ================= */
    if (strcmp(srec_line, "END") == 0)
    {
        /* Không in text khi đang update */
        INT_SYS_DisableIRQGlobal();
        TempApp_SetComplete();
        INT_SYS_EnableIRQGlobal();
 
        /* ACK cho END */
        LPUART1_transmit_char(SID_ACK);
        return;
    }
 
    /* ================= SREC LINE ================= */
    status = SREC_ParseLine(srec_line, &record);
 
    if (status == SREC_OK &&
        record.type >= 1 &&
        record.type <= 3)
    {
        /* Ghi flash trong critical section */
        INT_SYS_DisableIRQGlobal();
        TempApp_Write(record.address, record.data, record.data_len);
        INT_SYS_EnableIRQGlobal();
 
        /* ACK ngay sau khi ghi xong */
        LPUART1_transmit_char(SID_ACK);
    }
    else
    {
        /* Parse fail hoặc record không hợp lệ */
        LPUART1_transmit_char(SID_NACK);
    }
}

int main(void) {
	WDOG_disable();
	Hardware_Init();

	/* Khá»Ÿi táº¡o hÃ ng Ä‘á»£i 4 pháº§n tá»­ */
	LineQueue_t srec_queue;
	Queue_Init(&srec_queue);

    /* Ä�á»�c tráº¡ng thÃ¡i nÃºt báº¥m SW2 (PTC12) */
    /* LÆ°u Ã½: NÃºt báº¥m trÃªn board thÆ°á»�ng lÃ  Active High (Nháº¥n = 1) */
    uint32_t button = (PINS_DRV_ReadPins(PTC) >> 12) & 0x01;

    /* Cá»� dÃ¹ng Ä‘á»ƒ kiá»ƒm tra xem táº¡i vÃ¹ng MAIN Ä‘Ã£ cÃ³ firmware há»£p lá»‡ chÆ°a (kiá»ƒm tra flag "MAIN") */
	uint32_t *main_flag = (uint32_t *)STATUS_FLAG_ADDR;

    if (button == 0) {
        /* Náº¿u khÃ´ng nháº¥n button -> Nháº£y sang User App */
		if (*main_flag == 0x4D41494E) {
			PINS_DRV_SetPins(PTD, (1 << 0));
			PINS_DRV_SetPins(PTD, (1 << 15));
			PINS_DRV_SetPins(PTD, (1 << 16));
			LPUART1_transmit_string("Valid App found. Jumping...\r\n");
			JumpToUserApp(MAIN_APP_START_ADDR);
		}
    }
    else {
    	/* Khá»Ÿi táº¡o ngáº¯t UART1 */
        //TempApp_Prepare();
    	LPUART1_init_interrupt();
    	LPIT0_init(0); // Timer 1s polling
    	TempApp_Prepare();
		SID_Init();    // Gá»­i REQ_SEND (0x3F)

    	/* Náº¿u nháº¥n button -> Nháº£y sang Boot App */
    	LPUART1_transmit_string("BOOTLOADER ACTIVE. Send SREC now.\r\n");

    	char line_buffer[QUEUE_LINE_MAX];
		size_t len = 0;

        while (1) {
        	// Kiá»ƒm tra xem cÃ³ tin nháº¯n (dÃ²ng) má»›i tá»« UART khÃ´ng
			if (LPUART1_ReadLine(line_buffer, &len)) {
				SID_Handle_Message(line_buffer, len, &srec_queue);
			}

			// Kiá»ƒm tra Timer cho Heartbeat vÃ  Timeout
			SID_Check_Timers();

			Bootloader_Handle(&srec_queue);

			/* Cho LED RED nhÃ¡y cháº­m Ä‘á»ƒ biáº¿t Ä‘ang á»Ÿ cháº¿ Ä‘á»™ chá»� náº¡p */
			static uint32_t blink_cnt = 0;
			if (blink_cnt++ > 500000) {
				PINS_DRV_TogglePins(PTD, (1 << 15));
				blink_cnt = 0;
			}
		}
    }
}


