#include <stdio.h>
#include <stdlib.h>

#include "littlefs/lfs.h"

int main(int argc, char *argv[])
{
	lfs_t lfs;
	const struct lfs_config cfg = 
	{
		.read = ,
		.prog = ,
		.erase = ,
		.sync = ,

		.read_size = 1,
		.prog_size = 1,
		.block_size = 4096,
		.block_count = 4096, // 16 MB (MD-UV3x0)
		.block_cycles = 500,
		.cache_size = 16,
		.lookahead_size = 32,


}	
