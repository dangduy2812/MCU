#ifndef NVIC_REGISTERS_H
#define NVIC_REGISTERS_H

/**  NVIC (Nested Vectored Interrupt Controller) Registers */

typedef struct
{
    volatile unsigned int ISER[8];    /**< Interrupt Set Enable Register */
    volatile unsigned int ICER[8];    /**< Interrupt Clear Enable Register */
    volatile unsigned int ISPR[8];   /**< Interrupt Set Pending Register */
    volatile unsigned int ICPR[8];   /**< Interrupt Clear Pending Register */
    volatile const unsigned int IABR[8];   /**< Interrupt Active Bit Register */
    volatile unsigned char IPR[240];   /**< Interrupt Priority Register */
    volatile unsigned int STIR;       /**< Software Trigger Interrupt Register */
} NVIC_TypeDef;

/**< Base address for NVIC registers */
#define NVIC_BASE_ADDRESS 0xE000E100u

/**< Pointer to NVIC registers */
#define NVIC ((NVIC_TypeDef *) NVIC_BASE_ADDRESS)

#endif
