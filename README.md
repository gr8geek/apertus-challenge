# apertus-challenge T884 task 1

sudo su

make -C /lib/modules/$(uname -r)/build M=$PWD modules

insmod chardvr.ko

To know major number:


grep chrdev /proc/devices

sudo mknod chrdev c major_number 0
 
gcc test.c

./a.out chrdev
