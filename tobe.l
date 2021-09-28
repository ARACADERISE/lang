; declare how much "memory" we want
use 1500
; set the origin of the program
org 100 ; this sets the program "binary" to start 100 bytes in.

; block of code, explicitly say how much "memory" we want this block to use.
; blocks will be useful when we have a hefty portion of code to write, and want to use memory efficiently.
block: size = 50 {

}

; "global" code, limited to as much memory as you assigned.

; a container is a sequence of operations that do not affect the overall "memory" usage.
; containers require a name.
; use containers when you want to "store" code that you might, or might not, use.
; Containters follow rules of global/block code. It is primarily a secondary "file" within the current file.
container: MyContainer {

}