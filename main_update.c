#include "main_update.h"
#include "interrupt_manager.h"

#define FLAG_MAIN_VALID  0x4D41494EUL /* "MAIN" */

__attribute__((section(".code_ram")))
void MainApp_Erase(void)
{
    for(uint32_t addr = MAIN_APP_START_ADDR;
        addr < MAIN_APP_END_ADDR;
        addr += FLASH_SECTOR_SIZE)
    {
        Flash_EraseSector(addr);
    }
}

__attribute__((section(".code_ram")))
void Copy_Temp_To_Main(uint32_t fw_size)
{
    uint8_t buf[256];

    uint32_t max_size = MAIN_APP_END_ADDR - MAIN_APP_START_ADDR;
    if (fw_size > max_size)
        fw_size = max_size;

    /* Disable IRQ for entire copy */
    INT_SYS_DisableIRQGlobal();

    for (uint32_t offset = 0; offset < fw_size; offset += sizeof(buf))
    {
        uint32_t chunk = sizeof(buf);
        if (offset + chunk > fw_size)
            chunk = fw_size - offset;

        Flash_Read(TEMP_APP_START_ADDR + offset, buf, chunk);

        for (uint32_t i = 0; i < chunk; i += FLASH_PHRASE_SIZE)
        {
            uint32_t addr = MAIN_APP_START_ADDR + offset + i;

            /* Alignment check */
            if ((addr & 0x7U) != 0U)
                break;

            uint64_t phrase = 0xFFFFFFFFFFFFFFFFULL;

            for (uint32_t j = 0; j < FLASH_PHRASE_SIZE; j++)
            {
                if (i + j < chunk)
                    ((uint8_t*)&phrase)[7 - j] = buf[i + j];
            }

            Flash_ProgramPhrase(addr, phrase);
        }
    }

    INT_SYS_EnableIRQGlobal();
}

__attribute__((section(".code_ram")))
void MainApp_SetValid(void)
{
    Flash_EraseSector(STATUS_FLAG_ADDR);
//    Flash_ProgramPhrase(STATUS_FLAG_ADDR, ((uint64_t)FLAG_MAIN_VALID << 32));
//    Flash_ProgramPhrase(STATUS_FLAG_ADDR, (uint64_t)FLAG_MAIN_VALID);
    uint64_t phrase = 0x4E49414DFFFFFFFFULL;
	Flash_ProgramPhrase(STATUS_FLAG_ADDR, phrase);
}
