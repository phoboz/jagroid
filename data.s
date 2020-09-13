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

