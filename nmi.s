; This software is provided 'as-is', without any express or implied warranty.
; In no event will the authors be held liable for any damages arising from
; the use of this software.
;
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
;
; 1. The origin of this software must not be misrepresented; you must not
; claim that you wrote the original software. If you use this software in
; a product, an acknowledgment in the product documentation would be
; appreciated but is not required.
;
; 2. Altered source versions must be plainly marked as such, and must not
; be misrepresented as being the original software.
;
; 3. This notice may not be removed or altered from any source distribution.

;
; Startup code for cc65 (NES version)
;
; by Yahweasel
; based on code by Groepaz/Hitmen <groepaz@gmx.net>
; based on code by Ullrich von Bassewitz <uz@cc65.org>
; See cc65: https://cc65.github.io/
;

; Make tickcount a C-accessible value
.export _tickcount
_tickcount = tickcount

; We need the OAM address
.segment "OAM"
oam:

.segment "CODE"

; ------------------------------------------------------------------------
; System V-Blank Interrupt
; Updates PPU Memory (buffered).
; Updates VBLANK_FLAG and tickcount.
; ------------------------------------------------------------------------

nmi:    pha
        tya
        pha
        txa
        pha

        lda     #1
        sta     VBLANK_FLAG

        inc     tickcount
        bne     @s
        inc     tickcount+1

        ; Flush PPU instructions
@s:     jsr     ppubuf_flush

        ; Reset the video counter.
        lda     #$20
        sta     PPU_VRAM_ADDR2
        lda     #$00
        sta     PPU_VRAM_ADDR2

        jsr     _finvsync

        ; Transfer sprite info
        lda     #>oam
        sta     APU_SPR_DMA

        pla
        tax
        pla
        tay
        pla

; Interrupt exit

irq:
        rti


; ------------------------------------------------------------------------
; hardware vectors
; ------------------------------------------------------------------------

.segment "VECTORS"

        .word   nmi         ; $fffa vblank nmi
        .word   start       ; $fffc reset
        .word   irq         ; $fffe irq / brk


; jump table, and more importantly, the symbol for it
.segment "JUMPS"
_jumpTable:
