/*
Assignment1: wc209
Name: Jeon Sangeon
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define OUT_WORD 0
#define IN_WORD 1
#define START_COM_OUT 2
#define START_COM_IN 3
#define IN_COM 4
#define END_COM 5

int c;	
int state=OUT_WORD;
int nLetr=0;
int nWord=0;
int nLine=0;
int err_line=0;

/* CORRECTED: 내부 helper는 static, 무인자는 (void)로 명시한다.
   원래 DFA 전이와 줄/단어/문자 계산은 유지했다. */
static void out_word(void){
//Handle state when it comes to OUT_WORD
	if(isspace(c)) nLetr++;
	else if(c=='/') nLetr++, state=START_COM_OUT;
	else nLetr++, nWord++, state=IN_WORD;
}

static void in_word(void){
//Handle state when it comes to IN_WORD
	if(isspace(c)) nLetr++, state=OUT_WORD;
	else if(c=='/') nLetr++, state=START_COM_IN;
	else nLetr++;
}

static void start_com_out(void){
//Handle state when it comes to START_COM_OUT
	if(isspace(c)) nLetr++,nWord++, state=OUT_WORD;
	else if(c=='*') err_line=nLine,	state=IN_COM;
	else if(c=='/') nLetr++, nWord++, state=START_COM_IN;
	else nLetr++,nWord++, state=IN_WORD;
}

static void start_com_in(void){
//Handle state when it comes to START_COM_IN
	if(isspace(c)) nLetr++, state=OUT_WORD;
	else if(c=='*') err_line=nLine,	state=IN_COM;
	else if(c=='/') nLetr++;
	else nLetr++, state=IN_WORD;
}

static void in_com(void){
//Handle state when it comes to IN_COM
	if(c=='*') state=END_COM;
	else if(c=='\n') nLetr++;
	/* CORRECTED: 주석 내부의 나머지 문자는 의도적으로 버린다. */
}

static void end_com(void){
//Handle state when it comes to END_COM
	if(c=='/') state=OUT_WORD;
	else if(c=='\n') nLetr++, state=IN_COM;
	else if(c=='*');
	else state=IN_COM;
}

static void state_changing(void){
/*
Changing DFAstate once according to c and state
Glo var: c, state, nLine, nWord, nLine, errLine
*/
	if(c=='\n') nLine++;
	switch(state){
		case OUT_WORD:
			out_word();
			break;
		case IN_WORD:
			in_word();
			break;
		case START_COM_OUT:
			start_com_out();
			break;
		case START_COM_IN:
			start_com_in();
			break;
		case IN_COM:
			in_com();
			break;
		case END_COM:
			end_com();
			break;
	}
}

static int terminating(void){
/*
About the terminating condition and the error
Return whether the program terminate properly
Glo var: state, nLine, nWord, nLine, errLine
Write the result on the stdout or the error on the stderr
*/
	switch(state){
		case OUT_WORD:
		case IN_WORD:
		case START_COM_IN:
			break;
		case START_COM_OUT:
			nWord++;
			break;
		case IN_COM:
		case END_COM:
			fprintf(stderr,"Error: line %d: unterminated comment\n",err_line);
			return EXIT_FAILURE;
	}

	fprintf(stdout,"%d %d %d\n",nLine,nWord,nLetr);
	return EXIT_SUCCESS;
}

int main(void){
/*
Test whether the input is empty
Changes state according to input char
*/
	if((c=getchar())!=EOF) nLine++, state_changing();
	while((c=getchar())!=EOF) state_changing();
	/* CORRECTED: EOF와 실제 stdin 읽기 실패를 구분한다. */
	if (ferror(stdin)) {
		fprintf(stderr, "Error: cannot read standard input\n");
		return EXIT_FAILURE;
	}
	return terminating();
}

	
