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
; Startup code for cc65 (BNROM version, bank >= 1)
;
; by Yahweasel
; based on code by Groepaz/Hitmen <groepaz@gmx.net>
; based on code by Ullrich von Bassewitz <uz@cc65.org>
; See cc65: https://cc65.github.io/
;

        .export         _exit
        .export         _bank_switch
        .export         bank_switch
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
        .export         _jumpTable
        .import         donelib
        .import         push0, popa
        .import         ppubuf_flush, _finvsync

        ; Linker generated symbols
        .import         __RAM_START__, __RAM_SIZE__
        .import         __SRAM_START__, __SRAM_SIZE__
        .import         __ROM0_START__, __ROM0_SIZE__
        .import         __STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__
        .import         __CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__
        .import         __RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__

        .include        "zeropage.inc"
        .include        "nes.inc"


; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment        "STARTUP"

start:

; Set up the CPU and System-IRQ.

        sei
        cld
        ldx     #0

; Make sure we're on the right bank
        stx     $8000
        jmp     start

; Remaining startup code is only in bank 0

.include "bankswitch-gen-OWN_BANK.s"

_exit:  jsr     donelib         ; Run module destructors

; Reset the NES.

        jmp start


.include "nmi.s"
