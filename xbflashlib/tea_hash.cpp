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
 * tea_hash.c - TEA hashing functions
 * 
 */
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xbconfig.h"
#include "tea_hash.h"

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
// typedef unsigned int	DWORD;

static void tea_hash_func(void *p_start, int nbytes, DWORD ival1, DWORD ival2, DWORD *dwaResult);

#define FBL_START_ADDR 0xffffd400
#define FBL_END_ADDR 0xfffffc7f


TEA_HASH	FBL_expected_TEA_hash = { 0xc7, 0x40, 0x67, 0xaf, 0x5d, 0x2a, 0xd5, 0xda, 0x93, 0x32, 0xd4, 0xc5, 0xe8, 0xf4, 0xbf, 0x68 };

void
calc_TEA_hash(unsigned char *p_base, unsigned int count, unsigned int ival1, unsigned int ival2, TEA_HASH *p_hash)
{
	memset(p_hash, 0, sizeof(TEA_HASH));
	tea_hash_func(p_base, count, (DWORD)ival1, (DWORD)ival2, (DWORD *)p_hash);
}


int
is_expected_FBL_TEA_hash(TEA_HASH *p_hash)
{
	if (!memcmp(p_hash, FBL_expected_TEA_hash, sizeof(TEA_HASH))) {
		return 1;
	} else {
		return 0;
	}
}

#define Q32_MAGIC 0x9e3779b9
#define MAGICVAL_2 0x8f000

#define SWAP_DWORD(v1, v2) \
{ \
DWORD tmp; \
tmp = v1; \
v1 = v2; \
v2 = tmp; \
}

static void tea_hash_func(void *p_start, int nbytes, DWORD ival1, DWORD ival2, DWORD *dwaResult)
{
DWORD dwaTemp[3];
DWORD dwv1, dwv2;
DWORD num_dword_pairs_in = (nbytes >> 3);
DWORD *pdw_in = (DWORD *)(void *)p_start;
int	pass;
int	n;

dwv1 = ival1;
dwv2 = ival2;

dwaResult[2] = dwv2;
dwaResult[3] = MAGICVAL_2;

while(num_dword_pairs_in--) {

// operates on the same input dword pair twice
for (pass = 0; pass < 2; pass++) {

dwaResult[0] = pdw_in[0];
dwaResult[1] = pdw_in[1];

dwaTemp[0] = 0;
dwaTemp[1] = dwv1;
dwaTemp[2] = dwv2;

for(n=0;n<32;n++) {

dwaTemp[0] += Q32_MAGIC;

dwv1 += (dwv2 + dwaTemp[0]) ^ ((dwv2 << 4) + dwaTemp[1]) ^ 
((dwv2 >> 5) + dwaTemp[2]);
dwv2 += (dwv1 + dwaTemp[0]) ^ ((dwv1 << 4) + dwaResult[0]) ^ 
((dwv1 >> 5) + dwaResult[1]);

}

SWAP_DWORD(dwaResult[2], dwv1);
SWAP_DWORD(dwaResult[3], dwv2);
}

// Move to next dword pair in source data
pdw_in += 2;
}

dwaResult[0] = dwv1;
dwaResult[1] = dwv2;
}

