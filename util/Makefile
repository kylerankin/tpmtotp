# The programs that we actually use
PROGS += \
	nv \
	nv_definespace \
	nv_readvalue \
	nv_writevalue \
	ownersetdisable \
	pcrreset \
	physicaldisable \
	physicalenable \
	physicalpresence \
	physicalsetdeactivated \
	getcapability \
	clearown \
	counter_create \
	counter_increment \
	counter_read \
	counter_release \
	sealfile2 \
	unsealfile \
	forceclear \
	takeown \
	extend \
	quote \
	listkeys \
	loadkey \
	createek \
	getpubkey \
	getpubek \
	ownerreadinternalpub \
	activateidentity \
	identity \
	calcfuturepcr \

NOPROGS += \
	pcrread \
	certifykey \
	certifyselftest \
	chgauth \
	chgtpmauth \
	cmk_approvema \
	cmk_createticket \
	cmk_loadmigrationblob \
	cmk_setrestrictions \
	counter_calc_incr \
	createkeydelegation \
	createownerdelegation \
	createrevek \
	delegatemanage \
	delegatereadtable \
	dirread \
	dirwrite \
	disableforceclear \
	disableownerclear \
	disablepubek \
	dumpkey \
	enableaudit \
	evictkey \
	flushspecific \
	forceclear \
	getauditdigest \
	getauditdigestsigned \
	getcontextcount \
	getticks \
	keycontrol \
	killmaintenancefeature \
	libtpm-config \
	loadauthcontext \
	loadcontext \
	loadkeycontext \
	loadmanumaintpub \
	loadmigrationblob \
	loadownerdelegation \
	migrate \
	migratekey \
	ownerreadinternalpub \

NO = \
	bindfile \
	cmk_createkey \
	cmk_migrate \
	createkey \
	quote2 \

NOPROGS += \
	random \
	readmanumaintpub \
	resetestbit \
	resetlockvalue \
	revtrust \
	saveauthcontext \
	savecontext \
	savekeycontext \
	savestate \
	sealfile \
	sealxfile \
	selftest \
	session \
	setcapability \
	setoperatorauth \
	setownerinstall \
	setownerpointer \
	settempdeactivated \
	sha \
	sha1parts \
	sha1start \
	signfile \
	signmsg \
	tickstampblob \
	tpmbios \
	tpminit \
	tpmreset \
	transport_test \
	unbindfile \
	unsealxfile \
	updateverification \
	verifydelegation \
	verifyfile \

all: tpm

EXTRA_LDFLAGS += \
	-L../libtpm \
	-ltpm \
	-lmbedcrypto \

EXTRA_CFLAGS = \
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
	-I ../libtpm \

%.o: %.c
	$(CC) $(EXTRA_CFLAGS) $(CFLAGS) -o $@ -c $<

# Generate the wrapper program with all of the executables linked in
tpm: tpm_command.o $(foreach i,$(PROGS),$i.o)
	$(MAKE) -C ../libtpm
	$(CC) -o $@ $^ $(LDFLAGS) $(EXTRA_LDFLAGS)

clean: $(FORCE)
	$(RM) *.o .*.o.d tpm core

-include .*.d
