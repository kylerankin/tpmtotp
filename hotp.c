/*
 * Read a secret from stdin and generate the HOTP hash based on the time.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include "oath.h"
#include <stdlib.h>


int main(int argc, char *argv[])
{
	if (argc < 2)
  {
		fprintf(stderr, "Usage: %s <counter>\n", argv[0]);
    return -1;
  }

	const size_t keylen = 20;
	unsigned char key[keylen];
  uint32_t increment = atoi(argv[1]);

	// this will fail on partial reads, unlikely
	ssize_t rc = read(0, key, sizeof(key));

	if (rc < 0)
	{
		perror("stdin");
		return -1;
	}

	if (rc != (ssize_t) keylen)
	{
		fprintf(stderr, "Expected %zu bytes, read %zu\n",
			keylen,
			rc
		);
		return -1;
	}

	uint32_t token = hotp_calc(increment, key, keylen);

  printf("%06d\n", token);

	return 0;
}
