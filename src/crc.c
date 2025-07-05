#include <stddef.h>
#include <stdint.h>

uint8_t crc8(uint8_t msg[], size_t length, uint8_t init, uint8_t poly,
	     uint8_t xor)
{
	int i;
	uint8_t crc = init;
	for (i = 0; i < length; ++i) {
		crc = crc ^ msg[i];
		for (int j = 0; j < 8; ++j) {
			crc = (crc & 0x80) ? (crc << 1) ^ poly : (crc << 1);
		}
	}
	return crc ^ xor;
}
