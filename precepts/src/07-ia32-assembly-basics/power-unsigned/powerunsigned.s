### --------------------------------------------------------------------
### powerunsigned.s
### Author: Bob Dondero
### Control transfer instructions for unsigned types
### --------------------------------------------------------------------

        .section ".rodata"

cPrompt1:
        .asciz  "Enter the base:  "

cPrompt2:
        .asciz  "Enter the exponent:  "

cScanfFormat:
        .asciz  "%u"

cResult:
        .asciz  "%u to the %u power is %u.\n"

### --------------------------------------------------------------------

        .section ".data"

uiPower:
        .long   1

### --------------------------------------------------------------------

        .section ".bss"

uiBase:
        .skip   4

uiExp:
        .skip   4

uiIndex:
        .skip   4

### --------------------------------------------------------------------

        .section ".text"

        ## -------------------------------------------------------------
        ## int main(void)
        ## Read a non-negative base and exponent from stdin.  Write
        ## base raised to the exponent power to stdout.  Return 0.
        ## -------------------------------------------------------------

        .globl  main
        .type   main,@function
main:

        pushl   %ebp
        movl    %esp, %ebp

        ## printf("Enter the base:  ")
        pushl   $cPrompt1
        call    printf
        addl    $4, %esp

        ## scanf("%d", &uiBase)
        pushl   $uiBase
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## printf("Enter the exponent:  ")
        pushl   $cPrompt2
        call    printf
        addl    $4, %esp

        ## scanf("%d", &uiExp)
        pushl   $uiExp
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## uiIndex = 1
        movl    $1, uiIndex

loop1:

        ## if (uiIndex > uiExp) goto loopend1
        movl    uiIndex, %eax
        cmpl    uiExp, %eax
        ja      loopend1

        ## uiPower *= uiBase
        movl    uiPower, %eax
        mull    uiBase
        movl    %eax, uiPower

        ## uiIndex++
        incl    uiIndex

        ## goto loop1
        jmp     loop1

loopend1:

        ## printf("%u to the %u power is %u.\n", uiBase, uiExp, uiPower)
        pushl   uiPower
        pushl   uiExp
        pushl   uiBase
        pushl   $cResult
        call    printf
        addl    $16, %esp

        ## return 0
        movl    $0, %eax
        movl    %ebp, %esp
        popl    %ebp
        ret
