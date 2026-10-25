### --------------------------------------------------------------------
### power.s
### Author: Bob Dondero
### Control transfer instructions
### --------------------------------------------------------------------

        .section ".rodata"

cPrompt1:
        .asciz  "Enter the base:  "

cPrompt2:
        .asciz  "Enter the exponent:  "

cScanfFormat:
        .asciz  "%d"

cResult:
        .asciz  "%d to the %d power is %d.\n"

### --------------------------------------------------------------------

        .section ".data"

iPower:
        .long   1

### --------------------------------------------------------------------

        .section ".bss"

iBase:
        .skip   4

iExp:
        .skip   4

iIndex:
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

        ## scanf("%d", &iBase)
        pushl   $iBase
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## printf("Enter the exponent:  ")
        pushl   $cPrompt2
        call    printf
        addl    $4, %esp

        ## scanf("%d", &iExp)
        pushl   $iExp
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## iIndex = 1
        movl    $1, iIndex

loop1:

        ## if (iIndex > iExp) goto loopend1
        movl    iIndex, %eax
        cmpl    iExp, %eax
        jg      loopend1

        ## iPower *= iBase
        movl    iPower, %eax
        imull   iBase
        movl    %eax, iPower

        ## iIndex++
        incl    iIndex

        ## goto loop1
        jmp     loop1

loopend1:

        ## printf("%d to the %d power is %d.\n", iBase, iExp, iPower)
        pushl   iPower
        pushl   iExp
        pushl   iBase
        pushl   $cResult
        call    printf
        addl    $16, %esp

        ## return 0
        movl    $0, %eax
        movl    %ebp, %esp
        popl    %ebp
        ret
