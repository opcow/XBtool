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
 * cabio.c - CABinet (.cab) file I/O routines
 *           Requires MS Cabinet SDK, available from: 
 *                http://msdn.microsoft.com/library/en-us/dnsamples/cab-sdk.exe
 * 
 */
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <dos.h>
#include <sys/stat.h>

#include <fdi.h>
#include <fci.h>

#include "cabio.h"
#include "xbflashlib.h"


#define	COMPRESSION_TYPE	(tcompTYPE_LZX+(17<<tcompSHIFT_LZX_WINDOW))

// Local functions
static void	write_byte(FILE *fh, int val);
static void	write_word(FILE *fh, unsigned int val);
static void	write_dword(FILE *fh, unsigned int val);
static void	write_string(FILE *fh, const char *str);
static void	write_CFHEADER(FILE *fh, unsigned int cabsize, unsigned int cffile_off);
static void	write_CFFOLDER(FILE *fh, unsigned int num_cfdata, unsigned int cfdata_off);
static void	write_CFFILE(FILE *fh, const char *fname, unsigned int uncmp_file_size);
static unsigned int	write_CFDATA(FILE *fh, int this_cmp, int this_uncmp, char *cfdata_buf);


int
create_kernel_cab_from_compressed(const char *cab_temp_fname, const char *cab_internal_fname, unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image)
{
	FILE	*fh_cab;
	long	total_cmp, total_uncmp;
	unsigned int	stream_offset;
	int	this_cmp, this_uncmp;
	long	cab_total_size;
	int	cab_num_cfdata;
	long	cab_uncmp_file_size;
	long	cab_cfdata_off, cab_cffile_off;
	unsigned short	*p_short;

	if (!(fh_cab = fopen(cab_temp_fname, "wb"))) {
		if (logFile) fprintf(logFile, "Cannot create temporary .cab file!\n");
		return 0;
	}

	// write place holder header items
	write_CFHEADER(fh_cab, 0, 0);	// cabsize, cffile_off
	write_CFFOLDER(fh_cab, 0, 0);	// num_cfdata, cfdata_off
	cab_cffile_off = ftell(fh_cab);
	write_CFFILE(fh_cab, cab_internal_fname, 0);	// uncmp_file_size
	cab_cfdata_off = ftell(fh_cab);

	cab_total_size = ftell(fh_cab);
	cab_num_cfdata = 0;
	cab_uncmp_file_size = 0;

	total_cmp = 0;
	total_uncmp = 0;
	stream_offset = 0;
	while(1) {
		if (stream_offset >= size_kernelZ_image) {
			break;
		}
		p_short = (unsigned short *)(p_kernelZ_image + stream_offset);
		this_cmp = *p_short++;
		this_uncmp = *p_short++;

		if (this_cmp == 0) {
			break;
		}

		cab_total_size += write_CFDATA(fh_cab, this_cmp, this_uncmp, (char *)p_short);
		cab_num_cfdata += 1;
		cab_uncmp_file_size += this_uncmp;

		total_cmp += this_cmp;
		total_uncmp += this_uncmp;
		stream_offset += (this_cmp + 4);
	}

	// re-write CAB header with correct values
	fseek(fh_cab, 0L, SEEK_SET);
	write_CFHEADER(fh_cab, cab_total_size, cab_cffile_off);	// cabsize, cffile_off
	write_CFFOLDER(fh_cab, cab_num_cfdata, cab_cfdata_off);	// num_cfdata, cfdata_off
	write_CFFILE(fh_cab, cab_internal_fname, cab_uncmp_file_size);	// uncmp_file_size
	
	fclose(fh_cab);
	return 1;
}


FNALLOC(mem_alloc)
{
	return malloc(cb);
}

FNFREE(mem_free)
{
	free(pv);
}

FNOPEN(file_open)
{
	return _open(pszFile, oflag, pmode);
}

FNREAD(file_read)
{
	return _read(hf, pv, cb);
}

FNWRITE(file_write)
{
	return _write(hf, pv, cb);
}

FNCLOSE(file_close)
{
	return _close(hf);
}

FNSEEK(file_seek)
{
	return _lseek(hf, dist, seektype);
}

const char	*fdi_destdir;

FNFDINOTIFY(fdi_notify)
{
	char	fname_buf[BUFSIZ];
	int	fh;

	switch(fdint)
	{
	case fdintCOPY_FILE:
		strcpy(fname_buf, fdi_destdir);
		strcat(fname_buf, pfdin->psz1);
		fh = file_open(fname_buf, _O_BINARY|_O_CREAT|_O_WRONLY|_O_SEQUENTIAL,
					_S_IREAD|_S_IWRITE);
		return fh;

	case fdintCLOSE_FILE_INFO:
		file_close(pfdin->hf);
		return TRUE;
	default:
		break;
	}
	return 0;
}

int
extract_decompressed_from_kernel_cab(const char *cab_temp_base_fname, const char *dirname)
{
	HFDI	hfdi;
	ERF		erf;

	fdi_destdir = dirname;

	hfdi = FDICreate(mem_alloc, mem_free, file_open, file_read,
				file_write, file_close, file_seek, cpu80386, &erf);
	if (!hfdi) {
		if (logFile) fprintf(logFile, "FDICreate() error!\n");
		return 0;
	}
	if (FDICopy(hfdi, (char *)cab_temp_base_fname, (char *)dirname, 0, fdi_notify, NULL, NULL) != TRUE) {
		FDIDestroy(hfdi);
		if (logFile) fprintf(logFile, "FDICopy() error!\n");
		return 0;
	}

	FDIDestroy(hfdi);
	return 1;
}

FNFCIOPEN(fci_open)
{
	int	result;

	result = _open(pszFile, oflag, pmode);

	if (result == -1) {
		*err = errno;
	}

	return result;
}

FNFCIREAD(fci_read)
{
	unsigned int	result;

	result = (unsigned int)_read(hf, memory, cb);

	if (result != cb) {
		*err = errno;
	}

	return result;
}

FNFCIWRITE(fci_write)
{
	unsigned int	result;

	result = (unsigned int)_write(hf, memory, cb);

	if (result != cb) {
		*err = errno;
	}

	return result;
}

FNFCICLOSE(fci_close)
{
	int	result;

	result = _close(hf);

	if (result != 0) {
		*err = errno;
	}

	return result;
}

FNFCISEEK(fci_seek)
{
	long	result;

	result = _lseek(hf, dist, seektype);

	if (result == -1) {
		*err = errno;
	}

	return result;
}

FNFCIDELETE(fci_delete)
{
	int	result;

	result = remove(pszFile);

	if (result != 0) {
		*err = errno;
	}

	return result;
}


FNFCIFILEPLACED(file_placed)
{
	return 0;
}

FNFCIGETTEMPFILE(get_temp_file)
{
	char	*psz;

	psz = _tempnam("", "xx");
	if (psz && (strlen(psz) < (unsigned)cbTempName)) {
		strcpy(pszTempName, psz);
		free(psz);
		return TRUE;
	}

	if (psz) {
		free(psz);
	}

	return FALSE;
}

FNFCISTATUS(progress)
{
	return 0;
}

FNFCIGETNEXTCABINET(get_next_cabinet)
{
	// Is this really needed??
	return TRUE;
}

FNFCIGETOPENINFO(get_open_info)
{
	*pattribs = 0;

	return _open(pszName, _O_RDONLY|_O_BINARY);
}


int
create_kernel_cab_from_uncompressed(const char *cab_temp_base_fname, const char *dirname, const char *cab_internal_fname, unsigned char *in_kernel_fname)
{
	HFCI	hfci;
	ERF	erf;
	CCAB	cab_parameters;

	memset(&cab_parameters, 0, sizeof(cab_parameters));

	cab_parameters.cb = 0x7fffffff;
	cab_parameters.cbFolderThresh = 0x7fffffff;
	cab_parameters.iCab = 1;
	cab_parameters.iDisk = 0;
	cab_parameters.setID = 12345;

	strcpy(cab_parameters.szCabPath, dirname);
	strcpy(cab_parameters.szCab, cab_temp_base_fname);

	hfci = FCICreate(&erf, file_placed, mem_alloc, mem_free,
				fci_open, fci_read, fci_write, fci_close, fci_seek,
				fci_delete, get_temp_file, &cab_parameters, 0);

	if (!hfci) {
		if (logFile) fprintf(logFile, "FCICreate() error!\n");
		return 0;
	}

	if (FCIAddFile(hfci, (char *)in_kernel_fname, (char *)cab_internal_fname, TRUE,
			get_next_cabinet, progress, get_open_info, COMPRESSION_TYPE) == FALSE) {
		if (logFile) fprintf(logFile, "FCIAddFile() error!\n");
		FCIDestroy(hfci);
		return 0;
	}
	if (FCIFlushFolder(hfci, get_next_cabinet, progress) == FALSE) {
		if (logFile) fprintf(logFile, "FCIFlushFolder() error!\n");
		FCIDestroy(hfci);
		return 0;
	}
	if (FCIFlushCabinet(hfci, FALSE, get_next_cabinet, progress) == FALSE) {
		if (logFile) fprintf(logFile, "FCIFlushCabinet() error!\n");
		FCIDestroy(hfci);
		return 0;
	}
	if (FCIDestroy(hfci) != TRUE) {
		if (logFile) fprintf(logFile, "FCIDestroy() error!\n");
		return 0;
	}
	return 1;
}

int
extract_compressed_from_kernel_cab(const char *cab_temp_fname, unsigned char **pp_kernelZ_image, unsigned int *p_size_kernelZ_image)
{
	FILE	*fh;
	char	buf[20];
	unsigned int	cffile_offset;
	unsigned int	cffile_unc_size;
	unsigned int	cfdata_offset;
	unsigned short	num_cfdata_blocks;
	unsigned int	offset;
	int	block;
	unsigned int	this_csum;
	unsigned short	this_cmp, this_uncmp;
	unsigned int	cfdata_total_cmp, cfdata_total_uncmp;
	unsigned char	*p_buf, *p;

	if (!(fh = fopen(cab_temp_fname, "rb"))) {
		if (logFile) fprintf(logFile, "Couldn't open temporary CAB: '%s'!\n", cab_temp_fname);
		return 0;
	}
	if (fread(buf, 1, 4, fh) != 4) {
		if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
		fclose(fh);
		return 0;
	}
	if (strncmp(buf, "MSCF", 4)) {
		if (logFile) fprintf(logFile, "Error reading temporary CAB (bad header): '%s'!\n", cab_temp_fname);
		fclose(fh);
		return 0;
	}
	// Read offset to CFFILE section
	fseek(fh, 16, SEEK_SET);
	if (fread(&cffile_offset, 1, 4, fh) != 4) {
		if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
		fclose(fh);
		return 0;
	}

	fseek(fh, cffile_offset, SEEK_SET);
	if (fread(&cffile_unc_size, 1, 4, fh) != 4) {
		if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
		fclose(fh);
		return 0;
	}

	// FIXME: Doesn't deal with optional CFHEADER fields that'll make it
	// larger than 36-bytes

	// Seek to the first CFFOLDER entry
	fseek(fh, 36, SEEK_SET);
	if ((fread(&cfdata_offset, 1, 4, fh) != 4) ||
			(fread(&num_cfdata_blocks, 1, 2, fh) != 2)) {
		if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
		fclose(fh);
		return 0;
	}

	// FIXME: Doesn't deal with possible optional reserved bytes in CFDATA

	// First, scan through the blocks and add-up the total compressed size

	cfdata_total_uncmp = 0;
	cfdata_total_cmp = 0;

	offset = cfdata_offset;
	for (block = 0; block < num_cfdata_blocks; block++) {
		fseek(fh, offset, SEEK_SET);
		if ((fread(&this_csum, 1, 4, fh) != 4) ||
				(fread(&this_cmp, 1, 2, fh) != 2) ||
				(fread(&this_uncmp, 1, 2, fh) != 2)) {
			if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
			fclose(fh);
			return 0;
		}
		cfdata_total_cmp += this_cmp + (2 + 2);	// don't include csum in size
		cfdata_total_uncmp += this_uncmp;

		offset += (this_cmp + (4 + 2 + 2));
	}

	if ((cffile_unc_size != cfdata_total_uncmp)) {
		if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
		fclose(fh);
		return 0;
	}

	// Allocate space for the compressed image (now that we know the size)
	//
	if (!(p_buf = (unsigned char *) malloc(cfdata_total_cmp))) {
		if (logFile) fprintf(logFile, "Not enough memory for compressed kernel image!\n");
		fclose(fh);
		return 0;
	}

	p = p_buf;
	offset = cfdata_offset;
	for (block = 0; block < num_cfdata_blocks; block++) {
		fseek(fh, offset, SEEK_SET);
		if ((fread(&this_csum, 1, 4, fh) != 4) ||
				(fread(&this_cmp, 1, 2, fh) != 2) ||
				(fread(&this_uncmp, 1, 2, fh) != 2)) {
			if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
			fclose(fh);
			return 0;
		}
		((unsigned short *)p)[0] = this_cmp;
		((unsigned short *)p)[1] = this_uncmp;

		if (fread(p+4, 1, this_cmp, fh) != (unsigned int)this_cmp) {
			if (logFile) fprintf(logFile, "Error reading temporary CAB: '%s'!\n", cab_temp_fname);
			free(p_buf);
			fclose(fh);
			return 0;
		}

		offset += (this_cmp + (4 + 2 + 2));
		p += (this_cmp + (2 + 2));
	}
	fclose(fh);

	*pp_kernelZ_image = p_buf;
	*p_size_kernelZ_image = cfdata_total_cmp;

	return 1;
}


static void
write_byte(FILE *fh, int val)
{
	fputc((val & 0xff), fh);
}

static void
write_word(FILE *fh, unsigned int val)
{
	write_byte(fh, (val & 0xff));
	write_byte(fh, ((val >> 8) & 0xff));
}

static void
write_dword(FILE *fh, unsigned int val)
{
	write_word(fh, (val & 0xffff));
	write_word(fh, ((val >> 16) & 0xffff));
}

static void
write_string(FILE *fh, const char *str)
{
	do {
		write_byte(fh, *str);
	} while(*str++);
}

static void
write_CFHEADER(FILE *fh, unsigned int cabsize, unsigned int cffile_off)
{
	write_byte(fh, 'M');
	write_byte(fh, 'S');
	write_byte(fh, 'C');
	write_byte(fh, 'F');

	write_dword(fh, 0);	// reserved

	write_dword(fh, cabsize);	// size of this cab file in bytes

	write_dword(fh, 0);	// reserved

	write_dword(fh, cffile_off);	// offset of first CFFILE entry

	write_dword(fh, 0);	// reserved

	write_byte(fh, 0x03);	// cab format version
	write_byte(fh, 0x01);

	write_word(fh, 1);	// # of CFFOLDERs in this cab file

	write_word(fh, 1);	// # of CFFILEs in this cab file

	write_word(fh, 0);	// flags

	write_word(fh, 0x3039);	// set ID

	write_word(fh, 0);	// number of this cab file in a set
}

static void
write_CFFOLDER(FILE *fh, unsigned int num_cfdata, unsigned int cfdata_off)
{
	write_dword(fh, cfdata_off);	// offset of first CFDATA block

	write_word(fh, num_cfdata);	// num CFDATA blocks in this folder

	write_word(fh, 0x1103);	// compression type
}


static void
write_CFFILE(FILE *fh, const char *fname, unsigned int uncmp_file_size)
{
	write_dword(fh, uncmp_file_size);	// uncompressed size of this file

	write_dword(fh, 0);		// uncompressed offset of this file in folder

	write_word(fh, 0);	// index into the CFFOLDER area

	write_word(fh, 0x27dc);	// date stamp

	write_word(fh, 0x6bdd);	// time stamp

	write_word(fh, 0x0020);	// attribs

	write_string(fh, fname);
}

static unsigned int
write_CFDATA(FILE *fh, int this_cmp, int this_uncmp, char *cfdata_buf)
{
	unsigned char *p;
	unsigned int	csum;
	int	i;
	csum = 0;
	p = (unsigned char *)cfdata_buf;
	for (i = 0; i < this_cmp; i++) {
		csum += *p++;
	}

	csum = 0;
	write_dword(fh, csum);	// checksum
	write_word(fh, this_cmp);
	write_word(fh, this_uncmp);

	fwrite(cfdata_buf, 1, this_cmp, fh);

	return (4 + 2 + 2 + this_cmp);
}
