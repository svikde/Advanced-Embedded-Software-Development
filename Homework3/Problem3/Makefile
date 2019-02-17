obj-m= mod_timer.o
BROOTDIR:= ~/Buildroot/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220/
all:
	make ARCH=arm CROSS_COMPILE=/home/souvik/Buildroot/buildroot/output/host/usr/bin/arm-linux- -C $(BROOTDIR) M=$(shell pwd) modules

clean:
	make ARCH=arm CROSS_COMPILE=/home/souvik/Buildroot/buildroot/output/host/usr/bin/arm-linux- -C $(BROOTDIR) M=$(shell pwd) clean
