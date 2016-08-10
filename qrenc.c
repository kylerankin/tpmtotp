/*
 * qrenc - Output an ASCII QR encode for the input
 *
 * Copyright 2016 Trammell Hudson <trammell.hudson@twosigma.com>
 * Copyright 2015 Matthew Garrett <mjg59@srcf.ucam.org>
 * Copyright 2015 Andreas Fuchs, Fraunhofer SIT
 *
 * Portions derived from sealfile.c by J. Kravitz and Copyright (C) 2004 IBM
 * Corporation
 *
 * Portions derived from qrenc.c Copyright (C) 2006-2012 Kentaro Fukuchi
 * <kentaro@fukuchi.org>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <qrencode.h>

static const int margin = 1;

static void
writeANSI_margin(
	FILE * fp,
	int realwidth,
	char* buffer,
	size_t buffer_s,
	const char* white,
	size_t white_s
)
{
	(void) buffer_s;

	strncpy(buffer, white, white_s);
	memset(buffer + white_s, ' ', realwidth * 2);
	strcpy(buffer + white_s + realwidth * 2, "\033[0m\n"); // reset to default colors
	for(int y=0; y<margin; y++ ){
		fputs(buffer, fp);
	}
}


static int
writeANSI(
	const QRcode * const qrcode,
	FILE * const fp
)
{
	const char white[] = "\033[47m";
	const size_t white_s = sizeof(white)-1;
	const char black[] = "\033[40m";
	const char black_s = sizeof(black)-1;

	const int size = 1;

	const size_t realwidth = (qrcode->width + margin * 2) * size;
	const size_t buffer_s = ( realwidth * white_s ) * 2;
	char * const buffer = malloc( buffer_s );

	if(buffer == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		exit(1);
	}

	/* top margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);

	/* raw data */
	const unsigned char * const p = qrcode->data;

	for(int y=0; y<qrcode->width; y++) {
		const unsigned char * const row = p + y*qrcode->width;

		memset( buffer, 0, buffer_s );
		strncpy( buffer, white, white_s );
		for(int x=0; x<margin; x++ ){
			strncat( buffer, "  ", 2 );
		}

		int last = 0;

		for(int x=0; x<qrcode->width; x++) {
			if(row[x] & 0x1) {
				if( last != 1 ){
					strncat( buffer, black, black_s );
					last = 1;
				}
			} else {
				if( last != 0 ){
					strncat( buffer, white, white_s );
					last = 0;
				}
			}
			strncat( buffer, "  ", 2 );
		}

		if( last != 0 ){
			strncat( buffer, white, white_s );
		}
		for(int x=0; x<margin; x++ ){
			strncat( buffer, "  ", 2 );
		}
		strncat( buffer, "\033[0m\n", 5 );
		fputs( buffer, fp );
	}

	/* bottom margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);

	free(buffer);

	return 0;
}


int main(int argc, char *argv[])
{
	const char * qr_string = "";
	if (argc > 1)
		qr_string = argv[1];

	QRcode * qrcode = QRcode_encodeString(
		qr_string,
		0,
		QR_ECLEVEL_L,
		QR_MODE_8,
		1
	);

	writeANSI(qrcode, stdout);
	return 0;
}
