.export chrs

.segment "RODATA"
chrs:
        .incbin "icons1.b0.chr" ; 2 pages
        .incbin "nes.char.chr"  ; 6 pages
        .incbin "title.b1.chr"  ; 8 pages
        .incbin "back1.b1.chr"  ; 8 pages
        .incbin "ff3a.b1.chr"   ; 8 pages
        .incbin "ff3b.b1.chr"   ; 8 pages
        .incbin "ff3c.b1.chr"   ; 8 pages
        .incbin "ff3d.b1.chr"   ; 8 pages
        .incbin "spr1.spr.chr"  ; 16 pages
