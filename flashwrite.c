
/**
 * Copyright © 2017 Tino Reichardt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * This code is derived from the doc_loadbios utility from here:
 * http://git.infradead.org/mtd-utils.git
 */

#include <mtd/mtd-user.h>

#include <sys/stat.h>
#include <sys/ioctl.h>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

unsigned char databuf[512];

int main(int argc, char **argv)
{
	mtd_info_t meminfo;
	int ifd, ofd, i;
	struct stat statbuf;
	erase_info_t erase;
	unsigned long retlen, ofs, iplsize, ipltailsize;
	unsigned char *iplbuf;
	iplbuf = NULL;

	if (argc < 3) {
		fprintf(stderr,
			"Usage: flashwrite /dev/mtd/X firmware.bin [offset]\n");
		return 1;
	}

	if ((ofd = open(argv[1], O_RDWR)) < 0) {
		perror("Open flash device");
		return 1;
	}

	if ((ifd = open(argv[2], O_RDONLY)) < 0) {
		perror("Open firmware file\n");
		close(ofd);
		return 1;
	}

	if (fstat(ifd, &statbuf) != 0) {
		perror("Stat firmware file");
		goto error;
	}

	if (ioctl(ofd, MEMGETINFO, &meminfo) != 0) {
		perror("ioctl(MEMGETINFO)");
		goto error;
	}

	iplsize = (ipltailsize = 0);
	if (argc >= 4) {
		iplsize = strtoul(argv[3], NULL, 0);
		ipltailsize = iplsize % meminfo.erasesize;
	}

	if (lseek(ofd, iplsize - ipltailsize, SEEK_SET) < 0) {
		perror("lseek");
		goto error;
	}

	if (ipltailsize) {
		iplbuf = malloc(ipltailsize);
		if (iplbuf == NULL) {
			fprintf(stderr,
				"Not enough memory for IPL tail buffer of"
				" %lu bytes\n", (unsigned long)ipltailsize);
			goto error;
		}
		printf("Reading IPL%s area of length %lu at offset %lu\n",
		       (iplsize - ipltailsize) ? " tail" : "",
		       ipltailsize, (iplsize - ipltailsize));
		if (read(ofd, iplbuf, ipltailsize) != (ssize_t) ipltailsize) {
			perror("read");
			goto error;
		}
	}

	erase.length = meminfo.erasesize;

	for (ofs = iplsize - ipltailsize;
	     ofs < iplsize + statbuf.st_size; ofs += meminfo.erasesize) {
		erase.start = ofs;
		printf("Performing Flash Erase of length %lu at offset %lu\n",
		       (long unsigned)erase.length, (long unsigned)erase.start);

		if (ioctl(ofd, MEMERASE, &erase) != 0) {
			perror("ioctl(MEMERASE)");
			goto error;
		}
	}

	if (lseek(ofd, iplsize - ipltailsize, SEEK_SET) < 0) {
		perror("lseek");
		goto error;
	}

	if (ipltailsize) {
		printf("Writing IPL%s area of length %lu at offset %lu\n",
		       (iplsize - ipltailsize) ? " tail" : "",
		       ipltailsize, (iplsize - ipltailsize));
		if (write(ofd, iplbuf, ipltailsize) != (ssize_t) ipltailsize) {
			perror("write");
			goto error;
		}
	}

	printf("Writing the firmware of length %lu at %lu...\n",
	       statbuf.st_size, iplsize);
	i = 0;
	do {
		retlen = read(ifd, databuf, 512);
		if (retlen < 512)
			memset(databuf + retlen, 0xff, 512 - retlen);
		if (write(ofd, databuf, 512) != 512) {
			perror("write");
			goto error;
		}

		i++;

		if (i % 16 == 0)
			printf(".");

		if (i > 16 * 64) {
			printf("\n");
			i = 0;
		}
		fflush(stdout);
	} while (retlen == 512);
	printf("Done.\n");

	if (iplbuf != NULL)
		free(iplbuf);
	close(ifd);
	close(ofd);
	return 0;

 error:
	if (iplbuf != NULL)
		free(iplbuf);
	close(ifd);
	close(ofd);

	return 1;
}
