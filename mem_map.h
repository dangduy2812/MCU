#ifndef MEM_MAP_H
#define MEM_MAP_H

/* ================= FLASH MEMORY MAP ================= */

#define BOOT_START_ADDR       0x00000000UL
#define BOOT_END_ADDR         0x00007FFFUL

#define MAIN_APP_START_ADDR   0x00008000UL
#define MAIN_APP_END_ADDR     0x0003FFFFUL

#define TEMP_APP_START_ADDR   0x00040000UL
#define TEMP_APP_END_ADDR     0x0007EFFFUL

#define STATUS_FLAG_ADDR      0x0007F800UL

/* ================= FLASH PARAMETERS ================= */

#define FLASH_SECTOR_SIZE     0x1000UL   /* 4 KB */
#define FLASH_PHRASE_SIZE     8UL         /* 8 bytes */

/* ================= DEFINE APP AND SECTOR SIZE ================= */
#define APP_SIZE       (224 * 1024) // 224 KB
#define SECTOR_SIZE         4096 // 4KB

#endif
