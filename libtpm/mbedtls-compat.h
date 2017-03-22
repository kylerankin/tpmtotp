/** compatability layer with the mbedtls library */
#ifndef _mbedtls_compat_h_
#define _mbedtls_compat_h_

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//--------------------------------------------------

#include <mbedtls/sha1.h>
#define SHA_CTX mbedtls_sha1_context

static inline void
SHA1_Init(SHA_CTX * sha)
{
	mbedtls_sha1_init(sha);
	mbedtls_sha1_starts(sha);
}
	
#define SHA1_Update(ctx,buf,len) mbedtls_sha1_update(ctx,(const void*) buf, len)
#define SHA1_Final(buf,ctx) mbedtls_sha1_finish(ctx,buf)

#define EVP_sha1() "SHA1"

//--------------------------------------------------

#include <mbedtls/md.h>
#define HMAC_CTX mbedtls_md_context_t

static inline int
HMAC_Init(HMAC_CTX *ctx, const void * key, int key_len, const char *hash_name)
{
	const mbedtls_md_info_t * md_info
		= mbedtls_md_info_from_string(hash_name);

	mbedtls_md_init(ctx);
	mbedtls_md_setup(ctx, md_info, 1);
	mbedtls_md_hmac_starts(ctx, key, key_len);

	return 1;
}


#define HMAC_Update mbedtls_md_hmac_update
#define HMAC_Final(ctx,buf,lenptr) (mbedtls_md_hmac_finish(ctx,buf), *lenptr=20)
#define HMAC_cleanup mbedtls_md_free


/**
 * Read bytes from /dev/urandom, might be slow.
 */
static inline int
RAND_bytes(void * buf_ptr, size_t len)
{
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return 0;

	uint8_t * buf = buf_ptr;
	size_t offset = 0;

	while(offset < len)
	{
		ssize_t rc = read(fd, buf + offset, len - offset);
		if (rc <= 0)
		{
			close(fd);
			return 0;
		}
		offset += rc;
	}

	close(fd);
	return 1;
}

//--------------------------------------------------

#include <mbedtls/aes.h>

#define AES_BLOCK_SIZE 16
#define AES_KEY mbedtls_aes_context
#define AES_DECRYPT MBEDTLS_AES_DECRYPT
#define AES_ENCRYPT MBEDTLS_AES_ENCRYPT

#define AES_set_encrypt_key(key,len,ctx) mbedtls_aes_setkey_enc(ctx,key,len)
#define AES_set_decrypt_key(key,len,ctx) mbedtls_aes_setkey_dec(ctx,key,len)

#define AES_cbc_encrypt(in, out, len, key, ivec, dir) \
	mbedtls_aes_crypt_cbc(key, dir, len, ivec, in, out)

#define AES_encrypt mbedtls_aes_encrypt

//--------------------------------------------------

#include <mbedtls/rsa.h>

#define RSA mbedtls_rsa_context
#define NID_sha1 MBEDTLS_MD_SHA1
//#define RSA_PKCS1_PADDING	MBEDTLS_RSA_PKCS_V21
#define RSA_NO_PADDING          0

#define RSA_free mbedtls_rsa_free

static inline RSA *
RSA_new(void)
{
	RSA * rsa = calloc(1, sizeof(*rsa));
	mbedtls_rsa_init(rsa, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA1);
	return rsa;
}

static inline int
RSA_verify(int type, const unsigned char *m, unsigned int m_len,
           unsigned char *sigbuf, unsigned int siglen, RSA *rsa)
{
	(void) siglen;

	return mbedtls_rsa_pkcs1_verify(
		rsa,
		NULL, // not needed for public
		NULL, // not needed for public
		MBEDTLS_RSA_PUBLIC,
		type,
		m_len,
		m,
		sigbuf);
}


static inline int
RSA_public_encrypt(int flen, unsigned char *from,
           unsigned char *to, RSA *rsa, int padding)
{
	// XXX use flen and padding to fill this out.
	(void) flen;
	(void) padding;

	return mbedtls_rsa_public(
		rsa,
		from,
		to
	);
}


static inline size_t
RSA_size(RSA *rsa)
{
	return rsa->len;
}


//--------------------------------------------------

#include <mbedtls/bignum.h>
#define BIGNUM mbedtls_mpi

static inline BIGNUM * BN_new()
{
	BIGNUM * n = calloc(1, sizeof(*n));
	mbedtls_mpi_init(n);
	return n;
}

static inline void BN_free(BIGNUM * n)
{
	free(n);
}

#define BN_bin2bn(ptr,len,bn) mbedtls_mpi_read_binary(bn, ptr, len)


//--------------------------------------------------

#include <mbedtls/x509_crt.h>
typedef mbedtls_x509_crt X509;

static inline X509 * d2i_X509(X509 **x509_ptr, const unsigned char **in, int len)
{
	X509 * const x509 = x509_ptr ? *x509_ptr : calloc(1, sizeof(*x509));
	int rc = mbedtls_x509_crt_parse_der(x509, *in, len);
	if (rc != 0)
		return NULL;
	if (x509_ptr)
		*x509_ptr = x509;
	return x509;
}

#define X509_free free


#endif // _mbedtls-compat_h_
