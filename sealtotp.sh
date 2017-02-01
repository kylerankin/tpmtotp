#!/bin/sh
# Generate a random secret, seal it with the PCRs
# and write it to the TPM NVRAM.
#
# Pass in a hostname if you want to change it from the default string
#

die() {
	echo >&2 "$@"
	exit 1
}

warn() {
	echo >&2 "$@"
}

HOST="$1"
if [ -z "$HOST" ]; then
	HOST="TPMTOTP"
fi

dd \
	if=/dev/urandom \
	of=/tmp/secret \
	count=1 \
	bs=20 \
	2>/dev/null \
|| die "Unable to generate 20 random bytes"

secret="`base32 < /tmp/secret`"

# Use the current values of the PCRs, which will be read
# from the TPM as part of the sealing ("X").
# should this read the storage root key?
tpm sealfile2 \
	-if /tmp/secret \
	-of /tmp/sealed \
	-hk 40000000 \
	-ix 0 X \
	-ix 1 X \
	-ix 2 X \
	-ix 3 X \
	-ix 4 X \
|| die "Unable to seal secret"

rm /tmp/secret

# to create an nvram space we need the TPM owner password
# and the TPM physical presence must be asserted.
#
# The permissions are 0 since there is nothing special
# about the sealed file
tpm physicalpresence -s \
|| warn "Warning: Unable to assert physical presence"

read -s -p "TPM Owner password: " tpm_password
echo

tpm nv_definespace \
	-in 4d47 \
	-sz 312 \
	-pwdo "$tpm_password" \
	-per 0 \
|| die "Warning: Unable to define NVRAM space; trying anyway"


tpm nv_writevalue \
	-in 4d47 \
	-if /tmp/sealed \
|| die "Unable to write sealed secret to NVRAM"

rm /tmp/sealed

url="otpauth://totp/$HOST?secret=$secret"

qrenc "$url"
#echo "$url"
