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
#include <wchar.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <qrencode.h>

static int unicode_output = 0;
static const int margin = 1;

static const char * utf8(int cp)
{
	static unsigned char buf[4];

	buf[0] = 0xE0 | ((cp >> 12) & 0x0F);
	buf[1] = 0x80 | ((cp >>  6) & 0x3F);
	buf[2] = 0x80 | ((cp >>  0) & 0x3F);
	buf[3] = '\0';

	return (const char*) buf;
}


static const char * block(int cp)
{
	if (unicode_output)
	{
		if (cp == 0)
			return utf8(0x2588);
		if (cp == 1)
			return utf8(0x2580);
		if (cp == 2)
			return utf8(0x2584);
		if (cp == 3)
			return " ";
	} else {
		// code page whatever
		if (cp == 0) return "\xDB";
		if (cp == 1) return "\xDC";
		if (cp == 2) return "\xDF";
		if (cp == 3) return " ";
	}

	return "--?";
}


static int
writeANSI(
	const QRcode * const qrcode,
	FILE * const fp
)
{
	/* raw data */
	const unsigned char * const p = qrcode->data;
	
	for(int y=0 ; y < margin ; y++)
	{
		for(int x=0; x<qrcode->width + 4 * margin; x++)
			fputs(block(0), fp);
		fputs("\n", fp);
	}

	for(int y=0; y < qrcode->width; y += 2)
	{
		const unsigned char * const row0 = p + (y+0)*qrcode->width;
		const unsigned char * const row1 = p + (y+1)*qrcode->width;

		for(int x=0; x < margin*2; x++ )
			fputs(block(0), fp);

		for(int x=0; x < qrcode->width; x++)
		{
			int r0 = row0[x] & 0x1;
			int r1 = y < qrcode->width-1 ? row1[x] & 0x1 : 0;

			fputs(block(r0 << 1 | r1 << 0), fp);
		}

		for(int x=0; x<margin*2; x++ )
			fputs(block(0), fp);

		fputs("\n", fp);
	}

	/* bottom margin; only do a half block on the last one */
	for(int y=0 ; y < margin ; y++)
	{
		for(int x=0; x<qrcode->width + 4 * margin; x++)
			fputs(block(1), fp);
		fputs("\n", fp);
	}

	return 0;
}


int main(int argc, char *argv[])
{
	const char * qr_string = "";
	if (argc > 1)
		qr_string = argv[1];

	if (strlen(qr_string) == 0)
	{
		fprintf(stderr, "%s: empty strings are not valid\n", argv[0]);
		return -1;
	}

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
