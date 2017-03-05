
# Device: Allnet ALL0333CJ

This is a summary of flashing LEDE via tftp to this device.

```shell
# your own host must have an tftpd daemon, then do this:
ifconfig eth0:172 172.16.1.1 up

# telnet to allnet, root:admin
telnet 172.16.1.254

The following commands should be run on the Allnet Modem:

# kill this daemon, it spams your telnet console
killall dsl_cpe_control

# change to some place in ram:
cd /ramdisk/tftp_upload

# uImage should be your lede-lantiq-ase-ALL0333CJ-squashfs-sysupgrade.bin
tftp -g -r uImage 172.16.1.1

# https://github.com/mcmilk/uboot-utils/releases/download/0.1/mips32.tar.gz
# load these tools from your tftpd via:
tftp -g -r flashwrite 172.16.1.1
tftp -g -r fw_setenv 172.16.1.1
tftp -g -r fw.conf 172.16.1.1
ln -s fw_setenv fw_printenv
chmod +x *

# changing this variable is important, or LEDE will not boot:
./fw_setenv disable_recovery y

# this is also needed:
./fw_setenv kernel_addr 0xb0010000

# this will flash LEDE into the right address from fw.conf
./flashwrite /dev/mtd/1 uImage 0
```
