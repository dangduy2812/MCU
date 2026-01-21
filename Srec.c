/* --- START OF FILE Srec.c --- */
#include "Srec.h"
#include <string.h>

static uint8_t HexCharToVal(char c) {
    if (c >= '0' && c <= '9') return (uint8_t)(c - '0');
    if (c >= 'A' && c <= 'F') return (uint8_t)(c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (uint8_t)(c - 'a' + 10);
    return 0xFF;
}

static uint8_t HexStringToByte(const char* str) {
    uint8_t high = HexCharToVal(str[0]);
    uint8_t low = HexCharToVal(str[1]);
    return (uint8_t)((high << 4) | low);
}

static bool IsValidHexString(const char* str, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        if (HexCharToVal(str[i]) == 0xFF) return false;
    }
    return true;
}

static void TrimLine(const char* in, const char** start, size_t* len) {
    const char* s = in;
    const char* e = in;

    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') {
        s++;
    }
    while (*e != '\0') {
        e++;
    }
    while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r' || e[-1] == '\n')) {
        e--;
    }
    *start = s;
    *len = (size_t)(e - s);
}

SrecStatus_t SREC_ParseLine(const char* line, SrecLine_t* record) {
    uint8_t checksum_calc = 0;
    uint8_t checksum_read = 0;
    uint32_t addr_len = 0;
    const char* line_start = line;
    size_t line_len = 0;

    TrimLine(line, &line_start, &line_len);

    if (line_len < 4 || (line_start[0] != 'S' && line_start[0] != 's')) {
        return SREC_ERR_NO_START_S;
    }

    record->type = (uint8_t)(line_start[1] - '0');
    if (record->type > 9) return SREC_ERR_INVALID_TYPE;

    switch (record->type) {
        case 0: addr_len = 2; break;
        case 1: addr_len = 2; break;
        case 2: addr_len = 3; break;
        case 3: addr_len = 4; break;
        case 5: addr_len = 2; break;
        case 7: addr_len = 4; break;
        case 8: addr_len = 3; break;
        case 9: addr_len = 2; break;
        default: return SREC_ERR_INVALID_TYPE;
    }

    if (!IsValidHexString(&line_start[2], 2)) return SREC_ERR_INVALID_HEX;
    record->byte_count = HexStringToByte(&line_start[2]); //
    checksum_calc += record->byte_count;

    /* Expected length: "Sx" (2 chars) + 2 chars per byte in byte_count */
    if (line_len < (size_t)(2 + record->byte_count * 2)) return SREC_ERR_LENGTH;

    record->address = 0;//
    const char* pAddr = &line_start[4];
    for (uint32_t i = 0; i < addr_len; i++) {
        if (!IsValidHexString(pAddr, 2)) return SREC_ERR_INVALID_HEX;
        uint8_t val = HexStringToByte(pAddr);
        record->address = (record->address << 8) | val;
        checksum_calc += val;
        pAddr += 2;
    }

    if (record->byte_count < (addr_len + 1)) return SREC_ERR_LENGTH;
    record->data_len = record->byte_count - addr_len - 1;
    if (record->data_len > sizeof(record->data)) return SREC_ERR_LENGTH;

    const char* pData = pAddr;
    for (uint32_t i = 0; i < record->data_len; i++) {
        if (!IsValidHexString(pData, 2)) return SREC_ERR_INVALID_HEX;
        uint8_t val = HexStringToByte(pData);
        record->data[i] = val;
        checksum_calc += val;
        pData += 2;
    }

    const char* pCheck = pData;
    if (!IsValidHexString(pCheck, 2)) return SREC_ERR_INVALID_HEX;
    checksum_read = HexStringToByte(pCheck);

    checksum_calc = (uint8_t)(0xFFU ^ checksum_calc);
    if (checksum_calc != checksum_read) return SREC_ERR_CHECKSUM;

    return SREC_OK;
}

const char* SREC_GetErrorString(SrecStatus_t status) {
    switch (status) {
        case SREC_OK:               return "OK";
        case SREC_ERR_NO_START_S:   return "Error: No 'S' start";
        case SREC_ERR_INVALID_TYPE: return "Error: Invalid Type";
        case SREC_ERR_INVALID_HEX:  return "Error: Invalid Hex Char";
        case SREC_ERR_LENGTH:       return "Error: Length Mismatch";
        case SREC_ERR_CHECKSUM:     return "Error: Checksum Fail";
        default:                    return "Error: Unknown";
    }
}
