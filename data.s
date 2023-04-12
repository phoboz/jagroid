	include	"data_def.s"

	.data

	.globl  _tiles_pal
	.even
_tiles_pal:
	incbin  "images/tiles_cry.pal"

	.globl  _tiles_01
	.phrase
_tiles_01:
	incbin  "images/tiles_01.lz77"

	.globl  _tiles_02
	.phrase
_tiles_02:
	incbin  "images/tiles_02.lz77"

	.globl	_level_01
	.even
_level_01:
	incbin	"levels/level_01.lev"

	.globl	_level_02
	.even
_level_02:
	incbin	"levels/level_02.lev"

