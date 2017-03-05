############################################################
# Makefile for building build small and static utilities
# - mips32 should run fine on big endian based mips routers
#
# /TR 2016-03-05
############################################################

# replace with your preferred cross compiler
CC	= mips-gcc
STRIP	= mips-strip

# replace with your cflags
CFLAGS	= -W -Wall -Os -pipe -march=mips32
LDFLAGS	= -static

all:	clean flashwrite fw_setenv fw_printenv

flashwrite:
	$(CC) $(CFLAGS) $(LDFLAGS) -o flashwrite flashwrite.c
	$(STRIP) -R .note -R .comment flashwrite

fw_setenv:
	$(CC) $(CFLAGS) $(LDFLAGS) -o fw_setenv crc32.c fw_setenv.c
	$(STRIP) -R .note -R .comment fw_setenv

fw_printenv: fw_setenv
	ln -s fw_setenv fw_printenv

clean:
	rm -f flashwrite fw_setenv fw_printenv
