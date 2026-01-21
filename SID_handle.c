#include "SID_handle.h"

/* Biến nội bộ (static) để đảm bảo tính đóng gói */
static BootState_t g_boot_state = STATE_IDLE;
static uint32_t g_idle_seconds = 0;

#define TIMEOUT_LIMIT   5U
#define HEARTBEAT_LIMIT 2U

void SID_Init(void) {
    g_boot_state = STATE_IDLE;
    LPUART1_transmit_char((char)SID_REQ_SEND);
}

void SID_Handle_Message(char *line_buf, size_t line_len, LineQueue_t *app_queue) {
    if (line_len == 0) return;
    uint8_t sid = (uint8_t)line_buf[0];

    switch (sid) {
        case SID_START:
            g_boot_state = STATE_PROGRAMMING;
            g_idle_seconds = 0;
            LPUART1_transmit_char((char)SID_ACK);
//            TempApp_Prepare();
            break;

        case SID_DATA:
            if (g_boot_state == STATE_PROGRAMMING) {
                // Đẩy phần SREC (bỏ byte đầu SID) vào hàng đợi cho tầng Application xử lý
                if (Queue_Push(app_queue, &line_buf[1], line_len - 1)) {
                    LPUART1_transmit_char((char)SID_ACK);
                } else {
                    LPUART1_transmit_char((char)SID_NACK); // Queue đầy (4 dòng)
                }
            }
            break;

        case SID_END:
            // Đánh dấu để main biết cần thực hiện Migration
            g_boot_state = STATE_IDLE;
            LPUART1_transmit_char((char)SID_ACK);
            // Ghi cờ và báo tầng trên xử lý nốt (Migration làm ở Bootloader_Handle)
            Queue_Push(app_queue, "END", 3);
            break;

        case SID_HEARTBEAT:
            LPUART1_transmit_char((char)SID_ACK);
            break;
    }
}

void SID_Check_Timers(void) {
    if (LPIT0_Check_Timeout_Flag()) {
        /* Heartbeat gửi mỗi 2 giây */
        static uint32_t hb_cnt = 0;
        if (++hb_cnt >= HEARTBEAT_LIMIT) {
            LPUART1_transmit_char((char)SID_HEARTBEAT);
            hb_cnt = 0;
        }

        /* Timeout: Nếu đang Programming mà không thấy dữ liệu quá 5s */
        if (g_boot_state == STATE_PROGRAMMING) {
            if (++g_idle_seconds >= TIMEOUT_LIMIT) {
                LPUART1_transmit_char((char)SID_TIMEOUT);
                g_boot_state = STATE_IDLE;
                g_idle_seconds = 0;
            }
        }
    }
}
