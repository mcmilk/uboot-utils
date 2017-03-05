
# Binary utilities for flashing routers

This repository can be used for building / downloading binaries that are
needed for flashing firmware to routers.


## flashwrite

- this tool is based on doc_loadbios from mtd-utils
- the usage is `flashwrite /dev/mtd/X firmware.bin [offset]`

## fw_setenv / fw_printenv

- these two tools are based on uboot-envtools_20081215.orig.tar.gz
- they can be used for setting up and printing uboot environments


/TR
