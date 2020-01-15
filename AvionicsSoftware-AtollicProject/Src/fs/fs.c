#include "fs.h"
#include <stdint.h>
#include "flash.h"
#include "spiffs.h"
#include <assert.h>
#include <time.h>

#define PHY_SIZE                                   FLASH_END_ADDRESS
#define PHY_ADDR                                 FLASH_START_ADDRESS
#define PHYS_ERASE_BLOCK                                        4096

#define LOG_BLOCK_SIZE                                          4096
#define LOG_PAGE_SIZE                                            256

#define FILE_SIZE_MAX                                           4096
#define CHUNK_SIZE_MAX                                           512

struct Flash;
static uint8_t fdworkspace[192];
static uint8_t cache[1400];
Flash flash;

static struct spiffs_t fs;
static struct spiffs_config_t config;
static uint8_t workspace[2 * LOG_PAGE_SIZE];


static int hal_init(void)
{
	flash = flash_initialize();
	flash_erase_device(flash);
	return (0);
}

static s32_t hal_erase(struct spiffs_t *fs_p,
					   u32_t addr,
					   u32_t size)
{
	return (flash_erase_param_sector(flash, addr /*, how to control how much i want to erase?*/));
}


static s32_t hal_read(struct spiffs_t *fs_p, u32_t addr, u32_t size, u8_t *dst_p)
{
	if (flash_read(flash, addr, dst_p, size) != size)
	{
		return (-1);
	}
	
	return (0);
}

static s32_t hal_write(struct spiffs_t *fs_p, u32_t addr, u32_t size, u8_t *src_p)
{
	if (flash_write(flash, addr, src_p, size) != size) {
		return (-1);
	}
	
	return (0);
}



int fs_init(void)
{
	assert(hal_init() == 0);
	
	/* Initiate the config struct. */
	config.hal_read_f  = hal_read;
	config.hal_write_f = hal_write;
	config.hal_erase_f = hal_erase;
	config.phys_size = PHY_SIZE;
	config.phys_addr = PHY_ADDR;
	config.phys_erase_block = PHYS_ERASE_BLOCK;
	config.log_block_size = LOG_BLOCK_SIZE;
	config.log_page_size = LOG_PAGE_SIZE;
	
	/* Mount the file system to initialize the runtime variables. */
	assert(SPIFFS_mount(&fs,
						&config,
						workspace,
						fdworkspace,
						sizeof(fdworkspace),
						cache,
						sizeof(cache),
						NULL) != 0);
	
	/* Format and mount the file system again. */
	assert(SPIFFS_format(&fs) == 0);
	assert(SPIFFS_mount(&fs,
						&config,
						workspace,
						fdworkspace,
						sizeof(fdworkspace),
						cache,
						sizeof(cache),
						NULL) == 0);
	
	return (0);
}


int fs_test(void)
{
	char buf[12];
	spiffs_file fd;
	
	/* Create a file, delete previous if it already exists, and open
	   it for reading and writing. */
	fd = SPIFFS_open(&fs,
					 "file.txt",
					 SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR,
					 0);
	assert(fd >= 0);
	
	/* Write to it. */
	assert(SPIFFS_write(&fs, fd, "Hello world", 12) == 12);
	
	/* Close it. */
	assert(SPIFFS_close(&fs, fd) == 0);
	
	/* Open it. */
	fd = SPIFFS_open(&fs, "file.txt", SPIFFS_RDWR, 0);
	assert(fd >= 0);
	
	/* Read it. */
	assert(SPIFFS_read(&fs, fd, buf, 12) == 12);
	assert(strcmp(buf, "Hello world") == 0);
	
	/* Close it. */
	assert(SPIFFS_close(&fs, fd) == 0);
	
	return (0);
}