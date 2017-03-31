#!/bin/sh
# Retrieve the sealed file from the NVRAM, unseal it and compute the totp

die() {
	echo >&2 "$@"
	rm /tmp/sealed /tmp/secret 2>&-
	exit 1
}

tpm nv_readvalue \
	-in 4d47 \
	-sz 312 \
	-of /tmp/sealed \
|| die "Unable to retrieve sealed file from TPM NV"

tpm unsealfile  \
	-hk 40000000 \
	-if /tmp/sealed \
	-of /tmp/secret \
|| die "Unable to unseal totp secret"

rm /tmp/sealed

totp < /tmp/secret \
|| die "Unable to compute TOTP hash"

rm /tmp/secret
