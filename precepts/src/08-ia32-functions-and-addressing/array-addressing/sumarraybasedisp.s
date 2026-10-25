### --------------------------------------------------------------------
### sumarraybasedisp.s
### Author: Bob Dondero
### Base + Displacement Addressing
### --------------------------------------------------------------------

        .equ    ARRAYSIZE, 100

### --------------------------------------------------------------------

        .section ".rodata"

cPrompt:
        .asciz  "How many integers?  "
cScanfFormat:
        .asciz  "%d"
cResult:
        .asciz  "The sum is %d.\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"

iNumbers:
        .skip   4 * ARRAYSIZE
iIndex:
        .skip   4
iCount:
        .skip   4
iSum:
        .skip   4

### --------------------------------------------------------------------

        .section ".text"

        ## -------------------------------------------------------------
        ## int main(void)
        ## Read up to ARRAYSIZE integers from stdin, and write to
        ## stdout the sum of those integers.  Return 0.
        ## -------------------------------------------------------------

        .globl  main
        .type   main,@function

main:

        pushl   %ebp
        movl    %esp, %ebp

        ## iIndex = 0
        movl    $0, iIndex

        ## printf("How many integers?  ")
        pushl   $cPrompt
        call    printf
        addl    $4, %esp

        ## scanf("%d", &iCount)
        pushl   $iCount
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

loop1:

        ## if (iIndex >= iCount) goto loopend1
        movl    iIndex, %eax
        cmpl    iCount, %eax
        jge     loopend1

        ## scanf("%d", &aiNumbers[iIndex])
        movl    iIndex, %eax
        sall    $2, %eax
        addl    $iNumbers, %eax
        pushl   %eax
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## iIndex++
        incl    iIndex

        ## goto loop1
        jmp     loop1

loopend1:

        ## iSum = 0
        movl    $0, iSum

        ## iIndex = 0
        movl    $0, iIndex

loop2:

        ## if (iIndex >= iCount) goto loopend2
        movl    iIndex, %eax
        cmpl    iCount, %eax
        jge     loopend2

        ## iSum += aiNumbers[iIndex]
        movl    iIndex, %eax
        sall    $2, %eax
        movl    iNumbers(%eax), %eax # base+displacement addressing
        addl    %eax, iSum

        ## iIndex++
        incl    iIndex

        ## goto loop2
        jmp     loop2

loopend2:

        ## printf("The sum is %d.\n", iSum)
        pushl   iSum
        pushl   $cResult
        call    printf
        addl    $8, %esp

        ## return 0
        movl    $0, %eax
        movl    %ebp, %esp
        popl    %ebp
        ret
