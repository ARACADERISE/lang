use 1000
org 100

; We want 200 bytes of stack memory
; note: the stack in moca assembly does not grow downwards.
set bp, 200

sect data {
  name db 'hey'
}