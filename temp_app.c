#include "temp_app.h"
#include "BoardDefines.h"

#define FLAG_TEMP_COMPLETE  0x54454D50UL /* "TEMP" */

void TempApp_Prepare(void)
{
    for (uint32_t addr = TEMP_APP_START_ADDR;
         addr < TEMP_APP_END_ADDR;
         addr += FLASH_SECTOR_SIZE)
    {
        Flash_EraseSector(addr);
    }
}

void TempApp_Write(uint32_t srec_addr, uint8_t *data, uint32_t len)
{
    uint32_t base =
        TEMP_APP_START_ADDR + (srec_addr - MAIN_APP_START_ADDR);

    uint32_t write_len = (len + 7U) & ~7U;  /* align to 8 bytes */

    for (uint32_t i = 0; i < write_len; i += FLASH_PHRASE_SIZE)
    {
        uint32_t addr = base + i;

        /* boundary check */
        if (addr >= TEMP_APP_END_ADDR)
        {
            return;
        }

        /* alignment check */
        if ((addr & 0x7U) != 0U)
        {
            return;
        }

        uint64_t phrase = 0xFFFFFFFFFFFFFFFFULL;

        for (uint32_t j = 0; j < FLASH_PHRASE_SIZE; j++)
        {
            if ((i + j) < len)
            {
                ((uint8_t*)&phrase)[7 - j] = data[i + j];
            }
        }

        Flash_ProgramPhrase(addr, phrase);
    }
}

void TempApp_SetComplete(void)
{
    Flash_EraseSector(STATUS_FLAG_ADDR);
//    Flash_ProgramPhrase(STATUS_FLAG_ADDR, ((uint64_t)FLAG_TEMP_COMPLETE << 32));
//    Flash_ProgramPhrase(STATUS_FLAG_ADDR, (uint64_t)FLAG_TEMP_COMPLETE);
    uint64_t phrase = 0x504D4554FFFFFFFFULL;
    Flash_ProgramPhrase(STATUS_FLAG_ADDR, phrase);
}

bool TempApp_IsComplete(void)
{
	LMEM->PCCCR |= 0x05000001; // Xóa Cache
    uint32_t flag = *(volatile uint32_t *)STATUS_FLAG_ADDR;
    return (flag == FLAG_TEMP_COMPLETE);
}
