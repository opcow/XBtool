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
 * xbunpack.c - Functions for unpacking a ROM image
 * 
 */
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>

#include "xbconfig.h"
#include "xbflashlib.h"

#include "cabio.h"
#include "fbl_hash.h"
#include "tea_hash.h"
#include "cryptlib/hash/sha.h"
#include "cryptlib/crypt/rc4.h"

#include "xbunpack.h"

static int	load_rom_image(const char *romfname, unsigned char **p_rombuf, unsigned int *p_size);
static int	extract_fbl_image(unsigned char *p_rom_image, unsigned int size_rom_image, 
					unsigned char **p_fbl_image, unsigned int *p_size_fbl_image);
static int	extract_2bl_image(unsigned char *p_rom_image, unsigned int size_rom_image, 
					unsigned char **p_2bl_image, unsigned int *p_size_2bl_image);
static int	decrypt_2bl_image(unsigned char *p_2bl_image, unsigned int size_2bl_image, unsigned char *p_fbl_image, unsigned int size_fbl_image);
static int	extract_compressed_kernel_image(unsigned char *p_rom_image, unsigned int size_rom_image,
					unsigned char *p_2bl_image, unsigned int size_2bl_image,
					unsigned char **p_kernelZ_image, unsigned int *p_size_kernelZ_image);
static int	decrypt_compressed_kernel_image(unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image,
						unsigned char *p_2bl_image, unsigned int size_2bl_image);

//
// Reads input ROM file 'romfname'
//
// Creates multiple files inside directory 'outdirname':
//		bootloader2.img
//		xboxkrnl.img
//		remainder.img
//

XBFLASHLIB_API void freeKernelBuffer(unsigned char *kernelBuffer)
{
	free(kernelBuffer);
}

XBFLASHLIB_API int
do_xbunpack(const char *romfname, const char *outdirname)
{
	unsigned char	*p_rom_image;
	unsigned int	size_rom_image;
	unsigned char	*p_fbl_image;
	unsigned int	size_fbl_image;
	unsigned char	*p_2bl_image;
	unsigned int	size_2bl_image;
	unsigned char	*p_kernelZ_image;
	unsigned int	size_kernelZ_image;
	char	outdir_prefix[BUFSIZ+1];
	char	fname_cabtmp[BUFSIZ+1];
	char	fname_2bl[BUFSIZ+1];
	char	fname_fbl[BUFSIZ+1];
	char	fname_remainder[BUFSIZ+1];
	char	fname_kernel[BUFSIZ+1];
	FILE	*fh;
	unsigned int sizeRomOut;

	strcpy(outdir_prefix, outdirname);
	strcat(outdir_prefix, PATH_SEP);

	strcpy(fname_cabtmp, outdir_prefix);
	strcat(fname_cabtmp, IMGFNAME_CABTMP);
	strcpy(fname_2bl, outdir_prefix);
	strcat(fname_2bl, IMGFNAME_2BL);
	strcpy(fname_fbl, outdir_prefix);
	strcat(fname_fbl, IMGFNAME_FBL);
	strcpy(fname_kernel, outdir_prefix);
	strcat(fname_kernel, IMGFNAME_XBOXKRNL);
	strcpy(fname_remainder, outdir_prefix);
	strcat(fname_remainder, IMGFNAME_REMAINDER);

	if (logFile) fprintf(logFile, "Loading ROM image file '%s'... ", romfname);
	if (!load_rom_image(romfname, &p_rom_image, &size_rom_image)) {
		return 0;
	}
	if (CFG_fbl_address && CFG_fbl_size) {
		if (logFile) fprintf(logFile, "\nExtracting FBL image... ");
		if (!extract_fbl_image(p_rom_image, size_rom_image, 
						&p_fbl_image, &size_fbl_image)) {
			free(p_rom_image);
			return 0;
		}
	} else {
		p_fbl_image = 0;
		size_fbl_image = 0;
	}
	if (logFile) fprintf(logFile, "\nExtracting 2BL image... ");
	if (!extract_2bl_image(p_rom_image, size_rom_image, 
					&p_2bl_image, &size_2bl_image)) {
		free(p_rom_image);
		if (p_fbl_image) free(p_fbl_image);
		return 0;
	}
	if (CFG_RC4_key_encrypt == 2)
	{
		if (logFile) fprintf(logFile, "\nDecrypting 2BL image... ");
		if (!decrypt_2bl_image(p_2bl_image, size_2bl_image, p_fbl_image, size_fbl_image)) {
			free(p_rom_image);
			if (p_fbl_image) free(p_fbl_image);
			free(p_2bl_image);
			return 0;
		}
	}

	if (logFile) fprintf(logFile, "\nExtracting compressed KERNEL image... ");
	if (!extract_compressed_kernel_image(p_rom_image, size_rom_image,
					p_2bl_image, size_2bl_image,
					&p_kernelZ_image, &size_kernelZ_image)) {
		free(p_rom_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		return 0;
	}

	if (CFG_RC4_key_encrypt)
	{
		if (logFile) fprintf(logFile, "\nDecrypting compressed KERNEL image... ");
		if (!decrypt_compressed_kernel_image(p_kernelZ_image, size_kernelZ_image,
			p_2bl_image, size_2bl_image)) {
				free(p_rom_image);
				if (p_fbl_image) free(p_fbl_image);
				free(p_2bl_image);
				free(p_kernelZ_image);
				return 0;
			}
	}
	// Now ready to start writing output files, so create target directory

	mkdir(outdirname);

	// If the output file(s) already exist, then remove them first

	unlink(fname_cabtmp);
	unlink(fname_2bl);
	unlink(fname_kernel);
	unlink(fname_remainder);


	if (logFile) fprintf(logFile, "\nDecompressing KERNEL image (compressed size: %u)... ", size_kernelZ_image);
	if (!create_kernel_cab_from_compressed(fname_cabtmp, IMGFNAME_XBOXKRNL, p_kernelZ_image, size_kernelZ_image)) {
		free(p_rom_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		free(p_kernelZ_image);
		rmdir(outdirname);
		return 0;
	}

	// We no longer need compressed kernel image, so free it up
	free(p_kernelZ_image);


	if (logFile) fprintf(logFile, "\nWriting KERNEL image file '%s'... ", fname_kernel);
	if (!extract_decompressed_from_kernel_cab(IMGFNAME_CABTMP, outdir_prefix)) {
		free(p_rom_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		unlink(fname_cabtmp);
		rmdir(outdirname);
		return 0;
	}

	// We no longer need temp cab file, so remove it
	unlink(fname_cabtmp);

	// Write our output files


	if (logFile) fprintf(logFile, "\nWriting 2BL image file '%s'... ", fname_2bl);
	if (!(fh = fopen(fname_2bl, "wb"))) {
		if (logFile) fprintf(logFile, "Cannot write output 2bl file!\n");
		free(p_rom_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		return 0;
	}
	if (fwrite(p_2bl_image, 1, size_2bl_image, fh) != size_2bl_image) {
		if (logFile) fprintf(logFile, "Error writing output 2bl file!\n");
		fclose(fh);
		unlink(fname_2bl);
		free(p_rom_image);
		if (p_fbl_image) free(p_fbl_image);
		free(p_2bl_image);
		return 0;
	}
	fclose(fh);

	// We no longer need 2bl image, so free it up
	free(p_2bl_image);

	if (p_fbl_image) {
		if (logFile) fprintf(logFile, "\nWriting FBL image file '%s'... ", fname_fbl);
		if (!(fh = fopen(fname_fbl, "wb"))) {
			if (logFile) fprintf(logFile, "Cannot write output fbl file!\n");
			free(p_rom_image);
			free(p_fbl_image);
			return 0;
		}
		if (fwrite(p_fbl_image, 1, size_fbl_image, fh) != size_fbl_image) {
			if (logFile) fprintf(logFile, "Error writing output fbl file!\n");
			fclose(fh);
			unlink(fname_fbl);
			free(p_rom_image);
			free(p_fbl_image);
			return 0;
		}
		fclose(fh);

		// We no longer need fbl image, so free it up
		free(p_fbl_image);
	}
	
	// Lets just write out a minimum sized 2bl. This may break something,
	// but for now it might avoid some confusion.
	sizeRomOut = size_rom_image;
	if (sizeRomOut > 0x40000)
	{
		for (int i = sizeRomOut / 2; i >= 0x40000; i /= 2)
		{
			if (memcmp(p_rom_image + (size_rom_image - i), p_rom_image, 0x10) == 0)
				sizeRomOut = i;
		}
	}
	if (logFile) fprintf(logFile, "\nWriting REMAINDER image file '%s'... ", fname_remainder);
	if (!(fh = fopen(fname_remainder, "wb"))) {
		if (logFile) fprintf(logFile, "Cannot write output remainder file!\n");
		free(p_rom_image);
		return 0;
	}
	if (fwrite(p_rom_image + (size_rom_image - sizeRomOut), 1, sizeRomOut, fh) != sizeRomOut)
	{
		if (logFile) fprintf(logFile, "Error writing output remainder file!\n");
		fclose(fh);
		unlink(fname_remainder);
		free(p_rom_image);
		return 0;
	}
	fclose(fh);

	// We no longer need rom image, so free it up
	free(p_rom_image);

	if (logFile) fprintf(logFile, "\nSuccess!\n");

	return 1;
}

static int
load_rom_image(const char *romfname, unsigned char **p_rombuf, unsigned int *p_size)
{
	FILE	*fh;
	unsigned char	*rom_buf;
	unsigned int	filesize;

	if (!(fh = fopen(romfname, "rb"))) {
		if (logFile) fprintf(logFile, "Cannot open input ROM file: '%s'!\n", romfname);
		return 0;
	}

	fseek(fh, 0L, SEEK_END);
	filesize = ftell(fh);

	// Verify ROM file size is non-zero and a power of 2
	if (!filesize || (filesize & (filesize-1))) {
		if (logFile) fprintf(logFile, "Invalid filesize for ROM file: %u\n", filesize);
		fclose(fh);
		return 0;
	}

	if (!(rom_buf = (unsigned char *) malloc(filesize))) {
		if (logFile) fprintf(logFile, "Not enough memory to load ROM!\n");
		fclose(fh);
		return 0;
	}

	fseek(fh, 0L, SEEK_SET);
	if ((fread(rom_buf, 1, filesize, fh) != filesize)) {
		if (logFile) fprintf(logFile, "Error reading ROM file!\n");
		free(rom_buf);
		fclose(fh);
		return 0;
	}

	fclose(fh);

	*p_rombuf = rom_buf;
	*p_size = filesize;
	return 1;
}

static int
extract_2bl_image(unsigned char *p_rom_image, unsigned int size_rom_image, 
					unsigned char **p_2bl_image, unsigned int *p_size_2bl_image)
{
	unsigned char	*p_buf;
	unsigned int	rom_image_offset;

	rom_image_offset = (CFG_2bl_address & (size_rom_image - 1));

	if ((rom_image_offset + CFG_2bl_size) > size_rom_image) {
		if (logFile) fprintf(logFile, "CFG 2bl address/size problem!\n");
		return 0;
	}
	if (!(p_buf = (unsigned char *) malloc(CFG_2bl_size))) {
		if (logFile) fprintf(logFile, "Not enough memory to load 2BL!\n");
		return 0;
	}
	// Copy-out the 2bl image
	memcpy(p_buf, (p_rom_image + rom_image_offset), CFG_2bl_size);

	// Now go back to ROM image and wipe-out the area used by 2bl
	memset((p_rom_image + rom_image_offset), 0, CFG_2bl_size);

	*p_2bl_image = p_buf;
	*p_size_2bl_image = CFG_2bl_size;

	return 1;
}

static int
extract_fbl_image(unsigned char *p_rom_image, unsigned int size_rom_image, 
					unsigned char **p_fbl_image, unsigned int *p_size_fbl_image)
{
	unsigned char	*p_buf;
	unsigned int	rom_image_offset;

	if (!CFG_fbl_address || !CFG_fbl_size) {
		*p_fbl_image = 0;
		*p_size_fbl_image = 0;
		return 1;
	}
	rom_image_offset = (CFG_fbl_address & (size_rom_image - 1));

	if ((rom_image_offset + CFG_fbl_size) > size_rom_image) {
		if (logFile) fprintf(logFile, "CFG fbl address/size problem!\n");
		return 0;
	}
	if (!(p_buf = (unsigned char *) malloc(CFG_fbl_size))) {
		if (logFile) fprintf(logFile, "Not enough memory to load FBL!\n");
		return 0;
	}
	// Copy-out the fbl image
	memcpy(p_buf, (p_rom_image + rom_image_offset), CFG_fbl_size);

	// Now go back to ROM image and wipe-out the area used by fbl
	memset((p_rom_image + rom_image_offset), 0, CFG_fbl_size);

	*p_fbl_image = p_buf;
	*p_size_fbl_image = CFG_fbl_size;

	return 1;
}

static int
decrypt_2bl_image(unsigned char *p_2bl_image, unsigned int size_2bl_image, unsigned char *p_fbl_image, unsigned int size_fbl_image)
{
	SHA_CTX	sha_ctx;
	RC4_KEY	rc4_key;
	unsigned char	sha_digest[SHA_DIGEST_LENGTH];
	unsigned char	tmp_key[16];
	int	i;

	if (CFG_RC4_keymethod == 1) {
		SHA1_Init(&sha_ctx);

		// start with rc4 key
		SHA1_Update(&sha_ctx, CFG_RC4_key, 16);

		if (0 && p_fbl_image && size_fbl_image) {
			SHA1_Update(&sha_ctx, &p_fbl_image[size_fbl_image-16], 16);
		} else {
			SHA1_Update(&sha_ctx, &p_2bl_image[size_2bl_image-16], 16);
		}

		for (i = 0; i < 16; i++) {
			tmp_key[i] = CFG_RC4_key[i] ^ 0x5c;
		}

		SHA1_Update(&sha_ctx, tmp_key, 16);
		SHA1_Final(sha_digest, &sha_ctx);

		RC4_set_key(&rc4_key, 20, sha_digest);
		RC4(&rc4_key, size_2bl_image-16, p_2bl_image, p_2bl_image);
	} else {
		RC4_set_key(&rc4_key, RC4_keylen, CFG_RC4_key);
		RC4(&rc4_key, size_2bl_image, p_2bl_image, p_2bl_image);
	}
	return 1;
}

static int
extract_compressed_kernel_image(unsigned char *p_rom_image, unsigned int size_rom_image,
					unsigned char *p_2bl_image, unsigned int size_2bl_image,
					unsigned char **p_kernelZ_image, unsigned int *p_size_kernelZ_image)
{
	unsigned char	*p_kernelZ;
	unsigned int	kernelZ_size;
	unsigned int	kernel_data_size;
	unsigned int	kernel_top_rom_offset;
	unsigned int	kernel_rom_offset;

	if ( ((CFG_2bl_kernelkey_offset + RC4_KEYLEN) > size_2bl_image) ||
		((CFG_2bl_dwkernelsize_offset + sizeof(int)) > size_2bl_image) ||
		((CFG_2bl_dwkerneldatasize_offset + sizeof(int)) > size_2bl_image) ) {
		if (logFile) fprintf(logFile, "Invalid config params for kernel extract!\n");
		return 0;
	}

	kernelZ_size = *(unsigned int *)(p_2bl_image + CFG_2bl_dwkernelsize_offset);
	kernel_data_size = *(unsigned int *)(p_2bl_image + CFG_2bl_dwkerneldatasize_offset);

	kernel_top_rom_offset = (CFG_kernel_top_address & (size_rom_image-1));

	if ((kernelZ_size + kernel_data_size - 1) > kernel_top_rom_offset) {
		if (logFile) fprintf(logFile, "Invalid config params for kernel extract!\n");
		return 0;
	}

	kernel_rom_offset = (kernel_top_rom_offset + 1) - kernel_data_size - kernelZ_size;

	if (!(p_kernelZ = (unsigned char *) malloc(kernelZ_size))) {
		if (logFile) fprintf(logFile, "Not enough memory for kernel extract!\n");
		return 0;
	}

	// Copy-out the compressed kernel image
	memcpy(p_kernelZ, (p_rom_image + kernel_rom_offset), kernelZ_size);

	// Now go back to ROM image and wipe-out the area used by compressed kernel
	memset((p_rom_image + kernel_rom_offset), 0, kernelZ_size);

	// And the area used by the kernel initialized data section
	memset((p_rom_image + kernel_rom_offset + kernelZ_size), 0, kernel_data_size);

	*p_kernelZ_image = p_kernelZ;
	*p_size_kernelZ_image = kernelZ_size;

	return 1;
}

static int
decrypt_compressed_kernel_image(unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image,
						unsigned char *p_2bl_image, unsigned int size_2bl_image)
{
	RC4_KEY	rc4_key;
	unsigned char	kernel_key[RC4_KEYLEN];

	if (((CFG_2bl_kernelkey_offset + RC4_KEYLEN) > size_2bl_image)) {
		if (logFile) fprintf(logFile, "Invalid config params for kernel decrypt!\n");
		return 0;
	}
	memcpy(kernel_key, (p_2bl_image + CFG_2bl_kernelkey_offset), RC4_KEYLEN);

	RC4_set_key(&rc4_key, RC4_KEYLEN, kernel_key);
	RC4(&rc4_key, size_kernelZ_image, p_kernelZ_image, p_kernelZ_image);

	return 1;
}




