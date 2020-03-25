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
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Adjustments by Yahweasel, just changing values to make it fit in vblank for
; this code
;


        .export         ppubuf_waitempty
        .export         ppubuf_wait
        .export         ppubuf_put
        .export         ppubuf_flush
        .include        "nes.inc"

.code

; ------------------------------------------------------------------------
; ppubuf_waitempty
; Wait until buffer is empty

.proc   ppubuf_waitempty


@wait:  lda     ringcount
        bne     @wait
        rts

.endproc


; ------------------------------------------------------------------------
; ppubuf_wait
; Wait until buffer is full

.proc   ppubuf_wait

        lda     #$ff            ; (($0100/3)*1)
@wait:  cmp     ringcount
        beq     @wait
        rts

.endproc

; ------------------------------------------------------------------------
; Put a PPU-Memory write to buffer
; called from main program (not necessary when in vblank irq)

.proc   ppubuf_put

        sta     ppuval
        sty     ppuhi
        stx     ppulo

        jsr     ppubuf_wait             ; wait if buffer is full

        ldy     ringwrite
        lda     ppuhi
        sta     ringbuff,y
        lda     ppulo
        sta     ringbuff+$0100,y
        lda     ppuval
        sta     ringbuff+$0200,y

        iny
        sty     ringwrite
        inc     ringcount
        rts

.endproc

; ------------------------------------------------------------------------
; Flush PPU-Memory write buffer
; called from vblank interupt

.proc   ppubuf_flush

        ldy     ringcount
        bne     @doloop
        rts

@doloop:
        ldx     ringread
        lda     #$0d                ; #$0d calculated by inundating the buffer and looking for artifacts
        sta     temp

@loop:
.repeat 5
        lda     ringbuff,x
        sta     $2006
        lda     ringbuff+$0100,x
        sta     $2006
        lda     ringbuff+$0200,x
        sta     $2007
        inx

        dey
        beq     @end
.endrepeat

        dec     temp
        bne     @loop

@end:   stx     ringread
        sty     ringcount

        rts     

.endproc

; ------------------------------------------------------------------------
; Data

.bss

temp:   .res    1


