#pragma once


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

