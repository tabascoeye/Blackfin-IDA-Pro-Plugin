/*
   Rigol firmware loader v0.2 for IDA Pro
   
   Copyright (c) 2011 by TomasVlad.

   Based on parts of libopcodes (Free Software Foundation, Inc.) and
   Simple Python Byte Code Module (Chris Eagle <cseagle@gmail.com>)

   Some minor changes from Andreas Schuler <andreas at schulerdev.de>
   
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your option) 
   any later version.
   
   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
   more details.
   
   You should have received a copy of the GNU General Public License along with 
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
   Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __OP1_LDR_H__
#define __OP1_LDR_H__

/* block flags */
#define BFLAG_DMACODE_MASK      0x0000000F
#define BFLAG_DMACODE_SHIFT     0
#define BFLAG_SAFE              0x00000010
#define BFLAG_AUX               0x00000020
#define BFLAG_FILL              0x00000100
#define BFLAG_QUICKBOOT         0x00000200
#define BFLAG_CALLBACK          0x00000400
#define BFLAG_INIT              0x00000800
#define BFLAG_IGNORE            0x00001000
#define BFLAG_INDIRECT          0x00002000
#define BFLAG_FIRST             0x00004000
#define BFLAG_FINAL             0x00008000
#define BFLAG_HDRSIGN_MASK      0xFF000000
#define BFLAG_HDRSIGN_SHIFT     24
#define BFLAG_HDRSIGN_MAGIC     0xAD
#define BFLAG_HDRCHK_MASK       0x00FF0000
#define BFLAG_HDRCHK_SHIFT      16

#define LDR_BLOCK_HEADER_LEN (16)
struct ldr_hdr {
	uint32   block_code;
	uint32   target_addr;
	uint32   size;
	uint32   argument;
};

bool read_hdr(linput_t *li, ldr_hdr *hdr);

/*
 *  Bit defitions for ldr_hdr_flags
 */

#define _ZEROFILL     0x0001
#define _RESVECT      0x0002
#define _INIT         0x0008
#define _IGNORE       0x0010
#define _FINAL        0x8000


#endif
