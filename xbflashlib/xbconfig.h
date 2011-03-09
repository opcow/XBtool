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
 */
#ifndef	XBCONFIG_H
#define	XBCONFIG_H


//extern	int	read_xbconfig_file(const char *cfg_fname);


#define	RC4_KEYLEN		16

#define	PATH_SEP				"\\"

#define IMGFNAME_CABTMP			"cabtmp.cab"
#define	IMGFNAME_2BL			"2bl.img"
#define	IMGFNAME_FBL			"fbl.img"
#define	IMGFNAME_XBOXKRNL		"xboxkrnl.img"
#define	IMGFNAME_REMAINDER		"remainder.img"


extern	unsigned int	CFG_fbl_size;
extern	unsigned int	CFG_fbl_address;

extern	unsigned int	CFG_2bl_size;
extern	unsigned int	CFG_2bl_address;

extern	unsigned int	CFG_2bl_kernelkey_offset;
extern	unsigned int	CFG_2bl_dwkernelsize_offset;
extern	unsigned int	CFG_2bl_dwkerneldatasize_offset;

extern	unsigned int	CFG_kernel_top_address;

extern	unsigned int	CFG_2bl_dwflashstart_hashsize;
extern	unsigned int	CFG_2bl_sha_hash_offset;

extern	unsigned int	CFG_kernel_dwdatasize_offset;
extern	unsigned int	CFG_kernel_dwdatarombase_offset;
extern	unsigned int	CFG_kernel_dwdatarambase_offset;

extern	unsigned int	CFG_kernel_address_adj;

extern	unsigned int	CFG_RC4_keymethod;

extern	unsigned int	CFG_RC4_key_encrypt;

#define RC4_MAX_KEYLEN	128


extern	unsigned char CFG_RC4_key[RC4_MAX_KEYLEN];
extern	unsigned int	RC4_keylen;

#endif	// XBCONFIG_H
