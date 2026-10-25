### --------------------------------------------------------------------
### hello.s
### Original Author: Bob Dondero
### Assembly language fundamentals
### --------------------------------------------------------------------

        .section ".rodata"

cGreeting:
        .asciz  "hello, world\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"

### --------------------------------------------------------------------

        .section ".text"

        ## -------------------------------------------------------------
        ## int main(void)
        ## Write "hello, world\n" to stdout.  Return 0.
        ## -------------------------------------------------------------

        .globl  main
        .type   main,@function

main:

        pushl   %ebp
        movl    %esp, %ebp

        ## printf("hello, world\n")
        pushl   $cGreeting
        call    printf
        addl    $4, %esp

        ## return 0
        movl    $0, %eax
        movl    %ebp, %esp
        popl    %ebp
        ret
