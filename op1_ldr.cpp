/*
   Rigol firmware loader v0.2 for IDA Pro
   
   Copyright (c) 2011 by TomasVlad.

   Based on parts of libopcodes (Free Software Foundation, Inc.) and
   Simple Python Byte Code Module (Chris Eagle <cseagle@gmail.com>)

   Some changes from Andreas Schuler <andreas at schulerdev.de>
   
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

/*
        L O A D E R  for firmware files for Rigol oscilloscope.
*/

#include "../../ldr/idaldr.h"
#include "op1_ldr.h"

static ldr_hdr _hdr;
static ldr_hdr blk_hdr;
static int flg_fin;

//--------------------------------------------------------------------------
//
//      check input file format. if recognized, then return 1
//      and fill 'fileformatname'.
//      otherwise return 0
//
static int idaapi accept_ldr_file(linput_t *li,char fileformatname[MAX_FILE_FORMAT_NAME], int n) 
{
	if (n) {
		return(0);
	}

	qlseek(li, 0, SEEK_SET);
	if (!read_hdr(li, &_hdr)) {
		msg("Couldn't read a full block header.\n");
		return(0);
	}

	if (((_hdr.block_code & BFLAG_HDRSIGN_MASK) >> BFLAG_HDRSIGN_SHIFT) != BFLAG_HDRSIGN_MAGIC) {
		msg("Magic HDRSIGN byte doesn't match expected value.\n");
		return(0);
	}

	qstrncpy(fileformatname, "OP-1 friendly LDR file awwww yeeeah", MAX_FILE_FORMAT_NAME);

	return(1);
}

bool read_hdr(linput_t *li, ldr_hdr *hdr)
{
	unsigned char tmp[LDR_BLOCK_HEADER_LEN];

	if (qlread(li, tmp, LDR_BLOCK_HEADER_LEN) != LDR_BLOCK_HEADER_LEN) {
		return(false);
	}

	hdr->block_code  = tmp[0]  | tmp[1]  << 8 | tmp[2]  << 16 | tmp[3]  << 24;
	hdr->target_addr = tmp[4]  | tmp[5]  << 8 | tmp[6]  << 16 | tmp[7]  << 24;
	hdr->size        = tmp[8]  | tmp[9]  << 8 | tmp[10] << 16 | tmp[11] << 24;
	hdr->argument    = tmp[12] | tmp[13] << 8 | tmp[14] << 16 | tmp[15] << 24;

	return(true);
}
//--------------------------------------------------------------------------
//
//      load file into the database.
//
static void idaapi load_ldr_file(linput_t *li, ushort neflags,const char * /*fileformatname*/)
{
	flg_fin = 0;
	qlseek(li, 0, SEEK_SET);
	set_processor_type("Blackfin", SETPROC_ALL|SETPROC_FATAL);

	if (!add_segm(0, 0x00000000, 0x07ffffff, "SDRAM", CLASS_DATA)) loader_failure();
	if (!add_segm(0, 0xef000000, 0xef007fff, "BOOTROM", CLASS_CODE)) loader_failure();
	if (!add_segm(0, 0xff800000, 0xff807fff, "DATA1", CLASS_DATA)) loader_failure();
	if (!add_segm(0, 0xff900000, 0xff907fff, "DATA2", CLASS_DATA)) loader_failure();
	if (!add_segm(0, 0xffa00000, 0xffa07fff, "INST1", CLASS_CODE)) loader_failure();
	if (!add_segm(0, 0xffa08000, 0xffa0bfff, "INST2", CLASS_CODE)) loader_failure();
	if (!add_segm(0, 0xffa10000, 0xffa13fff, "INST3", CLASS_CODE)) loader_failure();
	if (!add_segm(0, 0xffb00000, 0xffb00fff, "SCRATCH", CLASS_DATA)) loader_failure();

	msg("Start loading\n");
	while(true)
	{
		if(!read_hdr(li, &blk_hdr)) loader_failure();
		msg("hdr:: addr=0x%0X, size=0x%0X, flags=0x%0X, args=0x%0x", blk_hdr.target_addr, blk_hdr.size, blk_hdr.block_code, blk_hdr.argument);
		if (blk_hdr.block_code & BFLAG_FINAL) {
			msg(" (finish)\n");
			flg_fin = 1;
		}

		if ((blk_hdr.block_code & BFLAG_IGNORE) || (blk_hdr.block_code & BFLAG_INIT)) {
			uchar *buf = (uchar *) malloc(blk_hdr.size);
			qlread(li, buf, blk_hdr.size);
			msg(" (ignore)\n");
			free(buf);
			continue;
		}

		if(blk_hdr.block_code & BFLAG_FILL) {
			msg(" (fill)\n");
			uchar *buf = (uchar *)malloc(blk_hdr.size);
			memset(buf, 0, blk_hdr.size);
			mem2base(buf, blk_hdr.target_addr, blk_hdr.target_addr + blk_hdr.size, -1);
			free(buf);
			continue;
		}
		msg("\n");

		//msg("file2base: 0x%0x-0x%0x (%d bytes)\n", blk_hdr.target_addr, blk_hdr.target_addr + blk_hdr.size, blk_hdr.size);
		file2base(li, qltell(li), blk_hdr.target_addr, blk_hdr.target_addr + blk_hdr.size, FILEREG_PATCHABLE);
		
		if(flg_fin) break;
	}

	inf.af =
	AF_FINAL|
	AF_FIXUP        | //   0x0001          // Create offsets and segments using fixup info
	AF_MARKCODE     | //   0x0002          // Mark typical code sequences as code
	AF_UNK          | //   0x0004          // Delete instructions with no xrefs
	AF_CODE         | //   0x0008          // Trace execution flow
	AF_PROC         | //   0x0010          // Create functions if call is present
	AF_USED         | //   0x0020          // Analyze and create all xrefs
	AF_FLIRT        | //   0x0040          // Use flirt signatures
	AF_PROCPTR      | //   0x0080          // Create function if data xref data->code32 exists
	AF_NULLSUB      | //   0x0200          // Rename empty functions as nullsub_...
	AF_IMMOFF       | //   0x2000          // Convert 32bit instruction operand to offset
	AF_DREFOFF    ; //   0x4000          // Create offset if data xref to seg32 exists
	inf.af2 = 0;

	set_processor_type("Blackfin", SETPROC_ALL|SETPROC_FATAL);
}

//----------------------------------------------------------------------
static bool idaapi init_loader_options(linput_t*)
{
  set_processor_type("Blackfin", SETPROC_ALL|SETPROC_FATAL);
  return true;
}

//----------------------------------------------------------------------
//
//      LOADER DESCRIPTION BLOCK
//
//----------------------------------------------------------------------
loader_t LDSC =
{
  IDP_INTERFACE_VERSION,
  0,                            // loader flags
  accept_ldr_file,
  load_ldr_file,
  NULL,
  NULL,
  NULL,
};
