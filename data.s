	include	"data_def.s"

	.data

	.globl  _tiscavPal
	.even
_tiscavPal:
	incbin  "images/tiscav_rgb.pal"

	.globl  _tiscavTiles
	.phrase
_tiscavTiles:
	incbin  "images/tiscav.map"

	.globl	_tiscavLevel01
	.even
_tiscavLevel01:
	incbin	"levels/tiscav01.lev"

	.globl	_tiscavLevel02
	.even
_tiscavLevel02:
	incbin	"levels/tiscav02.lev"

	.globl  _tiles_01_pal
	.even
_tiles_01_pal:
	incbin  "images/tiles_01_cry.pal"

	.globl  _tiles_01
	.phrase
_tiles_01:
	incbin  "images/tiles_01.lz77"

	.globl	_level_01
	.even
_level_01:
	incbin	"levels/level_01.lev"

