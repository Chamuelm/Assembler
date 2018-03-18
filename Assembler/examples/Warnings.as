; Tester by: Moshe Hamiel

; Invalid number of operands (more than 2)
	mov S1.1, W, r3

; Invalid number of operands (more than 1)
	not r2, r3

; Invalid number of operands (more than 0)
	rts W

; Invalid number of operands (less than 2)
	mov W

; Invalid number of operands (less than 1)
	not

; Missing comma ',' between parameters
	mov S1.1 W

; Variable does not exist (data)
	mov banana, W

; Variable does not exist (struct)
	mov S3.2, W

; Variable does not exist (string)
	mov STR3, W

; Invalid struct name
	mov S1.3, W

; Invalid argument name (e.g. start with digit / invalid character
	mov 12W, W
	mov WX%, W

; Invalid source addressing type
	lea #25, W

; Invalid destination addressing type
	mov W, #25

; Invalid comma before parameter
	mov , W,W

; Invalid comma before command 
	, mov S1.1, W

; Exceeded comma between parameters - invalid parameter
	mov S1.1 ,, W

; Exceeded comma after parameters
	mov S1.1, W,

; Colon without label name
:	.data 14

; Label in entry line
MINION:	.entry LOOP, LENGTH

; Label in extern line
MINION: .extern W5

; Label exceeded max length allowed
abcdefghijklmnopqrstuvwxyz123456789:	.data 22

; Label is keyword
mov:	.data 12

; Label in empty line
W4: 

; Label start with digit
3W:	.data 22

; Label have invalid character
W5#:	.data 22

; Invalid parameter - Invalid number
	.data -

; Invalid parameter - Invalid character in number
	.data 1@5

; Invalid parameter - number is not in program hardware limit (-255 to 256)
	.data 375

; Invalid parameter - missing number
	mov #, W

; Line number exceeded maximum length
               ABCDEFGHIJKLMNOPQRSTUVWXYZ123:       mov         #1      ,       W

; Unknown command
	shalom W, W1

; Extra text after instruction
	mov #2, W hi

; Expected " in end of line
	.string "banana

; Missing string
	.string 
  
; String is not in qoutation mark
	.string banana

; External is marked as entry
	.extern L8
	.entry L8






.entry LOOP
.entry LENGTH
.extern L3
.extern W
MAIN:	mov S1.1, W
                add r2,STR
LOOP:       jmp W
                prn #-5
                sub     r1, r4
                 inc K

                    mov S1.2, r3
                    bne L3
END:            stop
STR:        .string "abcdef"
LENGTH:     .data   6,-9,15
K:                    .data 22
S1:         .struct 8, "ab"
