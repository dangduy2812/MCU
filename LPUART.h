#ifndef LPUART_H_
#define LPUART_H_

#include "S32K144.h"
#include "device_registers.h"
#include "sdk_project_config.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "Nvic.h"
#include "Queue.h"

#define MAX_BUFFER_SIZE 128

/* --- LPUART0 (Dùng cho module ngoài/Debug) --- */
void LPUART0_init(void);
void LPUART0_transmit_char(char c);
void LPUART0_transmit_string(char* msg);

/* --- LPUART1 (Dùng cho dây USB Micro B lên Putty) --- */
void LPUART1_init(void);
void LPUART1_transmit_char(char c);
void LPUART1_transmit_string(const char* msg);

void LPUART1_init_interrupt(void);

bool LPUART1_ReadLine(char* out, size_t* out_len);
bool LPUART1_HasOverflow(void);
void LPUART1_ClearOverflow(void);

#endif
