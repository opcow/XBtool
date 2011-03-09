/* XBFLASH - Xbox Flash image utility
 * Copyright (C) 2002  Paul Bartholomew
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * 
 * xbpack.c - Functions for packing-up a new ROM image
 * 
 */
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xbconfig.h"
#include "xbpack.h"
#include "xbflashlib.h"
#include "cabio.h"
#include "fbl_hash.h"
#include "tea_hash.h"
#include "cryptlib/hash/sha.h"
#include "cryptlib/crypt/rc4.h"

// Local functions

static int	load_image(const char *fname, unsigned char **p_outbuf, unsigned int *p_size);

XBFLASHLIB_API int
do_xbpack(const char *indirname, const char *romfname, unsigned int romsize)
{
	char	indir_prefix[BUFSIZ+1];
	char	fname_cabtmp[BUFSIZ+1];
	char	fname_2bl[BUFSIZ+1];
	char	fname_fbl[BUFSIZ+1];
	char	fname_remainder[BUFSIZ+1];
	char	fname_kernel[BUFSIZ+1];
	unsigned char	*p_kernel_image;
	unsigned int	size_kernel_image;
	unsigned char	*p_2bl_image = 0;
	unsigned int	size_2bl_image;
	unsigned char	*p_fbl_image;
	unsigned int	size_fbl_image;
	unsigned char	*p_remainder_image;
	unsigned int	size_remainder_image;
	unsigned char	*p_kernelZ_image;
	unsigned int	size_kernelZ_image;
	SHA_CTX	sha_ctx;
	unsigned char	sha_digest[SHA_DIGEST_LENGTH];
	unsigned int	kernel_dataseg_size;
	unsigned int	kernel_dataseg_ramaddr;
	unsigned int	kernel_dataseg_romaddr;
	unsigned int	kernel_cmpimage_romaddr;
	RC4_KEY	rc4_key;
	FILE	*fh;
	TEA_HASH	tea_hash;
	int err = 0;

	strcpy(indir_prefix, indirname);
	strcat(indir_prefix, PATH_SEP);

	strcpy(fname_cabtmp, indir_prefix);
	strcat(fname_cabtmp, IMGFNAME_CABTMP);
	strcpy(fname_2bl, indir_prefix);
	strcat(fname_2bl, IMGFNAME_2BL);
	strcpy(fname_fbl, indir_prefix);
	strcat(fname_fbl, IMGFNAME_FBL);
	strcpy(fname_kernel, indir_prefix);
	strcat(fname_kernel, IMGFNAME_XBOXKRNL);
	strcpy(fname_remainder, indir_prefix);
	strcat(fname_remainder, IMGFNAME_REMAINDER);

	if (logFile) fprintf(logFile, "Loading KERNEL image file '%s'...", fname_kernel);
	if (!load_image(fname_kernel, &p_kernel_image, &size_kernel_image)) {
		return 0;
	}

	if (logFile) fprintf(logFile, "\nLoading 2BL image file '%s'...", fname_2bl);
	if (!load_image(fname_2bl, &p_2bl_image, &size_2bl_image)) {
		free(p_kernel_image);
		return 0;
	}
	if (CFG_fbl_address && CFG_fbl_size) {
		if (logFile) fprintf(logFile, "\nLoading FBL image file '%s'...", fname_fbl);
		if (!load_image(fname_fbl, &p_fbl_image, &size_fbl_image)) {
			free(p_2bl_image);
			free(p_kernel_image);
			return 0;
		}
	} else {
		p_fbl_image = 0;
	}
	if (logFile) fprintf(logFile, "\nLoading REMAINDER image file '%s'...", fname_remainder);
	if (!load_image(fname_remainder, &p_remainder_image, &size_remainder_image)) {
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernel_image);
		return 0;
	}

	// Verify ROM file size is non-zero and a power of 2
	if (!size_remainder_image || (size_remainder_image & (size_remainder_image-1))) {
		fprintf(stderr, "Invalid filesize for remainder file: %u\n", 
							size_remainder_image);
		free(p_remainder_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernel_image);
		return 0;
	}



	// FIXME: Do range checking here!

	kernel_dataseg_size = *(unsigned int *)(p_kernel_image + CFG_kernel_dwdatasize_offset);
	kernel_dataseg_ramaddr = *(unsigned int *)(p_kernel_image + CFG_kernel_dwdatarambase_offset);

	// Copy initialized kernel data segment to just below 0xFFFF9E00
	//
	kernel_dataseg_romaddr = ((CFG_kernel_top_address - kernel_dataseg_size) + 1);
	memcpy((p_remainder_image + (kernel_dataseg_romaddr & (size_remainder_image-1))),
				(p_kernel_image + (kernel_dataseg_ramaddr - CFG_kernel_address_adj)),
				kernel_dataseg_size);

	// Put address of ROM initialized data for kernel into kernel header

	*(unsigned int *)(p_kernel_image + CFG_kernel_dwdatarombase_offset) = kernel_dataseg_romaddr;


	// Compress the kernel image

	if (logFile) fprintf(logFile, "\nCompressing KERNEL image...");

	// In case temp .cab exists, delete it first
	unlink(fname_cabtmp);

	if (!create_kernel_cab_from_uncompressed(IMGFNAME_CABTMP, indir_prefix, IMGFNAME_XBOXKRNL, (unsigned char *) fname_kernel)) {
		free(p_remainder_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernel_image);
		return 0;
	}

	if (!extract_compressed_from_kernel_cab(fname_cabtmp, &p_kernelZ_image, &size_kernelZ_image)) {
		unlink(fname_cabtmp);
		free(p_remainder_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernel_image);
		return 0;
	}

	// We no longer need the .tmp CAB file, so remove it

	unlink(fname_cabtmp);

	if (logFile) fprintf(logFile, "\n\t(Compressed from %u bytes to %u bytes)", size_kernel_image, size_kernelZ_image);

	if (logFile) fprintf(logFile, "\nEncrypting KERNEL image...");

	// Now encrypt compressed kernel image RC4 key from 2bl
	//
	if (CFG_RC4_key_encrypt)
	{
		RC4_set_key(&rc4_key, RC4_KEYLEN, p_2bl_image+CFG_2bl_kernelkey_offset);
		RC4(&rc4_key, size_kernelZ_image, p_kernelZ_image, p_kernelZ_image);
	}

	// Copy compressed/encrypted kernel image to just below initialized kernel data segment in ROM
	//
	kernel_cmpimage_romaddr = ((CFG_kernel_top_address - kernel_dataseg_size - size_kernelZ_image) + 1);
	memcpy((p_remainder_image + (kernel_cmpimage_romaddr & (size_remainder_image-1))),
				p_kernelZ_image, size_kernelZ_image);

	// Poke 2bl with kernel init data size, kernel compressed image size
	//
	*(unsigned int *)(p_2bl_image + CFG_2bl_dwkerneldatasize_offset) = kernel_dataseg_size;
	*(unsigned int *)(p_2bl_image + CFG_2bl_dwkernelsize_offset) = size_kernelZ_image;


	if (logFile) fprintf(logFile, "\nGenerating SHA-1 hash...");

	// Compute SHA-1 hash
	//

	SHA1_Init(&sha_ctx);

	// start with rc4 key
	SHA1_Update(&sha_ctx, p_2bl_image+CFG_2bl_kernelkey_offset, RC4_KEYLEN);

	// include size of kernelZ image, then kernelZ image
	SHA1_Update(&sha_ctx, (unsigned char *) &size_kernelZ_image, 4);
	SHA1_Update(&sha_ctx, p_kernelZ_image, size_kernelZ_image);

	// include size of kernel initialized data image, then kernel data image
	SHA1_Update(&sha_ctx, (unsigned char *) &kernel_dataseg_size, 4);
	SHA1_Update(&sha_ctx, 
		(p_remainder_image + (kernel_dataseg_romaddr & (size_remainder_image-1))),
				kernel_dataseg_size);

	// include size of ROM-header/xcode image, then the image
	SHA1_Update(&sha_ctx, p_2bl_image+CFG_2bl_dwflashstart_hashsize, 4);
	SHA1_Update(&sha_ctx, p_remainder_image+0, 
		*(unsigned int *)(p_2bl_image+CFG_2bl_dwflashstart_hashsize));

	SHA1_Final(sha_digest, &sha_ctx);

	// re-init
	SHA1_Init(&sha_ctx);

	// start with rc4 key
	SHA1_Update(&sha_ctx, p_2bl_image+CFG_2bl_kernelkey_offset, RC4_KEYLEN);

	// include digest computed above
	SHA1_Update(&sha_ctx, sha_digest, sizeof(sha_digest));
		
	SHA1_Final(sha_digest, &sha_ctx);

	// Now copy the SHA-1 digest into the 2bl
	memcpy(p_2bl_image+CFG_2bl_sha_hash_offset, sha_digest, sizeof(sha_digest));

	if (logFile) fprintf(logFile, "\nEncrypting 2BL image...");

	// Now encrypt 2bl with main RC4 key
	if (CFG_RC4_keymethod == 1) {
		SHA_CTX	sha_ctx;
		unsigned char	sha_digest[SHA_DIGEST_LENGTH];
		unsigned char	tmp_key[16];
		int	i;

		SHA1_Init(&sha_ctx);

		// start with rc4 key
		SHA1_Update(&sha_ctx, CFG_RC4_key, 16);

		SHA1_Update(&sha_ctx, &p_2bl_image[size_2bl_image-16], 16);

		for (i = 0; i < 16; i++) {
			tmp_key[i] = CFG_RC4_key[i] ^ 0x5c;
		}

		SHA1_Update(&sha_ctx, tmp_key, 16);
		SHA1_Final(sha_digest, &sha_ctx);

		RC4_set_key(&rc4_key, 20, sha_digest);
		RC4(&rc4_key, size_2bl_image-16, p_2bl_image, p_2bl_image);

	} else if (CFG_RC4_key_encrypt == 2)
	{
		RC4_set_key(&rc4_key, RC4_keylen, CFG_RC4_key);
		RC4(&rc4_key, size_2bl_image, p_2bl_image, p_2bl_image);
	}

	// Now copy 2bl back into the overall image
	memcpy((p_remainder_image + (CFG_2bl_address & (size_remainder_image-1))),
				p_2bl_image, size_2bl_image);

	if (p_fbl_image && size_fbl_image) {
		// Copy the FBL back on top of the 2BL in the overall image

		memcpy((p_remainder_image + (CFG_fbl_address & (size_remainder_image-1))),
				p_fbl_image, size_fbl_image);
	}



	if ((CFG_RC4_keymethod == 1) && (CFG_fbl_address && CFG_fbl_size)) {
		if (logFile) fprintf(logFile, "\nGenerating FBL signature...");

		gen_fbl_signature(p_remainder_image, size_remainder_image);
	}

	if ((CFG_fbl_address && CFG_fbl_size)) {
		if (logFile) fprintf(logFile, "\nVerifying FBL's TEA hash...");

		calc_TEA_hash(&p_remainder_image[(0xffffd400 & (size_remainder_image-1))], (0xfffffc80-0xffffd400), FBL_TEA_HASH_IVAL1, FBL_TEA_HASH_IVAL2, &tea_hash);
		if (is_expected_FBL_TEA_hash(&tea_hash)) {
			if (logFile) fprintf(logFile, "OK!\n");
		} else {
			if (logFile) fprintf(logFile, "** TEA hash check failure! **\n");
		}
	}

	// If output ROM image exists, remove it
//	unlink(romfname);

	if (logFile) fprintf(logFile, "\nWriting ROM image file '%s'...", romfname);
	// Write the output ROM image
	if (!(fh = fopen(romfname, "wb"))) {
		if (logFile) fprintf(logFile, "Error creating output ROM image: '%s'!\n", romfname);

		free(p_remainder_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernel_image);
		free(p_kernelZ_image);
		return 0;
	}
	unsigned int minRomSize = size_remainder_image;
	if (minRomSize > 0x40000)
	{
		for (int i = minRomSize / 2; i >= 0x80000; i /= 2)
		{
			if (memcmp(p_remainder_image + (size_remainder_image - i), p_remainder_image, 0x10) == 0)
				minRomSize = i;
		}
		if (romsize < minRomSize)
		{
			romsize = minRomSize;
			if (logFile)
				fprintf(logFile, "Warning: output ROM image smaller than native remainder.img size.\n");
			if (logFile)
				fprintf(logFile, "Setting ROM size to %dk.\n", romsize / 1024);
		}
	}
	// save to selected size
	if (romsize >= size_remainder_image) // just mirror it in this case
	{
		for (int i = romsize / size_remainder_image; i > 0; i--)
		{
			if (fwrite(p_remainder_image, 1, size_remainder_image, fh) != size_remainder_image)
			{
				if (logFile)
					fprintf(logFile, "Error writing output ROM image: '%s'!\n", romfname);
				err = 1;
			}
		}
	}
	else
	{
		if (fwrite(p_remainder_image + (size_remainder_image - romsize), 1, romsize, fh) != romsize)
		{
			if (logFile)
				fprintf(logFile, "Error writing output ROM image: '%s'!\n", romfname);
			err = 1;
		}
	}

	free(p_remainder_image);
	free(p_2bl_image);
	free(p_kernel_image);
	free(p_kernelZ_image);
	fclose(fh);

	if (err)
	{
		unlink(romfname);
		return !err;
	}
	if (logFile)
		fprintf(logFile, "\nSuccess!\n\n");
	return !err;

	/*
	if (fwrite(p_remainder_image, 1, size_remainder_image, fh) != size_remainder_image) {
		fprintf(stderr, "Error writing output ROM image: '%s'!\n", romfname);

		fclose(fh);
		unlink(romfname);
		free(p_remainder_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernel_image);
		free(p_kernelZ_image);
		return 0;
	}
	fclose(fh);

	if (logFile) fprintf(logFile, "\nSuccess!\n");

	return 1;
*/
}

static int
load_image(const char *fname, unsigned char **p_outbuf, unsigned int *p_size)
{
	FILE	*fh;
	unsigned char	*buf;
	unsigned int	filesize;

	if (!(fh = fopen(fname, "rb"))) {
		fprintf(stderr, "Cannot open input file: '%s'!\n", fname);
		return 0;
	}

	fseek(fh, 0L, SEEK_END);
	filesize = ftell(fh);

	if (!(buf = (unsigned char *) malloc(filesize))) {
		fprintf(stderr, "Not enough memory to load image: '%s'!\n", fname);
		fclose(fh);
		return 0;
	}

	fseek(fh, 0L, SEEK_SET);
	if ((fread(buf, 1, filesize, fh) != filesize)) {
		fprintf(stderr, "Error reading image file: '%s'!\n", fname);
		free(buf);
		fclose(fh);
		return 0;
	}

	fclose(fh);

	*p_outbuf = buf;
	*p_size = filesize;
	return 1;
}

