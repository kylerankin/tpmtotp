CFLAGS = \
	-ggdb \
	-O3 \
	-Wp,-MMD,$(dir $@).$(notdir $@).d \
	-Wp,-MT,$@ \
	-W \
	-Wall \
	-Wextra \
	-std=c99 \
	-DTPM_POSIX=1 \
	-DTPM_NV_DISK=1 \
	-DTPM_AES=1 \
	-DTPM_V12=1 \
	-DTPM_USE_TAG_IN_STRUCTURE=1 \
	-DTPM_USE_CHARDEV=1 \
	-I ./libtpm \
	-I ../mbedtls-2.3.0/include \


PLYMOUTH_CFLAGS = `pkg-config --cflags ply-boot-client`

#LDLIBS=-Llibtpm -ltpm -loath /usr/lib/x86_64-linux-gnu/libcrypto.a -ldl
#LDLIBS=-Llibtpm -ltpm -loath ../libressl-2.4.1/crypto/.libs/libcrypto.a -ldl
#LDLIBS=-Llibtpm -ltpm -loath ../mbedtls-2.3.0/library/libmbedcrypto.a -ldl
LDLIBS=-Llibtpm -ltpm ../mbedtls-2.3.0/library/libmbedcrypto.a -ldl

PLYMOUTH_LDLIBS = `pkg-config --libs ply-boot-client`

APPS=sealtotp unsealtotp qrenc

all: libtpm/libtpm.a $(APPS)

libtpm/libtpm.a:
	$(MAKE) -C libtpm

unsealtotp: unsealtotp.o oath.o

plymouth-unsealtotp: plymouth-unsealtotp.c
	$(CC) $(CFLAGS) $(PLYMOUTH_CFLAGS) -o $@ $< $(PLYMOUTH_LDLIBS) $(LDLIBS)

qrenc: qrenc.c
	$(CC) \
		$(CFLAGS) \
		-I../qrencode-3.4.4 \
		-o $@ \
		$^ \
		../qrencode-3.4.4/.libs/libqrencode.so \
		$(LDLIBS)

sealtotp: sealtotp.c base32.c
	$(CC) \
		$(CFLAGS) \
		-I../qrencode-3.4.4 \
		-o $@ \
		$^ \
		../qrencode-3.4.4/.libs/libqrencode.so \
		$(LDLIBS)

clean:
	rm -f *.o $(APPS)
	$(MAKE) -C libtpm clean

install:
	install sealtotp unsealtotp plymouth-unsealtotp /usr/bin/
	install -D dracut/module-setup.sh /usr/lib/dracut/modules.d/60tpmtotp/module-setup.sh
	install -m 0644 tpmtotp.service /lib/systemd/system
	systemctl enable tpmtotp.service

uninstall:
	rm /usr/bin/sealtotp /usr/bin/unsealtotp /usr/bin/plymouth-unsealtotp
	rm -rf /usr/lib/dracut/modules.d/60tpmtotp/
	rm /lib/systemd/system/tpmtotp.service

-include .*.o.d
