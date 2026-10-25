### --------------------------------------------------------------------
### Assignment4 mydc.s
###
### 20180569 Jeon Sangeon
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	
.section ".rodata"

# WRONG: 폭 제한 없는 %s가 20바이트 buffer를 넘겨 쓴다. 긴 입력도 끝까지 소비하며 범위를 검사해야 한다.
scanfFormat:
	.asciz "%s"
stackEmpty:
	.asciz "dc:stack empty\n"
displayFormat:
	.asciz "%d\n"
### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:

# WRONG: main이 사용하는 EBX/ESI는 callee-saved 레지스터다. 호출자에게 돌아가기 전에 저장한 원래 값을 복구해야 한다.
	pushl   %ebp
	movl    %esp, %ebp

input:

	## dc number stack initialized. %esp = %ebp
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp

	## check if user input EOF
	cmp	$EOF, %eax
	je	quit

########################################################
########################################################
########################################################
	## check if user input digit
	
	movl	$0, %ebx
	movzbl	buffer(,%ebx,1), %ecx
	pushl	%ecx
	call 	isdigit
	## %eax: isdigit
	addl	$4, %esp
# WRONG: ECX는 caller-saved라 isdigit 호출 후 원래 문자가 보장되지 않는다. 호출 뒤 buffer에서 다시 읽거나 호출 전에 보존해야 한다.
	## %ecx: first letter

	cmp	$0, %eax
	jne	digit

#########################################################
#########################################################

##Operaters Switch
	## user input '_'
	cmp	$95, %ecx
	je	inputneg
	
	## user input 'p'
	cmp	$112, %ecx
	je	inputp	

	## user input 'q'
	cmp	$113, %ecx
	je	quit
	
	## user input '+'
	cmp	$43, %ecx
	je	inputplus
	
	## user input '-'
	cmp	$45, %ecx
	je	inputminus

	## user input '*'
	cmp	$42, %ecx
	je	inputmul

	## user input '/'
	cmp	$47, %ecx
	je	inputdivq

	## user input '%'
	cmp	$37, %ecx
	je	inputdivr

	## user input '^'
	cmp	$94, %ecx
	je	inputexp

	## user input 'f'
	cmp	$102, %ecx
	je	inputf

	## user input 'c'
	cmp	$99, %ecx
	je	inputc
	
	## user input 'd'
	cmp	$100, %ecx
	je	inputd
	
	## user input 'r'
	cmp	$114, %ecx
	je	inputr

	## user input wrong
	jmp	input

#########################################################
#########################################################

##Tool funcs

twoOpds:
## Pops 1st opd in %ebx,
##      2nd opd in %eax
	movl	(%esp), %ecx
	## %ecx: old eip
	addl	$4, %esp
	cmp	%esp, %ebp
	je	inputEmpty1
	popl 	%ebx
	## %ebx: opd1

	cmp	%esp, %ebp
	je	inputEmpty2
	popl	%eax
	## %eax: opd2

	pushl	%ecx
	ret
#########################################################
inputEmpty1:
## When 1st opd is empty
	pushl	$stackEmpty
	call 	printf
	addl	$4, %esp
	jmp	input
#########################################################
inputEmpty2:
## When 2nd opd is empty
	subl	$4, %esp
	pushl	$stackEmpty
	call 	printf
	addl	$4, %esp
	jmp	input

#########################################################
#########################################################

##Store funcs

digit:
## user input digit
	pushl	$buffer
# WRONG: atoi는 12abc도 12로 바꾼다. PDF가 요구하는 혼합 영숫자 무시는 입력 전체를 검사해야 한다.
	call	atoi
	addl	$4, %esp
	## %eax: input int
	pushl	%eax
	jmp 	input

#########################################################
#########################################################

##Calculate funcs

inputneg:
## user input '_'
	movl	$1, %eax
	movzbl	buffer(,%eax,1), %ebx
	## %ebx: second letter

	pushl	%ebx
	call 	isdigit
	addl	$4, %esp
	cmp	$0, %eax
	je	input

	movl	$1, %ebx
	addl	$buffer, %ebx
	## %ebx: addr of 2nd letter
	pushl	%ebx
# WRONG: atoi는 12abc도 12로 바꾼다. PDF가 요구하는 혼합 영숫자 무시는 입력 전체를 검사해야 한다.
	call	atoi
	## %eax: pos val
	addl	$4, %esp
	negl	%eax
	## %eax: neg val

	pushl	%eax
	jmp	input
#########################################################
inputplus:
## user input '+'
	call	twoOpds
	## %ebx: opd1
	## %eax: opd2
	addl	%ebx, %eax
	## %eax: opd2+opd1
	pushl	%eax
	jmp 	input
#########################################################
inputminus:
## user input '-'
	call	twoOpds
	## %ebx: opd1
	## %eax: opd2
	subl	%ebx, %eax
	## %eax: opd2-opd1
	pushl	%eax
	jmp 	input
#########################################################
inputmul:
## user input '*'
	call	twoOpds
	## %ebx: opd1
	## %eax: opd2
	movl	$0, %edx
	imull	%ebx
	## %eax: opd2*opd1
	pushl	%eax
	jmp input
#########################################################
inputdivq:
## user input '/'
	call	twoOpds
	## %ebx: opd1
	## %eax: opd2
	cmp	$0, %eax
	jl	inputdivq1
	movl	$0, %edx
	idivl	%ebx
	## %eax: opd2/opd1
	pushl	%eax
	jmp 	input
# WRONG: 음수 피제수를 neg로 양수화하면 INT_MIN은 여전히 음수다. cltd로 EDX:EAX를 부호 확장한 뒤 idiv해야 한다.
inputdivq1:
	negl	%eax
	movl	$0, %edx
	idivl	%ebx
	## %eax: opd2/opd1
	negl	%eax
	## %eax: opd2/opd1
	pushl	%eax
	jmp	input
#########################################################
# WRONG: 음수 피연산자를 무시하면서 두 값은 이미 pop되어 사라진다. signed remainder도 cltd/idiv로 계산해야 한다.
inputdivr:
## user input '%'
	call	twoOpds
	## %ebx: opd1
	## %eax: opd2
	cmp	$0, %eax
	jl	input
	cmp	$0, %ebx
	jl	input
	movl	$0, %edx
	idivl	%ebx
	## %edx: opd2%opd1
	pushl	%edx
	jmp input

#########################################################
# WRONG: 지수 0도 곱셈 루프에 들어가 -1,-2,...로 계속 감소한다. 0을 먼저 처리하고 PDF 요구대로 별도 power 함수를 만들어야 한다.
inputexp:
## user input '^'
	call	twoOpds
	## %ebx: exponent
	## %eax: base

	cmp	$0, %ebx
	jl	input

	movl	%ebx, %esi
	movl	%eax, %ebx
	movl	$1, %eax
	## %esi: exponent
	## %ebx: base
	## %eax: return val
inputexp1:
	movl	$0, %edx
	imull	%ebx
	## %eax= %eax*%ebx
	subl	$1, %esi
	cmp	$0, %esi
	jne	inputexp1

	pushl	%eax
	jmp	input
	
#########################################################
#########################################################

##Advanced funcs

inputp:
## user input 'p'
	cmp	%esp, %ebp
	je	inputEmpty1
	pushl	$displayFormat
	call	printf
	addl	$4, %esp
	jmp	input
#########################################################
# WRONG: f는 빈 스택에서 아무것도 하지 않아야 한다. 여기서는 stack empty를 출력한다.
inputf:
## user input 'f'
	cmp	%esp, %ebp
	je	inputEmpty1
	movl	%esp, %ebx
	## %ebx: a val of %esp
inputf1:
	pushl	(%ebx)
	pushl	$displayFormat
	call	printf
	addl	$8, %esp
	addl	$4, %ebx
	cmp	%ebx, %ebp
	jne	inputf1
	jmp	input
#########################################################
# WRONG: c도 빈 스택을 정상적으로 지우면 된다. 빈 경우 오류를 출력하면 PDF 요구와 다르다.
inputc:
## user input 'c'
	cmp	%esp, %ebp
	je	inputEmpty1
inputc1:
	cmp	%esp ,%ebp
	je	input
	addl	$4, %esp
	jmp	inputc1
#########################################################
inputd:
	cmp	%esp, %ebp
	je	inputEmpty1
	pushl	(%esp)
	jmp	input
#########################################################
inputr:
## user input 'r'
	cmp	%esp, %ebp
	je	inputEmpty1
	addl	$4, %esp

	cmp	%esp, %ebp
	je	inputEmpty2
	subl	$4, %esp	
# WRONG: r은 상위 두 값만 교환해야 한다. 아래 루프는 스택 전체를 뒤집어 세 값 이상일 때 결과가 틀린다.
inputr1:
	movl	%esp, %eax
	movl	%esp, %esi
	movl	%ebp, %esp
	subl	$4, %esp
	## %esp: from bottom to top
	## %eax: from top to bottom
	## %esi: return point
inputr2:
	cmp	%esp, %eax
	jge	inputr3
	movl	(%esp), %ebx
	movl	(%eax), %ecx
	## %ebx: temp restore for bottom
	## %ecx: temp restore for top
	movl	%ecx, (%esp)
	movl	%ebx, (%eax)
	addl	$4, %eax
	subl	$4, %esp
	jmp	inputr2
inputr3:
	movl	%esi, %esp
	jmp	input

#########################################################
#########################################################
#########################################################
quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret
