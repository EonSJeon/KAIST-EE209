### --------------------------------------------------------------------
### powerfunction.s
### Author: Bob Dondero
### Functions
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

### --------------------------------------------------------------------

        .section ".bss"

### --------------------------------------------------------------------

        .section ".text"

        ## -------------------------------------------------------------
        ## int power(int iBase, int iExp)
        ## Return iBase raised to the iExp power, where iBase and iExp
        ## are non-negative.
        ## -------------------------------------------------------------

        ## Formal parameter offsets:
        .equ IBASE,    8
        .equ IEXP,     12

        ## Local variable offsets:
        .equ IPOWER,  -4
        .equ IINDEX,  -8

        .type   power,@function

power:

        pushl   %ebp
        movl    %esp, %ebp

        ## int iPower = 1
        pushl   $1

        ## int iIndex
        subl    $4, %esp

        ## iIndex = 1
        movl    $1, IINDEX(%ebp)

loop1:

        ## if (iIndex > iExp) goto loopend1
        movl    IINDEX(%ebp), %eax
        cmpl    IEXP(%ebp), %eax
        jg      loopend1

        ## iPower *= iBase
        movl    IPOWER(%ebp), %eax
        imull   IBASE(%ebp)
        movl    %eax, IPOWER(%ebp)

        ## iIndex++
        incl    IINDEX(%ebp)

        ## goto loop1
        jmp     loop1

loopend1:

        ## return iPower
        movl    IPOWER(%ebp), %eax
        movl    %ebp, %esp
        popl    %ebp
        ret

        ## -------------------------------------------------------------
        ## int main(void)
        ## Read a non-negative base and exponent from stdin.  Write
        ## base raised to the exponent power to stdout.  Return 0.
        ## -------------------------------------------------------------

        ## Local variable offsets:
        .equ IBASE,   -4
        .equ IEXP,    -8
        .equ IPOWER, -12

        .globl  main
        .type   main,@function

main:

        pushl   %ebp
        movl    %esp, %ebp

        ## int iBase
        subl    $4, %esp

        ## int iExp
        subl    $4, %esp

        ## int iPower
        subl    $4, %esp

        ## printf("Enter the base:  ")
        pushl   $cPrompt1
        call    printf
        addl    $4, %esp

        ## scanf("%d", &iBase)
        leal    IBASE(%ebp), %eax
        ## Alternative to leal:
        ##    movl %ebp,%eax
        ##    addl $IBASE,%eax
        pushl   %eax
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## printf("Enter the exponent:  ")
        pushl   $cPrompt2
        call    printf
        addl    $4, %esp

        ## scanf("%d", &iExp)
        leal    IEXP(%ebp), %eax
        ## Alternative to leal:
        ##    movl %ebp,%eax
        ##    addl $IEXP,%eax
        pushl   %eax
        pushl   $cScanfFormat
        call    scanf
        addl    $8, %esp

        ## iPower = power(iBase, iExp)
        pushl   IEXP(%ebp)
        pushl   IBASE(%ebp)
        call    power
        addl    $8, %esp
        movl    %eax, IPOWER(%ebp)

        ## printf("%d to the %d power is %d.\n", iBase, iExp, iPower)
        pushl   IPOWER(%ebp)
        pushl   IEXP(%ebp)
        pushl   IBASE(%ebp)
        pushl   $cResult
        call    printf
        addl    $16, %esp

        ## return 0
        movl    $0, %eax
        movl    %ebp, %esp
        popl    %ebp
        ret
