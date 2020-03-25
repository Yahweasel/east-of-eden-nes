.export _farcall1, _farcall2
.export _gfarcall0, _gfarcall1, _gfarcall2

; Generic farcall when the last argument is the bank number and the function to
; be called is function 0
.import bank_switch, popa

_farcall1:
_farcall2:
        ldx #0
        jmp bank_switch


; Generic farcall: Last argument is the index to call, second to last is the
; bank
_gfarcall0:
_gfarcall1:
_gfarcall2:
        tax
        jsr popa
        jmp bank_switch
