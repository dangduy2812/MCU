#ifndef NVIC_H
#define NVIC_H

#include "Nvic_Registers.h"

#define NVIC_MAX_INTERRUPT_NUMBER 240

typedef struct
{
    unsigned int interruptNumber;  /**< Interrupt number to be configured */
    unsigned int priority;          /**< Priority level for the interrupt */
} Nvic_ConfigType;

/*!
 * @brief NVIC configuration structure
 *
 * This structure is used to configure the NVIC for a specific interrupt.
 *
 * @param[in] interruptNumber:  The interrupt number to be configured (IRQ number).
 * @param[in] priority:         The priority level for the interrupt (0 is highest priority).
 */

void NVIC_EnableInterrupt(unsigned int interruptNumber);
void NVIC_DisableInterrupt(unsigned int interruptNumber);
void NVIC_SetPending(unsigned int interruptNumber);
void NVIC_ClearPending(unsigned int interruptNumber);
unsigned int NVIC_GetActive(unsigned int interruptNumber);
void NVIC_SetPriority(unsigned int interruptNumber, unsigned int priority);
unsigned int NVIC_GetPriority(unsigned int interruptNumber);

#endif
