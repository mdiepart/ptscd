#include <stdio.h>
#include <stdlib.h>

#include "littlefs/lfs.h"
#include "littlefs/bd/lfs_emubd.h"

int main(int argc, char *argv[])
{
	lfs_t lfs;
	lfs_emubd_t bd;

	const struct lfs_config cfg = 
	{
		.context = &bd,
		.read = &lfs_emubd_read,
		.prog = &lfs_emubd_prog,
		.erase = &lfs_emubd_erase,
		.sync = &lfs_emubd_sync,

		.read_size = 1,
		.prog_size = 1,
		.block_size = 4096,
		.block_count = 4096, // 16 MB (MD-UV3x0)
		.block_cycles = 500,
		.cache_size = 16,
		.lookahead_size = 256,
	};

	const struct lfs_emubd_config emu_cfg = 
	{
		.read_size = 1,
		.prog_size = 1,
		.erase_size = 4096,
		.erase_count = 4096,
		.erase_value = 0xFF,
		.erase_cycles = 500,
		.badblock_behavior = LFS_EMUBD_BADBLOCK_READERROR,
		.power_cycles = 0,
		.disk_path = "./flash.bin",
	};

	int err = lfs_emubd_create(&cfg, &emu_cfg);
	if(err)
	{
		return err;
	}
	printf("Created emulation block device\r\n");

	err = lfs_format(&lfs, &cfg);
	if(err)
	{
		return err;
	}
	printf("Formatted block device\r\n");

	err = lfs_mount(&lfs, &cfg);
	if(err) 
	{
		return err;
	}
	printf("Mounted block device\r\n");

	err = lfs_unmount(&lfs);
	if(err)
	{
		return err;
	}
	printf("Unmounted block device\r\n");

	lfs_emubd_destroy(&cfg);
	printf("Destroyed block device\r\n");

}

