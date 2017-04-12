/*
 * Read a secret from stdin and generate the TOTP hash based on the time.
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


int main(int argc, char *argv[])
{
	int show_date = 1;
	if (argc > 1 && strcmp(argv[1], "-q") == 0)
		show_date = 0;

	const size_t keylen = 20;
	unsigned char key[keylen];

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

	time_t now = time(NULL);
	char time_str[128];
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
	uint32_t token = oauth_calc(now, key, keylen);

	if (show_date)
		printf("%s: %06d\n", time_str, token);
	else
		printf("%06d\n", token);

	return 0;
}
