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

	gfx_data	_player_frame_01,	"images/player_frame_01.rgb"
	gfx_data	_player_frame_02,	"images/player_frame_02.rgb"
	gfx_data	_player_frame_03,	"images/player_frame_03.rgb"
	gfx_data	_player_frame_04,	"images/player_frame_04.rgb"
	gfx_data	_player_frame_05,	"images/player_frame_05.rgb"
	gfx_data	_player_frame_06,	"images/player_frame_06.rgb"
	gfx_data	_player_frame_07,	"images/player_frame_07.rgb"
	gfx_data	_player_frame_08,	"images/player_frame_08.rgb"
	gfx_data	_player_frame_09,	"images/player_frame_09.rgb"
	gfx_data	_player_frame_10,	"images/player_frame_10.rgb"
	gfx_data	_player_frame_11,	"images/player_frame_11.rgb"
	gfx_data	_player_frame_12,	"images/player_frame_12.rgb"
	gfx_data	_player_frame_13,	"images/player_frame_13.rgb"
	gfx_data	_player_frame_14,	"images/player_frame_14.rgb"
	gfx_data	_player_frame_15,	"images/player_frame_15.rgb"
	gfx_data	_player_frame_16,	"images/player_frame_16.rgb"
	gfx_data	_player_frame_17,	"images/player_frame_17.rgb"

	.globl		_player_frames
	.even
_player_frames:
	dc.l		_player_frame_01
	dc.l		_player_frame_02
	dc.l		_player_frame_03
	dc.l		_player_frame_04
	dc.l		_player_frame_05
	dc.l		_player_frame_06
	dc.l		_player_frame_07
	dc.l		_player_frame_08
	dc.l		_player_frame_09
	dc.l		_player_frame_10
	dc.l		_player_frame_11
	dc.l		_player_frame_12
	dc.l		_player_frame_13
	dc.l		_player_frame_14
	dc.l		_player_frame_15
	dc.l		_player_frame_16
	dc.l		_player_frame_17

