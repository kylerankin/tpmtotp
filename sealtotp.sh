#!/bin/sh
# Generate a random secret, seal it with the PCRs
# and write it to the TPM NVRAM.
#

die() {
	echo >&2 "$@"
	exit 1
}

warn() {
	echo >&2 "$@"
}

dd if=/dev/urandom of=/tmp/secret bs=1 count=20 \
|| die "Unable to generate 20 random bytes"

# Use the current values of the PCRs, which will be read
# from the TPM as part of the sealing ("X").
sealfile2 \
	-if /tmp/secret \
	-of /tmp/sealed \
	-hk 40000000 \
	-ix 0 X \
	-ix 1 X \
	-ix 2 X \
	-ix 3 X \
	-ix 4 X \
|| die "Unable to seal secret"

# to create an nvram space we need the TPM owner password
# and the TPM physical presence must be asserted.
#
# The permissions are 0 since there is nothing special
# about the sealed file
setphysicalpresence -s \
|| warn "Warning: Unable to assert physical presence"

nv_definespace \
	-in 4d47 \
	-sz 312 \
	-pwdo abcd@1234 \
	-per 0 \
|| warn "Warning: Unable to define NVRAM space; trying anyway"


nv_writevalue \
	-in 4d47 \
	-if /tmp/sealed \
|| die "Unable to write sealed secret to NVRAM"

secret="`base32 < /tmp/secret`"

url="otpauth://totp/TPMTOTP?secret=$secret"

qrenc "$url"
echo "$url"
