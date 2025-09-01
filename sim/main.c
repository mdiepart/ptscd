#include <stdio.h>
#include <stdlib.h>

#include "littlefs/lfs.h"
#include "littlefs/bd/lfs_emubd.h"
#include "ptscd.h"

int main(int argc, char *argv[])
{
	lfs_t lfs;
	lfs_emubd_t bd;
	lfs_file_t lfs_file;

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

	// Create minimal set of files and headers
	printf("Created \"ptscd.dat\".\r\n");
	lfs_file_open(&lfs, &lfs_file, "ptscd.dat", LFS_O_CREAT | LFS_O_RDWR);
	char str[] = "PTSCD 0.1p1\nchannels.dat\ncontacts.dat\ngroups.dat\n";
	lfs_file_write(&lfs, &lfs_file, str, sizeof(str)-1);
	lfs_file_close(&lfs, &lfs_file);
	printf("Closed \"ptscd.dat\".\r\n");

	printf("Created \"channels.dat\".\r\n");
	lfs_file_open(&lfs, &lfs_file, "channels.dat", LFS_O_CREAT | LFS_O_RDWR);
	lfs_file_close(&lfs, &lfs_file);
	printf("Closed \"channels.dat\".\r\n");

	printf("Created \"contacts.dat\".\r\n");
	lfs_file_open(&lfs, &lfs_file, "contacts.dat", LFS_O_CREAT | LFS_O_RDWR);
	lfs_file_close(&lfs, &lfs_file);
	printf("Closed \"contacts.dat\".\r\n");

	printf("Created \"groups.dat\".\r\n");
	lfs_file_open(&lfs, &lfs_file, "groups.dat", LFS_O_CREAT | LFS_O_RDWR);
	lfs_file_close(&lfs, &lfs_file);
	printf("Closed \"groups.dat\".\r\n");


	ptscd_t *cps = ptscd_init(&lfs);
	if(cps == NULL)
	{
		printf("ptscd_init() returned NULL.\r\n");
	}
	else 
	{
		printf("Initialized PTSCD\r\n");
		err = ptscd_open(cps, NULL);
		printf("ptscd_open() returned %d.\r\n", err);
	}
	

	

	err = lfs_unmount(&lfs);
	if(err)
	{
		return err;
	}
	printf("Unmounted block device\r\n");

	lfs_emubd_destroy(&cfg);
	printf("Destroyed block device\r\n");

}

