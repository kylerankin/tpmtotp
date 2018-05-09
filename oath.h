#ifndef _oath_h_
#define _oath_h_

#include <stdint.h>


extern uint32_t
oauth_calc(
	uint32_t now,
	const uint8_t * secret,
	size_t secret_len
);
extern uint32_t
hotp_calc(
	uint32_t counter,
	const uint8_t * secret,
	size_t secret_len
);


#endif
