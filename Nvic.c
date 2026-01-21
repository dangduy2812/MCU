#include "Nvic.h"

/*!
 * @brief NVIC configuration structure
 *
 * This structure is used to configure the NVIC for a specific interrupt.
 *
 * @param[in] interruptNumber:  The interrupt number to be configured (IRQ number).
 * @param[in] priority:         The priority level for the interrupt (0 is highest priority).
 */



 void NVIC_EnableInterrupt(unsigned int interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        NVIC->ISER[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

void NVIC_DisableInterrupt(unsigned int interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        NVIC->ICER[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

void NVIC_SetPending(unsigned int interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        NVIC->ISPR[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

void NVIC_ClearPending(unsigned int interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        NVIC->ICPR[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

unsigned int NVIC_GetActive(unsigned int interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        return ((NVIC->IABR[interruptNumber / 32] >> (interruptNumber % 32)) & 0x01u);
    }
    return 0;
}

void NVIC_SetPriority(unsigned int interruptNumber, unsigned int priority)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        NVIC->IPR[interruptNumber] = (priority & 0xFFu); // Assuming 8-bit priority
    }
}

unsigned int NVIC_GetPriority(unsigned int interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        return (NVIC->IPR[interruptNumber] & 0xFFu); // Assuming 8-bit priority
    }
    return 0;
}
