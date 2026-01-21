#include "flash_drv.h"
#include "S32K144.h"

/* ================= FTFC status bits ================= */
#define CCIF_MASK    (1U << 7)
#define ACCERR_MASK  (1U << 5)
#define FPVIOL_MASK  (1U << 4)

/* ================= IRQ control (NO s32_core_cm4.h) ================= */
static inline uint32_t Flash_DisableIRQ(void)
{
    uint32_t primask;
    __asm volatile (
        "mrs %0, primask\n"
        "cpsid i\n"
        : "=r"(primask) :: "memory"
    );
    return primask;
}

static inline void Flash_RestoreIRQ(uint32_t primask)
{
    __asm volatile (
        "msr primask, %0\n"
        :: "r"(primask) : "memory"
    );
}

/* ================= Internal helpers ================= */
static void Flash_Wait(void)
{
    while ((FTFC->FSTAT & CCIF_MASK) == 0U)
    {
        /* wait */
    }
}

static void Flash_ClearError(void)
{
    /* Write 1 to clear error flags */
    FTFC->FSTAT = ACCERR_MASK | FPVIOL_MASK;
}

/* ================= Public APIs ================= */

__attribute__((section(".code_ram")))
bool Flash_EraseSector(uint32_t address)
{
    uint32_t irq_state = Flash_DisableIRQ();   /* <<< CRITICAL */

    Flash_Wait();
    Flash_ClearError();

    /* Erase Sector command */
    FTFC->FCCOB[0] = 0x09U;
    FTFC->FCCOB[1] = (address >> 16) & 0xFFU;
    FTFC->FCCOB[2] = (address >> 8)  & 0xFFU;
    FTFC->FCCOB[3] =  address        & 0xFFU;

    /* Launch command */
    FTFC->FSTAT = CCIF_MASK;
    Flash_Wait();

    Flash_RestoreIRQ(irq_state);               /* <<< RESTORE */

    return true;
}

__attribute__((section(".code_ram")))
bool Flash_ProgramPhrase(uint32_t address, uint64_t data)
{
    uint32_t irq_state = Flash_DisableIRQ();   /* <<< CRITICAL */

    Flash_Wait();
    Flash_ClearError();

    /* Program Phrase command */
    FTFC->FCCOB[0] = 0x07U;
    FTFC->FCCOB[1] = (address >> 16) & 0xFFU;
    FTFC->FCCOB[2] = (address >> 8)  & 0xFFU;
    FTFC->FCCOB[3] =  address        & 0xFFU;

    /* Data (8 bytes, MSB first) */
    FTFC->FCCOB[4] = (uint8_t)(data >> 56);
    FTFC->FCCOB[5] = (uint8_t)(data >> 48);
    FTFC->FCCOB[6] = (uint8_t)(data >> 40);
    FTFC->FCCOB[7] = (uint8_t)(data >> 32);
    FTFC->FCCOB[8] = (uint8_t)(data >> 24);
    FTFC->FCCOB[9] = (uint8_t)(data >> 16);
    FTFC->FCCOB[10]= (uint8_t)(data >> 8);
    FTFC->FCCOB[11]= (uint8_t)(data);

    /* Launch command */
    FTFC->FSTAT = CCIF_MASK;
    Flash_Wait();

    Flash_RestoreIRQ(irq_state);               /* <<< RESTORE */

    return true;
}

void Flash_Read(uint32_t address, uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        buf[i] = *(volatile uint8_t *)(address + i);
    }
}
