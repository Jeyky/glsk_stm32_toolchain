#include <libopencm3/stm32/spi.h>

void spi_init(void);
uint8_t flash_read_status_register(uint32_t spi);
uint16_t flash_read_id(uint32_t spi);
uint8_t flash_status(uint32_t spi);
void flash_wr_disable(uint32_t spi);
void flash_wr_enable(uint32_t spi);
void flash_erase_4kbyte(uint32_t spi, uint32_t address);
void flash_byte_program(uint32_t spi, uint32_t address, uint8_t data);
void flash_read_data(uint32_t spi,uint32_t address, uint8_t *data, uint32_t len);
void cs_set(bool state);
void flash_get_password(uint8_t *pass_arr);
void flash_get_threshold(uint8_t *pass_arr);

extern const sk_pin sk_io_spiflash_ce;