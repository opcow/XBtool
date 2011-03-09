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
#ifndef CABIO_H
#define	CABIO_H

extern	int	create_kernel_cab_from_compressed(const char *cab_temp_fname, const char *cab_internal_fname, unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image);
extern	int	extract_decompressed_from_kernel_cab(const char *cab_temp_fname, const char *outdirname);

extern	int	create_kernel_cab_from_uncompressed(const char *cab_temp_base_fname, const char *dirname, const char *cab_internal_fname, unsigned char *in_kernel_fname);
extern	int	extract_compressed_from_kernel_cab(const char *cab_temp_fname, unsigned char **pp_kernelZ_image, unsigned int *p_size_kernelZ_image);

#endif	// CABIO_H
