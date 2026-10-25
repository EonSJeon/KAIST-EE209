# CORRECTED: Assignment 4: mydc.s, Jeon Sangeon.
# CORRECTED: Corrected by hand from the original ESP-based calculator.
# CORRECTED: EDI marks the empty operand stack; main restores all saved registers.
        .equ ARRAYSIZE, 1024
        .equ EOF, -1
        .equ ARG1, 8
        .equ ARG2, 12
        .section .rodata
stackEmpty:    .asciz "dc: stack empty\n"
divideZero:    .asciz "dc: divide by zero\n"
displayFormat: .asciz "%d\n"
messageFormat: .asciz "%s"
        .section .bss
buffer: .skip ARRAYSIZE
        .section .text
        .globl main
        .type main,@function
# CORRECTED: Read stdin tokens, dispatch operations, return 0 on q or EOF.
main:
        pushl %ebp
        movl %esp, %ebp
        pushl %ebx
        pushl %esi
        pushl %edi
        movl %esp, %edi
input:
        call readToken
        cmpl $EOF, %eax
        je quit
        testl %eax, %eax
        jz input
        call parseNumber
        testl %edx, %edx
        jnz digit
        cmpb $0, buffer+1
        jne input
        movzbl buffer, %ecx
        cmpl $112, %ecx                 # CORRECTED: p
        je inputp
        cmpl $113, %ecx                 # CORRECTED: q
        je quit
        cmpl $43, %ecx
        je inputplus
        cmpl $45, %ecx
        je inputminus
        cmpl $42, %ecx
        je inputmul
        cmpl $47, %ecx
        je inputdivq
        cmpl $37, %ecx
        je inputdivr
        cmpl $94, %ecx
        je inputexp
        cmpl $102, %ecx                 # CORRECTED: f
        je inputf
        cmpl $99, %ecx                  # CORRECTED: c
        je inputc
        cmpl $100, %ecx                 # CORRECTED: d
        je inputd
        cmpl $114, %ecx                 # CORRECTED: r
        je inputr
        jmp input

digit:
        pushl %eax
        jmp input

# CORRECTED: Peek two operands without changing the data stack. CF=1 on shortage;
# CORRECTED: otherwise EBX=top (right operand), EAX=next (left operand), CF=0.
        .type twoOpds,@function
twoOpds:
        leal 12(%esp), %ecx             # CORRECTED: includes return address
        cmpl %edi, %ecx
        ja twoMissing
        movl 4(%esp), %ebx
        movl 8(%esp), %eax
        clc
        ret
twoMissing:
        stc
        ret
inputEmpty1:
        pushl $stackEmpty
        call printMessage
        addl $4, %esp
        jmp input
inputplus:
        call twoOpds
        jc inputEmpty1
        addl %ebx, %eax
        jmp storeResult
inputminus:
        call twoOpds
        jc inputEmpty1
        subl %ebx, %eax
        jmp storeResult
inputmul:
        call twoOpds
        jc inputEmpty1
        imull %ebx, %eax
        jmp storeResult
inputdivq:
        call twoOpds
        jc inputEmpty1
        testl %ebx, %ebx
        jz inputZero
        # CORRECTED: INT_MIN/-1 overflows idiv; wrap without a processor trap.
        cmpl $0x80000000, %eax
        jne divqNormal
        cmpl $-1, %ebx
        je storeResult
divqNormal:
        cltd                            # CORRECTED: sign-extend EAX into EDX:EAX
        idivl %ebx
        jmp storeResult
inputdivr:
        call twoOpds
        jc inputEmpty1
        testl %ebx, %ebx
        jz inputZero
        cmpl $0x80000000, %eax
        jne divrNormal
        cmpl $-1, %ebx
        jne divrNormal
        xorl %eax, %eax
        jmp storeResult
divrNormal:
        cltd
        idivl %ebx
        movl %edx, %eax
        jmp storeResult
inputZero:
        pushl $divideZero
        call printMessage
        addl $4, %esp
        jmp input
inputexp:
        call twoOpds
        jc inputEmpty1
        testl %ebx, %ebx
        js input                        # CORRECTED: negative exponent: leave intact
        pushl %ebx
        pushl %eax
        call power
        addl $8, %esp
storeResult:
        addl $8, %esp
        pushl %eax
        jmp input

inputp:
        cmpl %esp, %edi
        je inputEmpty1
        pushl (%esp)
        call printValue
        addl $4, %esp
        jmp input
inputf:
        movl %esp, %ebx
inputf1:
        cmpl %edi, %ebx
        je input
        pushl (%ebx)
        call printValue
        addl $4, %esp
        addl $4, %ebx
        jmp inputf1
inputc:
        movl %edi, %esp
        jmp input
inputd:
        cmpl %esp, %edi
        je inputEmpty1
        pushl (%esp)
        jmp input
inputr:
        call twoOpds
        jc inputEmpty1
        movl %eax, (%esp)
        movl %ebx, 4(%esp)
        jmp input
quit:
        xorl %eax, %eax
        leal -12(%ebp), %esp
        popl %edi
        popl %esi
        popl %ebx
        popl %ebp
        ret

# CORRECTED: Return base^exp in EAX, exp>=0; 0^0=1. No I/O. No saved-register
# CORRECTED: changes. Squaring handles exponent zero and large exponents quickly.
        .type power,@function
power:
        pushl %ebp
        movl %esp, %ebp
        movl ARG1(%ebp), %edx
        movl ARG2(%ebp), %ecx
        movl $1, %eax
powerLoop:
        testl %ecx, %ecx
        jz powerDone
        testl $1, %ecx
        jz powerSquare
        imull %edx, %eax
powerSquare:
        shrl $1, %ecx
        jz powerDone
        imull %edx, %edx
        jmp powerLoop
powerDone:
        popl %ebp
        ret

# CORRECTED: Print ARG1 as an integer / message. Each libc call has a 16-byte
# CORRECTED: aligned stack regardless of how many operands main has pushed.
        .type printValue,@function
printValue:
        pushl %ebp
        movl %esp, %ebp
        andl $-16, %esp
        subl $8, %esp
        pushl ARG1(%ebp)
        pushl $displayFormat
        call printf
        movl %ebp, %esp
        popl %ebp
        ret
        .type printMessage,@function
printMessage:
        pushl %ebp
        movl %esp, %ebp
        andl $-16, %esp
        subl $8, %esp
        pushl ARG1(%ebp)
        pushl $messageFormat
        call printf
        movl %ebp, %esp
        popl %ebp
        ret

# CORRECTED: Read one complete whitespace-delimited token into buffer. Return
# CORRECTED: 1 for a token, 0 for an overlong token (consumed fully), -1 for EOF.
# CORRECTED: EBX=len, ESI=overflow; preserve EBX/ESI/EDI across this call.
        .type readToken,@function
readToken:
        pushl %ebp
        movl %esp, %ebp
        pushl %ebx
        pushl %esi
        andl $-16, %esp
        xorl %ebx, %ebx
        xorl %esi, %esi
readNext:
        call getchar
        cmpl $EOF, %eax
        je readEnd
        cmpl $32, %eax
        je readSpace
        cmpl $9, %eax
        jb readChar
        cmpl $13, %eax
        jbe readSpace
readChar:
        cmpl $ARRAYSIZE-1, %ebx
        jae readOverflow
        movb %al, buffer(%ebx)
        incl %ebx
        jmp readNext
readOverflow:
        movl $1, %esi
        jmp readNext
readSpace:
        testl %ebx, %ebx
        jz readNext
readEnd:
        testl %ebx, %ebx
        jz readReturn                   # CORRECTED: EAX=-1: no token at EOF
        movb $0, buffer(%ebx)
        movl $1, %eax
        testl %esi, %esi
        jz readReturn
        xorl %eax, %eax
readReturn:
        leal -8(%ebp), %esp
        popl %esi
        popl %ebx
        popl %ebp
        ret

# CORRECTED: Parse the ENTIRE buffer as digits or _digits. EDX=1 on success,
# CORRECTED: EAX=value. Reject mixed text and values outside signed 32-bit range.
# CORRECTED: No I/O; preserve EBX/ESI/EDI, including main's stack boundary.
        .type parseNumber,@function
parseNumber:
        pushl %ebx
        pushl %esi
        pushl %edi
        movl $buffer, %ebx
        xorl %esi, %esi
        xorl %ecx, %ecx
        cmpb $95, (%ebx)
        jne numberStart
        incl %ecx
        incl %ebx
numberStart:
        cmpb $0, (%ebx)
        je numberInvalid
numberLoop:
        movzbl (%ebx), %edi
        testl %edi, %edi
        jz numberDone
        subl $48, %edi
        cmpl $9, %edi
        ja numberInvalid
        cmpl $214748364, %esi
        ja numberInvalid
        jne numberAccumulate
        leal 7(%ecx), %eax
        cmpl %eax, %edi
        ja numberInvalid
numberAccumulate:
        imull $10, %esi
        addl %edi, %esi
        incl %ebx
        jmp numberLoop
numberDone:
        movl %esi, %eax
        testl %ecx, %ecx
        jz numberValid
        negl %eax
numberValid:
        movl $1, %edx
        jmp numberReturn
numberInvalid:
        xorl %edx, %edx
numberReturn:
        popl %edi
        popl %esi
        popl %ebx
        ret
        .section .note.GNU-stack,"",@progbits
