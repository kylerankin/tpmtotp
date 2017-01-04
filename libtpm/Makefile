EXTRA_CFLAGS = \
	-Werror=implicit-function-declaration \
	-fPIC \
	-Wall \
	-O3 \
	-Wp,-MMD,$(dir $@).$(notdir $@).d \
	-Wp,-MT,$@ \
	-DTPM_POSIX=1 \
	-DTPM_NV_DISK=1 \
	-DTPM_AES=1 \
	-DTPM_V12=1 \
	-DTPM_USE_TAG_IN_STRUCTURE=1 \
	-DTPM_USE_CHARDEV=1 \
	$(CONFIG_OPENSSL) \
	-I. \

LIBRARY = libtpm.so

HEADERS = tpmfunc.h tpm.h tpmkeys.h

EXTRA_DIST = hmac.h oiaposap.h pcrs.h tpmutil.h

SOURCES = \
	auditing.c \
	chgauth.c \
	context.c \
	counter.c \
	daa.c \
	debug.c \
	dir.c \
	eviction.c \
	hmac.c \
	identity.c \
	maintenance.c \
	management.c \
	migrate.c \
	miscfunc.c \
	oiaposap.c \
	optin.c \
	owner.c \
	ownertpmdiag.c \
	raw.c \
	rng.c \
	serialize.c \
	session.c \
	sha.c \
	signature.c \
	startup.c \
	testing.c \
	ticks.c \
	tpmutil.c \
	tpmutil_sock.c \
	tpmutil_tty.c \
	tpmutil_unixio.c \
	tpmutil_libtpms.c \
	transport.c \
	seal.c \
	keys.c \
	pcrs.c \
	bind.c \
	delegation.c \
	keyswap.c \
	nv.c \


OBJECTS = $(SOURCES:.c=.o)

$(LIBRARY): $(OBJECTS)
	@-$(RM) "$@"
	$(CC) -shared -o "$@" $^

%.o: %.c
	$(CC) $(EXTRA_CFLAGS) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) -f *.o *.a


-include .*.o.d
