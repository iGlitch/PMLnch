/*-
* Copyright 2003-2005 Colin Percival
* All rights reserved
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted providing that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <gctypes.h>

#include "bspatch.h"

static int64_t offtin(u_char *buf)
{
	int64_t y;

	y = buf[7] & 0x7F;
	y = y * 256; y += buf[6];
	y = y * 256; y += buf[5];
	y = y * 256; y += buf[4];
	y = y * 256; y += buf[3];
	y = y * 256; y += buf[2];
	y = y * 256; y += buf[1];
	y = y * 256; y += buf[0];

	if (buf[7] & 0x80) y = -y;

	return (int64_t)y;
}

int getbsPatchOutputSize(u8 * patchBuffer)
{
	if (memcmp(patchBuffer, "BSDIFF40", 8) != 0)
		return -1;
	return (int)offtin(patchBuffer + 24);
}

int bspatch(u8 * old, int oldsize, u8 * patchBuffer, int patchsize, u8 * _new, int innewsize)
{
	//FILE * f, *cpf, *dpf, *epf;
	//int cbz2err, dbz2err, ebz2err;
	//int fd;
	int64_t newsize;// , newsize;
	int64_t bzctrllen, bzdatalen;
	u_char header[32], buf[8];
	//u_char *old;// , *new;
	int oldpos, newpos;
	int64_t ctrl[3];
	int64_t lenread;
	int64_t i;

	//if (argc != 4) errx(1, "usage: %s oldfile newfile patchfile\n", argv[0]);

	/* Open patch file */
	//if ((f = fopen(argv[3], "r")) == NULL) err(1, "fopen(%s)", argv[3]);

	/*
	File format:
	0	8	"BSDIFF40"
	8	8	X
	16	8	Y
	24	8	sizeof(newfile)
	32	X	bzip2(control block)
	32+X	Y	bzip2(diff block)
	32+X+Y	???	bzip2(extra block)
	with control block a set of triples (x,y,z) meaning "add x bytes
	from oldfile to x bytes from the diff block; copy y bytes from the
	extra block; seek forwards in oldfile by z bytes".
	*/

	/* Read header */
	off_t fOffset = 0;
	memcpy(header, patchBuffer, 1 * 32);
	fOffset += 32;
	//if (fread(header, 1, 32, f) < 32) {
	//if (feof(f)) errx(1, "Corrupt patch\n"); err(1, "fread(%s)", argv[3]);	}

	/* Check for appropriate magic */
	if (memcmp(header, "BSDIFF40", 8) != 0)
		return 1; //errx(1, "Corrupt patch\n");

	/* Read lengths from header */
	bzctrllen = offtin(header + 8); //24
	bzdatalen = offtin(header + 16); //226
	newsize = offtin(header + 24); //3584
	//outnewsize = (int)newsize;
	if ((bzctrllen < 0) || (bzdatalen < 0) || (newsize < 0))
		return 2;// errx(1, "Corrupt patch\n");
	int64_t cpfbz2Offset = 32;
	int64_t dpfbz2Offset = 32 + bzctrllen; //56
	int64_t epfbz2Offset = 32 + bzctrllen + bzdatalen; //282

	/* Close patch file and re-open it via libbzip2 at the right places */
	//if (fclose(f)) err(1, "fclose(%s)", argv[3]);
	//if ((cpf = fopen(argv[3], "r")) == NULL) err(1, "fopen(%s)", argv[3]);
	//if (fseeko(cpf, 32, SEEK_SET)) err(1, "fseeko(%s, %lld)", argv[3], (long long)32);

	//if ((cpfbz2 = BZ2_bzReadOpen(&cbz2err, cpf, 0, 0, NULL, 0)) == NULL) errx(1, "BZ2_bzReadOpen, bz2err = %d", cbz2err);
	//if ((dpf = fopen(argv[3], "r")) == NULL) err(1, "fopen(%s)", argv[3]);
	//if (fseeko(dpf, 32 + bzctrllen, SEEK_SET)) err(1, "fseeko(%s, %lld)", argv[3], (long long)(32 + bzctrllen));

	//if ((dpfbz2 = BZ2_bzReadOpen(&dbz2err, dpf, 0, 0, NULL, 0)) == NULL) errx(1, "BZ2_bzReadOpen, bz2err = %d", dbz2err);
	//if ((epf = fopen(argv[3], "r")) == NULL) err(1, "fopen(%s)", argv[3]);
	//if (fseeko(epf, 32 + bzctrllen + bzdatalen, SEEK_SET)) err(1, "fseeko(%s, %lld)", argv[3], (long long)(32 + bzctrllen + bzdatalen));

	//if ((epfbz2 = BZ2_bzReadOpen(&ebz2err, epf, 0, 0, NULL, 0)) == NULL) errx(1, "BZ2_bzReadOpen, bz2err = %d", ebz2err);
	/*if (((fd = open(argv[1], O_RDONLY, 0))<0) ||
		((oldsize = lseek(fd, 0, SEEK_END)) == -1) ||
		((old = malloc(oldsize + 1)) == NULL) ||
		(lseek(fd, 0, SEEK_SET) != 0) ||
		(read(fd, old, oldsize) != oldsize) ||
		(close(fd) == -1)) err(1, "%s", argv[1]);*/
	//memset(*_new, 0, newsize);
	oldpos = 0; newpos = 0;
	while (newpos<newsize) {

		/* Read control data */
		for (i = 0; i <= 2; i++) {
			lenread = 8;
			if (cpfbz2Offset + lenread > patchsize) lenread = patchsize - cpfbz2Offset;
			memcpy(buf, patchBuffer + cpfbz2Offset, lenread);
			cpfbz2Offset += lenread;
			/*lenread = BZ2_bzRead(&cbz2err, cpfbz2, buf, 8);
			if ((lenread < 8) || ((cbz2err != BZ_OK) &&
			(cbz2err != BZ_STREAM_END)))
			errx(1, "Corrupt patch\n");*/
			ctrl[i] = offtin(buf);
		};


		/* Sanity-check */
		if (newpos + ctrl[0] > newsize) return 4;// errx(1, "Corrupt patch\n");

		/* Read diff string */
		lenread = ctrl[0];
		if (dpfbz2Offset + lenread > patchsize) lenread = patchsize - dpfbz2Offset;
		memcpy((_new + newpos), (patchBuffer + dpfbz2Offset), lenread);
		dpfbz2Offset += lenread;
		//lenread = BZ2_bzRead(&dbz2err, dpfbz2, new + newpos, ctrl[0]);
		//if ((lenread < ctrl[0]) || ((dbz2err != BZ_OK) && (dbz2err != BZ_STREAM_END))) errx(1, "Corrupt patch\n");

		/* Add old data to diff string */
		for (i = 0; i < ctrl[0]; i++)
		{
			if ((oldpos + i >= 0) && (oldpos + i < oldsize))
				_new[newpos + i] += old[oldpos + i];
		}
		/* Adjust pointers */
		newpos += ctrl[0];
		oldpos += ctrl[0];

		/* Sanity-check */
		if (newpos + ctrl[1]>newsize)
			return 5;// errx(1, "Corrupt patch\n");

		/* Read extra string */
		lenread = ctrl[1];
		if (epfbz2Offset + lenread > patchsize) lenread = patchsize - epfbz2Offset;
		memcpy(_new + newpos, patchBuffer + epfbz2Offset, lenread);
		epfbz2Offset += lenread;
		//lenread = BZ2_bzRead(&ebz2err, epfbz2, new + newpos, ctrl[1]);
		//if ((lenread < ctrl[1]) || ((ebz2err != BZ_OK) && (ebz2err != BZ_STREAM_END))) errx(1, "Corrupt patch\n");

		/* Adjust pointers */
		newpos += ctrl[1];
		oldpos += ctrl[2];
	};

	/* Clean up the bzip2 reads */
	/*BZ2_bzReadClose(&cbz2err, cpfbz2);
	BZ2_bzReadClose(&dbz2err, dpfbz2);
	BZ2_bzReadClose(&ebz2err, epfbz2);
	if (fclose(cpf) || fclose(dpf) || fclose(epf))
	err(1, "fclose(%s)", argv[3]);

	if (((fd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666)) < 0) ||
	(write(fd, new, newsize) != newsize) || (close(fd) == -1))
	err(1, "%s", argv[2]);

	free(new);
	free(old);*/
	return 0;
}
