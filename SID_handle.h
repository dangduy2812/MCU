#ifndef SID_HANDLE_H
#define SID_HANDLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "LPUART.h"
#include "Srec.h"
#include "temp_app.h"
#include "main_update.h"
#include "clocks_and_modes.h"
#include "Timer_Lib.h"
#include <stdio.h>
#include <string.h>

/* Định nghĩa các mã SID */
#define SID_START      0x10
#define SID_DATA       0x20
#define SID_END        0x30
#define SID_HEARTBEAT  0x40
#define SID_TIMEOUT    0x50
#define SID_OVERFLOW   0x51
#define SID_ACK        0x06
#define SID_NACK       0x15
#define SID_REQ_SEND   0x3F

/* Các trạng thái của Bootloader */
typedef enum {
    STATE_IDLE = 0,
    STATE_PROGRAMMING
} BootState_t;

/* Hàm xử lý chính */
void SID_Handle_Message(char *line_buf, size_t line_len, LineQueue_t *app_queue);
void SID_Check_Timers(void);
void SID_Init(void);

#endif
