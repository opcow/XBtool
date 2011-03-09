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
 * xbconfig.c - config file (xbflash.ini) access functions
 * 
 */
#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "xbconfig.h"
#include "xbflashlib.h"

typedef struct tagCFG_ENTRY {
	const char	*ce_keyword;
	void	*ce_p_data;
	int	ce_size_data;	// At the moment, only '4' is valid here
	int	ce_optional;
	unsigned int	*ce_p_actual_size;
	int	ce_saw_keyword;
} CFG_ENTRY;


// Local functions

static void	init_config_vals(void);
static int	read_config_vals(const char *cfgname);
static int	get_cfg_data(char *keyword, char *data, CFG_ENTRY *pentry);
static int	verify_config_vals(void);
static void strip_spaces(char *buf);


unsigned char CFG_RC4_key[RC4_MAX_KEYLEN];
unsigned int	RC4_keylen = 0;

unsigned int	CFG_fbl_size = 0;
unsigned int	CFG_fbl_address = 0;

unsigned int	CFG_2bl_size;
unsigned int	CFG_2bl_address;

unsigned int	CFG_2bl_kernelkey_offset;
unsigned int	CFG_2bl_dwkernelsize_offset;
unsigned int	CFG_2bl_dwkerneldatasize_offset;

unsigned int	CFG_kernel_top_address;

unsigned int	CFG_2bl_dwflashstart_hashsize;
unsigned int	CFG_2bl_sha_hash_offset;

unsigned int	CFG_kernel_dwdatasize_offset;
unsigned int	CFG_kernel_dwdatarombase_offset;
unsigned int	CFG_kernel_dwdatarambase_offset;


unsigned int	CFG_kernel_address_adj;

unsigned int	CFG_RC4_keymethod = 0;

extern	unsigned int	CFG_RC4_key_encrypt = 2;

enum {
	mandatory,
	optional
};
//const int optional = 1;
//const int mandatory = 0;

CFG_ENTRY	CFG_tab[] = {
	{ "RC4_key_encrypt", 
		&CFG_RC4_key_encrypt, 
		sizeof(CFG_RC4_key_encrypt), 
		mandatory },
	{ "RC4_keymethod", &CFG_RC4_keymethod, sizeof(CFG_RC4_keymethod), optional },
	{ "RC4_key", CFG_RC4_key, sizeof(CFG_RC4_key), 0, &RC4_keylen, mandatory },
	{ "FBL_base_ROM_address", &CFG_fbl_address, sizeof(CFG_fbl_address), optional },
	{ "FBL_size", &CFG_fbl_size, sizeof(CFG_fbl_size), optional },
	{ "2BL_base_ROM_address", &CFG_2bl_address, sizeof(CFG_2bl_address), mandatory },
	{ "2BL_size", &CFG_2bl_size, sizeof(CFG_2bl_size), mandatory },
	{ "KERNEL_top_ROM_address", &CFG_kernel_top_address, sizeof(CFG_kernel_top_address), mandatory },
	{ "KERNEL_address_adjust", &CFG_kernel_address_adj, sizeof(CFG_kernel_address_adj), mandatory },
	{ "2BL_kernelkey_offset", &CFG_2bl_kernelkey_offset, sizeof(CFG_2bl_kernelkey_offset), mandatory },
	{ "2BL_dwkerneldatasize_offset", &CFG_2bl_dwkerneldatasize_offset, sizeof(CFG_2bl_dwkerneldatasize_offset), mandatory },
	{ "2BL_dwflashstart_hashsize", &CFG_2bl_dwflashstart_hashsize, sizeof(CFG_2bl_dwflashstart_hashsize), mandatory },
	{ "2BL_dwkernelsize_offset", &CFG_2bl_dwkernelsize_offset, sizeof(CFG_2bl_dwkernelsize_offset), mandatory },
	{ "2BL_sha_digest_offset", &CFG_2bl_sha_hash_offset, sizeof(CFG_2bl_sha_hash_offset), mandatory },
	{ "KERNEL_dwdatasize_offset", &CFG_kernel_dwdatasize_offset, sizeof(CFG_kernel_dwdatasize_offset), mandatory },
	{ "KERNEL_dwdataROMbase_offset", &CFG_kernel_dwdatarombase_offset, sizeof(CFG_kernel_dwdatarombase_offset), mandatory },
	{ "KERNEL_dwdataRAMbase_offset", &CFG_kernel_dwdatarambase_offset, sizeof(CFG_kernel_dwdatarambase_offset), mandatory },
};
int	CFG_tab_size = sizeof(CFG_tab) / sizeof(CFG_tab[0]);

FILE *logFile = NULL;

int
open_log(const char *logFileName, const char *openText)
{
	if(!(logFile = fopen(logFileName, "a")))
		return 0;
	if (logFile)
		fprintf(logFile, openText);
	return 1;
}

void
close_log()
{
	fclose(logFile);
}

int
read_xbconfig_file(const char *cfgfname)
{
	init_config_vals();

	if (!read_config_vals(cfgfname)) {
		return 0;
	}

	if (!verify_config_vals()) {
		return 0;
	}
	return 1;
}

static void
init_config_vals(void)
{
	int	i;
	CFG_ENTRY	*pentry;

	pentry = CFG_tab;
	for (i = 0; i < CFG_tab_size; i++, pentry++) {
		memset(pentry->ce_p_data, 0, pentry->ce_size_data);
		pentry->ce_saw_keyword = 0;
	}
}

static int
read_config_vals(const char *cfgfname)
{
	FILE	*fh;
	char	buf[BUFSIZ+1];
	char	*p_eq;
	int	i;
	CFG_ENTRY	*pentry;
	int	error = 0;

	if (!(fh = fopen(cfgfname, "r"))) {
		fprintf(logFile, "Cannot open config file: '%s'!\n", cfgfname);
		return 0;
	}

	while(fgets(buf, sizeof(buf)-1, fh)) {
		strip_spaces(buf);

		if (!buf[0] || (buf[0] == '#')) {
			continue;
		}

		if (!(p_eq = strchr(buf, '='))) {
			fprintf(logFile, "Bad config line format: '%s'!\n", buf);
			error = 1;
			continue;
		}

		*p_eq++ = 0;
		strip_spaces(buf);

		pentry = CFG_tab;
		for (i = 0; i < CFG_tab_size; i++, pentry++) {
			if (!stricmp(pentry->ce_keyword, buf)) {
				if (!get_cfg_data(buf, p_eq, pentry)) {
					error = 1;
				}
				pentry->ce_saw_keyword = 1;
				break;
			}
		}
		if ((i == CFG_tab_size)) {
			fprintf(logFile, "Unknown config entry: '%s'!\n", buf);
			error = 1;
		}
	}

	fclose(fh);

	return !error;
}

static int
get_cfg_data(char *p_keyword, char *p_data, CFG_ENTRY *pentry)
{
	unsigned int	val;
	int	i;
	unsigned char	*p;

	if (pentry->ce_p_actual_size) {
		*(pentry->ce_p_actual_size) = 0;
	}

	while(*p_data && isspace(*p_data)) {
		++p_data;
	}
	val = strtoul(p_data, 0, 0);

	switch(pentry->ce_size_data) {
	case 2:
		if (val <= 0xffff) {
			*(unsigned short *)(pentry->ce_p_data) = (unsigned short)val;
			if (pentry->ce_p_actual_size) {
				*(pentry->ce_p_actual_size) = 2;
			}
			return 1;
		} else {
			fprintf(logFile, "Config error: data value 0x%8.8x too large for keyword '%s'!\n", val, p_keyword);
			return 0;
		}
	case 4:
		*(unsigned int *)(pentry->ce_p_data) = val;
		if (pentry->ce_p_actual_size) {
			*(pentry->ce_p_actual_size) = 4;
		}
		return 1;
	default:
		break;
	}

	p = (unsigned char *)(pentry->ce_p_data);
	for (i = 0; i < pentry->ce_size_data; i++) {
		while(*p_data && isspace(*p_data)) {
			++p_data;
		}
		if (!p_data[0]) {
			// fprintf(logFile, "Config error: Not enough data for keyword '%s'!\n", p_keyword);
			return 1;
		}
		val = strtoul(p_data, &p_data, 0);
		if (val > 0xff) {
			fprintf(logFile, "Config error: data value 0x%8.8x too large for keyword '%s'!\n", val, p_keyword);
			return 0;
		}
		*p++ = (unsigned char)val;
		if (pentry->ce_p_actual_size) {
			*(pentry->ce_p_actual_size) += 1;
		}
	}
	return 1;
}

static int
verify_config_vals(void)
{
	int	i;
	CFG_ENTRY	*pentry;
	int	error = 0;

	int encryptLevel;

	encryptLevel	= *((int *) CFG_tab[0].ce_p_data);

	if( encryptLevel &&  !*(CFG_tab[2].ce_p_actual_size))
	{
		fprintf(logFile, "Config error: '%s=%d', but '%s' was not specified!\n", CFG_tab[0].ce_keyword,
			encryptLevel, CFG_tab[2].ce_keyword);
		return 0;
	}
	pentry = CFG_tab;
	for (i = 0; i < CFG_tab_size; i++, pentry++) {
		if (pentry->ce_optional) {
			continue;
		}
		if (!pentry->ce_saw_keyword) {
			fprintf(logFile, "Config file did not specify: '%s'!\n", 
				pentry->ce_keyword);
			error = 1;
		}
	}
	return !error;
}

static void
strip_spaces(char *buf)
{
	char	*p;

	if ((p = strchr(buf, '\r'))) {
		*p = 0;
	}
	if ((p = strchr(buf, '\n'))) {
		*p = 0;
	}
	p = strchr(buf, '\0');
	while((p > buf) && isspace(p[-1])) {
		*--p = 0;
	}
}