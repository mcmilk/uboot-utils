#ifndef CRC32_H
#define CRC32_H

#include <stdint.h>

extern uint32_t crc32(uint32_t crc, const unsigned char *buf, uint32_t len);

#endif
