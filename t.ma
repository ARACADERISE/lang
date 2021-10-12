use 1000
org 100

; We want 200 bytes of stack memory
; note: the stack in moca assembly does not grow downwards.
set bp, 200

set ax, 0x10
set bx, 100
set dx, 100
set cx, 100

sect data {
  name db 'hey'
  value dw 0x50
}