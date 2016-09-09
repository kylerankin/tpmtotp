/*
 * Translate stdin into base32
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include "base32.h"


int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	const size_t max_len = 65536;
	unsigned char blob[max_len];

	size_t offset = 0;
	while(offset < sizeof(blob))
	{
		ssize_t rc = read(0, blob+offset, sizeof(blob) - offset);
		if (rc < 0)
		{
			perror("stdin");
			return -1;
		}
		if (rc == 0)
			break;

		offset += rc;
	}

	if (offset == sizeof(blob))
	{
		fprintf(stderr, "Truncated data? max len %zu\n", sizeof(blob));
		return -1;
	}


	size_t base32_len = BASE32_LEN(offset);
	unsigned char output[base32_len+1];
	base32_encode(blob, offset, output);

	output[base32_len] = '\0';
	printf("%s", output);

	return 0;
}
