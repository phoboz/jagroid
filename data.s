	include	"data_def.s"

	.data

	.globl  _tiles_pal
	.even
_tiles_pal:
	incbin  "levels/tunnel_tiles_cry.pal"

	.globl  _tiles_01
	.phrase
_tiles_01:
	incbin  "levels/tunnel_tiles.lz77"

	.globl  _tiles_02
	.phrase
_tiles_02:
	incbin  "levels/tiles_02.lz77"

	.globl	_level_01
	.even
_level_01:
	incbin	"levels/tunnel.lev"

	.globl	_level_02
	.even
_level_02:
	incbin	"levels/level_02.lev"

