// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2018-2020 by Jaime "Lactozilla" Passos.
// Copyright (C) 2019-2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  r_picformats.h
/// \brief Patch generation.

#ifndef __R_PICFORMATS__
#define __R_PICFORMATS__

#include "r_defs.h"
#include "doomdef.h"

typedef enum
{
	PICFMT_NONE = 0,

	// Doom formats
	PICFMT_PATCH,
	PICFMT_FLAT,

	// PNG
	PICFMT_PNG,

	// 16bpp
	PICFMT_PATCH16,
	PICFMT_FLAT16,

	// 32bpp
	PICFMT_PATCH32,
	PICFMT_FLAT32
} pictureformat_t;

typedef enum
{
	PICFLAGS_XFLIP = 1,
	PICFLAGS_YFLIP = 1<<1
} pictureflags_t;

enum
{
	PICDEPTH_NONE = 0,
	PICDEPTH_8BPP = 8,
	PICDEPTH_16BPP = 16,
	PICDEPTH_32BPP = 32
};

void *Picture_Convert(
	pictureformat_t informat, void *picture, pictureformat_t outformat,
	size_t insize, size_t *outsize,
	INT32 inwidth, INT32 inheight, INT32 inleftoffset, INT32 intopoffset,
	pictureflags_t flags);

void *Picture_PatchConvert(
	pictureformat_t informat, void *picture, pictureformat_t outformat,
	size_t insize, size_t *outsize,
	INT16 inwidth, INT16 inheight, INT16 inleftoffset, INT16 intopoffset,
	pictureflags_t flags);
void *Picture_FlatConvert(
	pictureformat_t informat, void *picture, pictureformat_t outformat,
	size_t insize, size_t *outsize,
	INT16 inwidth, INT16 inheight, INT16 inleftoffset, INT16 intopoffset,
	pictureflags_t flags);
void *Picture_GetPatchPixel(
	patch_t *patch, pictureformat_t informat,
	INT32 x, INT32 y,
	pictureflags_t flags);

void *Picture_TextureToFlat(size_t trickytex);

INT32 Picture_FormatBPP(pictureformat_t format);
boolean Picture_IsPatchFormat(pictureformat_t format);
boolean Picture_IsFlatFormat(pictureformat_t format);
boolean Picture_CheckIfPatch(patch_t *patch, size_t size);

// Structs
typedef enum
{
	ROTAXIS_X, // Roll (the default)
	ROTAXIS_Y, // Pitch
	ROTAXIS_Z  // Yaw
} rotaxis_t;

typedef struct
{
	INT32 x, y;
	rotaxis_t rotaxis;
} spriteframepivot_t;

typedef struct
{
	spriteframepivot_t pivot[64];
	boolean available;
} spriteinfo_t;

// Portable Network Graphics
boolean Picture_IsLumpPNG(const UINT8 *d, size_t s);
#define Picture_ThrowPNGError(lumpname, wadfilename) I_Error("W_Wad: Lump \"%s\" in file \"%s\" is a .png - please convert to either Doom or Flat (raw) image format.", lumpname, wadfilename); // Fears Of LJ Sonic

#ifndef NO_PNG_LUMPS
void *Picture_PNGConvert(
	const UINT8 *png, pictureformat_t outformat,
	INT32 *w, INT32 *h,
	INT16 *topoffset, INT16 *leftoffset,
	size_t insize, size_t *outsize,
	pictureflags_t flags);
boolean Picture_PNGDimensions(UINT8 *png, INT16 *width, INT16 *height, size_t size);
#endif

#define PICTURE_PNG_USELOOKUP

// SpriteInfo
extern spriteinfo_t spriteinfo[NUMSPRITES];
void R_LoadSpriteInfoLumps(UINT16 wadnum, UINT16 numlumps);
void R_ParseSPRTINFOLump(UINT16 wadNum, UINT16 lumpNum);

// Sprite rotation
#ifdef ROTSPRITE
INT32 R_GetRollAngle(angle_t rollangle);
void R_CacheRotSprite(spritenum_t sprnum, UINT8 frame, spriteinfo_t *sprinfo, spriteframe_t *sprframe, INT32 rot, UINT8 flip);
void R_FreeSingleRotSprite(spritedef_t *spritedef);
void R_FreeSkinRotSprite(size_t skinnum);
extern fixed_t rollcosang[ROTANGLES];
extern fixed_t rollsinang[ROTANGLES];
void R_FreeAllRotSprite(void);
#endif

#endif // __R_PATCH__
