/** \file
 * Simple TOTP authentication library.
 *
 * cloned from bitbucket.org/hudosn/pebble/auth/src/totp.c
 */
#include <string.h>
#include "oath.h"

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

typedef union
{
  uint8_t b[BLOCK_LENGTH];
  uint32_t w[BLOCK_LENGTH/4];
} _buffer;
typedef union {
  uint8_t b[HASH_LENGTH];
  uint32_t w[HASH_LENGTH/4];
} _state;

static _buffer buffer;
static uint8_t bufferOffset;
static _state state;
static uint32_t byteCount;
static uint8_t keyBuffer[BLOCK_LENGTH];
static uint8_t innerHash[HASH_LENGTH];
    
#define SHA1_K0 0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

static const uint8_t sha1InitState[] = {
  0x01,0x23,0x45,0x67, // H0
  0x89,0xab,0xcd,0xef, // H1
  0xfe,0xdc,0xba,0x98, // H2
  0x76,0x54,0x32,0x10, // H3
  0xf0,0xe1,0xd2,0xc3  // H4
};

static void sha1_init(void) {
  memcpy(state.b,sha1InitState,HASH_LENGTH);
  byteCount = 0;
  bufferOffset = 0;
}

static uint32_t sha1_rol32(uint32_t number, uint8_t bits) {
  return ((number << bits) | (number >> (32-bits)));
}

static void sha1_hashBlock() {
  uint8_t i;
  uint32_t a,b,c,d,e,t;

  a=state.w[0];
  b=state.w[1];
  c=state.w[2];
  d=state.w[3];
  e=state.w[4];
  for (i=0; i<80; i++) {
    if (i>=16) {
      t = buffer.w[(i+13)&15] ^ buffer.w[(i+8)&15] ^ buffer.w[(i+2)&15] ^ buffer.w[i&15];
      buffer.w[i&15] = sha1_rol32(t,1);
    }
    if (i<20) {
      t = (d ^ (b & (c ^ d))) + SHA1_K0;
    } else if (i<40) {
      t = (b ^ c ^ d) + SHA1_K20;
    } else if (i<60) {
      t = ((b & c) | (d & (b | c))) + SHA1_K40;
    } else {
      t = (b ^ c ^ d) + SHA1_K60;
    }
    t+=sha1_rol32(a,5) + e + buffer.w[i&15];
    e=d;
    d=c;
    c=sha1_rol32(b,30);
    b=a;
    a=t;
  }
  state.w[0] += a;
  state.w[1] += b;
  state.w[2] += c;
  state.w[3] += d;
  state.w[4] += e;
}

void sha1_addUncounted(uint8_t data) {
  buffer.b[bufferOffset ^ 3] = data;
  bufferOffset++;
  if (bufferOffset == BLOCK_LENGTH) {
    sha1_hashBlock();
    bufferOffset = 0;
  }
}

void sha1_write(uint8_t data) {
  ++byteCount;
  sha1_addUncounted(data);
}

void sha1_writebytes(const uint8_t* data, int length) {
 for (int i=0; i<length; i++)
 {
   sha1_write(data[i]);
 }
}

void sha1_pad() {
  // Implement SHA-1 padding (fips180-2 §5.1.1)

  // Pad with 0x80 followed by 0x00 until the end of the block
  sha1_addUncounted(0x80);
  while (bufferOffset != 56) sha1_addUncounted(0x00);

  // Append length in the last 8 bytes
  sha1_addUncounted(0); // We're only using 32 bit lengths
  sha1_addUncounted(0); // But SHA-1 supports 64 bit lengths
  sha1_addUncounted(0); // So zero pad the top bits
  sha1_addUncounted(byteCount >> 29); // Shifting to multiply by 8
  sha1_addUncounted(byteCount >> 21); // as SHA-1 supports bitstreams as well as
  sha1_addUncounted(byteCount >> 13); // byte.
  sha1_addUncounted(byteCount >> 5);
  sha1_addUncounted(byteCount << 3);
}


uint8_t* sha1_result(void) {
  // Pad to complete the last block
  sha1_pad();
  
  // Swap byte order back
  for (int i=0; i<5; i++) {
    uint32_t a,b;
    a=state.w[i];
    b=a<<24;
    b|=(a<<8) & 0x00ff0000;
    b|=(a>>8) & 0x0000ff00;
    b|=a>>24;
    state.w[i]=b;
  }
  
  // Return pointer to hash (20 characters)
  return state.b;
}

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c

void sha1_initHmac(const uint8_t* key, size_t keyLength) {
  uint8_t i;
  memset(keyBuffer,0,BLOCK_LENGTH);
  if (keyLength > BLOCK_LENGTH) {
    // Hash long keys
    sha1_init();
    for (;keyLength--;) sha1_write(*key++);
    memcpy(keyBuffer,sha1_result(),HASH_LENGTH);
  } else {
    // Block length keys are used as is
    memcpy(keyBuffer,key,keyLength);
  }
  // Start inner hash
  sha1_init();
  for (i=0; i<BLOCK_LENGTH; i++) {
    sha1_write(keyBuffer[i] ^ HMAC_IPAD);
  }
}

uint8_t* sha1_resultHmac(void) {
  uint8_t i;
  // Complete inner hash
  memcpy(innerHash,sha1_result(),HASH_LENGTH);
  // Calculate outer hash
  sha1_init();
  for (i=0; i<BLOCK_LENGTH; i++) sha1_write(keyBuffer[i] ^ HMAC_OPAD);
  for (i=0; i<HASH_LENGTH; i++) sha1_write(innerHash[i]);
  return sha1_result();
}


/** C function to do oauth computation */
uint32_t
oauth_calc(
	uint32_t unix_epoch,
	const uint8_t * secret,
	size_t secret_len
)
{
	const uint32_t now = unix_epoch / 30;

	uint8_t byteArray[] = {
		0,
		0,
		0,
		0,
		now >> 24,
		now >> 16,
		now >>  8,
		now >>  0,
	};

	sha1_initHmac(secret, secret_len);
	sha1_writebytes(byteArray, 8);
	const uint8_t * const hash = sha1_resultHmac();
  
	const unsigned offset = hash[20 - 1] & 0xF; 
	uint32_t truncatedHash = 0;
	for (int j = 0; j < 4; ++j) {
		truncatedHash <<= 8;
		truncatedHash  |= hash[offset + j];
	}
    
	truncatedHash &= 0x7FFFFFFF;
	truncatedHash %= 1000000;

	return truncatedHash;
}
