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

NOPROGS += \
	pcrread \
	ownerreadinternalpub \
	activateidentity \
	calcfuturepcr \
	certifykey \
	certifyselftest \
	chgauth \
	chgtpmauth \
	clearown \
	cmk_approvema \
	cmk_createticket \
	cmk_loadmigrationblob \
	cmk_setrestrictions \
	counter_calc_incr \
	counter_create \
	counter_increment \
	counter_read \
	counter_release \
	createek \
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
	extend \
	flushspecific \
	forceclear \
	getauditdigest \
	getauditdigestsigned \
	getcontextcount \
	getpubek \
	getpubkey \
	getticks \
	identity \
	keycontrol \
	killmaintenancefeature \
	libtpm-config \
	listkeys \
	loadauthcontext \
	loadcontext \
	loadkey \
	loadkeycontext \
	loadmanumaintpub \
	loadmigrationblob \
	loadownerdelegation \
	migrate \
	migratekey \
	nv \
	nv_definespace \
	nv_readvalue \
	nv_writevalue \
	ownerreadinternalpub \
	ownersetdisable \
	pcrread \
	pcrreset \
	physicaldisable \
	physicalenable \
	physicalpresence \
	physicalsetdeactivated \

NO = \
	bindfile \
	cmk_createkey \
	cmk_migrate \
	createkey \
	quote \
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
	sealfile2 \
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
	takeown \
	tickstampblob \
	tpmbios \
	tpminit \
	tpmreset \
	transport_test \
	unbindfile \
	unsealfile \
	unsealxfile \
	updateverification \
	verifydelegation \
	verifyfile \

all: $(PROGS)
%: %.c
	$(CC) $(CFLAGS) -o $@ $< -L../libtpm -ltpm -L../../mbedtls-2.3.0/library -lmbedcrypto

CFLAGS += \
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
	-I ../../mbedtls-2.3.0/include \


clean: $(FORCE)
	$(RM) $(PROGS)
