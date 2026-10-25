/*--------------------------------------------------------------------*/
/* ish.c          Assign5                                             */
/* Original Author: Bob Dondero                                       */
/* Modified to shell: Jeon Sangeon 20180569                           */
/*--------------------------------------------------------------------*/

#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*--------------------------------------------------------------------*/

#define READ 0
#define WRITE 1
#define CHILD 0

enum {MAX_LINE_SIZE = 1024};

enum {FALSE, TRUE};

enum TokenType {TOKEN_ORDINARY, TOKEN_SPECIAL};

enum CommandType {USER, SETENV, UNSETENV, CD, EXIT};

/* WRONG: 전역 oTokens를 해제 후 NULL로 만들지 않아 시그널 핸들러에서 다시 free할 수 있다. 줄 단위 지역 소유권으로 관리해야 한다. */
DynArray_T oTokens;

/*--------------------------------------------------------------------*/

/* A Token is either a number or a word, expressed as a string. */

struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/

static void freeToken(void *pvItem, void *pvExtra)

/* Free token pvItem.  pvExtra is unused. */

{
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/

static struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}
/*--------------------------------------------------------------------*/

/* WRONG: DynArray_add 실패 시 이미 만든 Token을 해제하지 않아 메모리가 누수된다. */
static int makeAddToken(enum TokenType eTokenType,char *acValue, DynArray_T oTokens)
/* Make and add token to the oTokens*/
{
	struct Token *psToken;

	psToken=makeToken(eTokenType,acValue);
	if (psToken == NULL){
		fprintf(stderr, "Cannot allocate memory\n");
		return FALSE;
	}
	if (! DynArray_add(oTokens, psToken)){
                fprintf(stderr, "Cannot allocate memory\n");
                return FALSE;
        }
	return TRUE;
}
/*--------------------------------------------------------------------*/
//Lexical Analysis
/*--------------------------------------------------------------------*/

static char* lexLine(const char *pcLine, DynArray_T oTokens)
/*Perform lexical analysis.
 Return error message if any,
 return otherwise NULL*/

{
   	enum LexState {OUT_WORD, IN_WORD, IN_QUOTE, END_QUOTE};
   	enum {ALLOC_ERR,QUOTE_ERR};
	char* LexError[2]={"Cannot allocate memory",
					"Could not find quote pair"};

   	enum LexState eState = OUT_WORD;

   	int iLineIndex = 0;
   	int iValueIndex = 0;
/* WRONG: 음수 char를 isspace에 넘기면 정의되지 않은 동작이다. 입력 바이트는 unsigned char로 변환해야 한다. */
   	char c;
   	char acValue[MAX_LINE_SIZE];
	
  	assert(pcLine != NULL);
   	assert(oTokens != NULL);

  	 for (;;)
   	{
      		c = pcLine[iLineIndex++];
      		switch (eState)
      		{
         		case OUT_WORD:
            			if ((c == '\n') || (c == '\0')) return NULL;
	    			else if (c=='|'){
					acValue[0]='|';
					acValue[1]='\0';
					if(!makeAddToken(TOKEN_SPECIAL,acValue,oTokens))
						return LexError[ALLOC_ERR];
					iValueIndex = 0;
	    			}
	    			else if (c=='"') eState=IN_QUOTE;
	    			else if (!isspace(c)){
					acValue[iValueIndex++] = c;
					eState=IN_WORD;
	    			}
            			break;
	
        		 case IN_WORD:
	    			if (isspace(c)){
					acValue[iValueIndex] = '\0';
					if(!makeAddToken(TOKEN_ORDINARY,acValue,oTokens))
						return LexError[ALLOC_ERR];
         				iValueIndex = 0;
					eState=OUT_WORD;
	    			}
            			else if ((c == '\n') || (c == '\0')){
               				/* Create a NUMBER token. */
               				acValue[iValueIndex] = '\0';
               				if(!makeAddToken(TOKEN_ORDINARY,acValue,oTokens))
						return LexError[ALLOC_ERR];
               				iValueIndex = 0;
               				return NULL;
            			}
	    			else if (c=='|'){
					acValue[iValueIndex] = '\0';
               				if(!makeAddToken(TOKEN_ORDINARY,acValue,oTokens))
						return LexError[ALLOC_ERR];
               				acValue[0]='|';
					acValue[1]='\0';
					if(!makeAddToken(TOKEN_SPECIAL,acValue,oTokens))
						return LexError[ALLOC_ERR];
               				iValueIndex = 0;
	       				eState=OUT_WORD;
	    			}
	    			else if (c=='"') eState=IN_QUOTE;
	    			else acValue[iValueIndex++] = c;
           			break;

         		case IN_QUOTE:
            			if ((c == '\n') || (c == '\0')) return LexError[QUOTE_ERR];
	    			else if (c=='"') eState=END_QUOTE;
            			else acValue[iValueIndex++] = c;
            			break;
	
/* WRONG: 닫힌 따옴표 다음 공백 경로에서 iValueIndex를 0으로 만들지 않는다. 다음 단어에 앞 단어의 내용이 섞이며 인접 따옴표도 잘못 처리한다. */
	 		case END_QUOTE:
	    			if (isspace(c)){
	       				acValue[iValueIndex] = '\0';
               				if(!makeAddToken(TOKEN_ORDINARY,acValue,oTokens))
						return LexError[ALLOC_ERR];
	       				eState=OUT_WORD;
	    			}
	    			else if ((c == '\n') || (c == '\0')){
	       				acValue[iValueIndex] = '\0';
               				if(!makeAddToken(TOKEN_ORDINARY,acValue,oTokens))
						return LexError[ALLOC_ERR];
	       				return NULL;
	    			}
	    			else if (c=='|'){
	       				acValue[iValueIndex] = '\0';
               				if(!makeAddToken(TOKEN_ORDINARY,acValue,oTokens))
						return LexError[ALLOC_ERR];
               				acValue[0]='|';
					acValue[1]='\0';
					if(!makeAddToken(TOKEN_SPECIAL,acValue,oTokens))
						return LexError[ALLOC_ERR];
               				iValueIndex = 0;
	       				eState=OUT_WORD;
	    			}
	   			else if (c=='"') acValue[iValueIndex++] = '"';
	    			else{
	       				acValue[iValueIndex++] = c;
	       				eState=IN_WORD;
	    			}
      		}
   	}
}

/*--------------------------------------------------------------------*/
//Syntactic Analysis
/*--------------------------------------------------------------------*/
enum CommandType commandType(void* pcItem)
/*Examine whether pcItem is setenv, unsetenv, cd, exit*/
{
	char* builtInCommand[4]={"setenv","unsetenv","cd","exit"};
	char* psValue=((struct Token*)pcItem)->pcValue;
	int i;
	for(i=0;i<4;i++) if(strcmp(psValue,builtInCommand[i])==0) return i+1;
	return 0;
}
/*--------------------------------------------------------------------*/
char* synLine(DynArray_T oTokens)
/*Perform syntactic analysis.
 Return error message if any,
 return otherwise NULL*/
{
	enum {MISS_COM_ERR, PIPE_ERR, SETENV_ERR, UNSETENV_ERR, CD_ERR, EXIT_ERR};
	char* SynError[6]={"Missing command name",
			   "Pipe or redirection destination not spcified",
			   "setenv takes one or two parameters",
			   "unsetenv takes one parameter",
			   "cd takes one parameter",
			   "exit does not take any parameters"};

	int len=DynArray_getLength(oTokens);
	int i;

/* WRONG: 공백만 있는 줄은 토큰이 0개다. 인덱스 0 접근 전에 빈 목록을 정상 처리하지 않으면 assert로 종료된다. */
	struct Token* firstToken=(struct Token*)DynArray_get(oTokens,0);
	int TokenType= firstToken->eType;
	int firstCommandType;

	if(TokenType==TOKEN_SPECIAL) return SynError[MISS_COM_ERR];
	firstCommandType=commandType(firstToken);
	if(firstCommandType!=USER){
		switch (firstCommandType){
			case SETENV:
				if(len!=2 && len!=3)
					return SynError[SETENV_ERR];
				break;
			case UNSETENV:
				if(len!=2)
					return SynError[UNSETENV_ERR];
				break;
			case CD:
				if(len!=1 && len!=2)
					return SynError[CD_ERR];
				break;
			case EXIT:
				if(len!=1) return SynError[EXIT_ERR];
				else return NULL;
		}
		for(i=1;i<len;i++){
			struct Token* tempToken=(struct Token*)DynArray_get(oTokens,i);
			int TokenType= tempToken->eType;
			if(TokenType==TOKEN_SPECIAL) return SynError[PIPE_ERR];
		}
		return NULL;
	}
	else{
		int afterPipe=0;
		for(i=1;i<len;i++){
			struct Token* tempToken=(struct Token*)DynArray_get(oTokens,i);
			int TokenType= tempToken->eType;
			if(afterPipe&&TokenType!=TOKEN_SPECIAL) afterPipe=0;
			else if(afterPipe && TokenType==TOKEN_SPECIAL)
				return SynError[PIPE_ERR];
			else if(!afterPipe && TokenType==TOKEN_SPECIAL) afterPipe=1;
		}
		if(afterPipe) return SynError[PIPE_ERR];
		else return NULL;
	}
}
/*--------------------------------------------------------------------*/
//Execution
/*--------------------------------------------------------------------*/
int unitCommandExtractor(DynArray_T oTokens, int len, int startIdx, char*** pgmArgs){
/*Extract sequence of tokens between Tokens*/
	struct Token* tempToken=NULL;
		
	if(startIdx==-1) return -1;
	int pipeIdx;
	for(pipeIdx=startIdx;pipeIdx<len;pipeIdx++){
		tempToken=(struct Token*)DynArray_get(oTokens,pipeIdx);
		int TokenType= tempToken->eType;
		if(TokenType==TOKEN_SPECIAL) break;
	}

/* WRONG: 인자 개수보다 작게 할당하며 execvp가 요구하는 마지막 NULL도 없다. 필요한 개수+1을 할당하고 NULL로 끝내야 한다. */
	*pgmArgs=(char**)malloc((pipeIdx-startIdx-1)*sizeof(char*));
	int i=0;
	int j=startIdx;
	for(;j<pipeIdx;i++,j++){
		tempToken=(struct Token*)DynArray_get(oTokens,j);
		(*pgmArgs)[i]=tempToken->pcValue;
	}
	if(pipeIdx>=len) return -1;
	else return pipeIdx+1;
}
/*--------------------------------------------------------------------*/
int terminating(const char * Error, const char * pgmName){
/* Print the error and make sure that pgm end without memory leak*/
	if(Error){
		fprintf(stderr, "%s: %s\n", pgmName,Error);
	}
	if(oTokens){
		DynArray_map(oTokens, freeToken, NULL);
      		DynArray_free(oTokens);
	}
	return 0;
}
/*--------------------------------------------------------------------*/
int singleLine(char* acLine, char * argv0){
/* Excute single line
   Do lexical and syntatical analysis
   and execute each command sequencially*/
	char** pgmArgs=NULL;
	char *Error=NULL;
	int status;

	if(!strcmp(acLine,"\n") || !strcmp(acLine,"")) return 0;

	oTokens = DynArray_new(0);
	if (oTokens == NULL){
		Error="Cannot allocate memory\n";
		terminating(Error,argv0);
       		return 0;
	}

	if((Error=lexLine(acLine, oTokens))!=NULL){
		terminating(Error,argv0);
		return 0;
	}

	if((Error=synLine(oTokens))!=NULL){
		terminating(Error,argv0);
		return 0;
	}

	int len=DynArray_getLength(oTokens);
	struct Token* tempToken=(struct Token*)DynArray_get(oTokens,0);
	int firstCommandType=commandType(tempToken);
	switch(firstCommandType){
		case USER:
		{
			int nextStartIdx=0;
			int* pipes1=(int*)malloc(2*sizeof(int));
			int* pipes2=(int*)malloc(2*sizeof(int));
			int pid;
			nextStartIdx=unitCommandExtractor(oTokens,len,nextStartIdx,&pgmArgs);
			if(nextStartIdx==-1){
				fflush(NULL);
				pid=fork();
				if(pid==-1) assert(0);
				else if(pid==CHILD){
					signal(SIGALRM,SIG_DFL);
					signal(SIGINT,SIG_DFL);
					signal(SIGQUIT,SIG_DFL);
					if(execvp(pgmArgs[0],pgmArgs)==-1){
						terminating("Cannot execute program",argv0);
/* WRONG: exec 실패는 사용자 오류다. abort 대신 errno를 보고하고 해당 자식만 _exit해야 한다. */
						raise(SIGABRT);
					}
					assert(0);
				}
				else{
/* WRONG: 단일 외부 명령 성공 경로가 토큰/pgmArgs/pipes를 해제하지 않고 switch를 빠져나가 반복 실행 때 누수된다. */
					wait(NULL);
					break;
				}
			}

/* WRONG: pipe/fork 오류 검사가 부족하다. 부모와 각 자식의 불필요한 read/write FD가 남아 EOF가 전달되지 않고 파이프라인이 멈출 수 있다. */
			pipe(pipes1);
			fflush(NULL);
			pid=fork();
			if(pid==-1) assert(0);
			else if(pid==CHILD){
				signal(SIGALRM,SIG_DFL);
				signal(SIGINT,SIG_DFL);
				signal(SIGQUIT,SIG_DFL);
				close(WRITE); dup(pipes1[WRITE]); close(pipes1[WRITE]);
				if(execvp(pgmArgs[0],pgmArgs)==-1){
					close(WRITE);
					terminating("Cannot execute program",argv0);
					raise(SIGABRT);
				}
				assert(0);
			}
			nextStartIdx=unitCommandExtractor(oTokens,len,nextStartIdx,&pgmArgs);

			while(nextStartIdx!=-1){
				pipes2=(int*)malloc(2*sizeof(int));
				pipe(pipes2);
				fflush(NULL);pid=fork();
				if(pid==-1) assert(0);
				else if(pid==CHILD){
					signal(SIGALRM,SIG_DFL);
					signal(SIGINT,SIG_DFL);
					signal(SIGQUIT,SIG_DFL);
					close(READ); dup(pipes1[READ]); close(pipes1[READ]);
					close(WRITE); dup(pipes2[WRITE]); close(pipes2[WRITE]);
					if(execvp(pgmArgs[0],pgmArgs)==-1){
						close(READ); close(WRITE);
						terminating("Cannot execute program",argv0);
/* WRONG: exec 실패는 사용자 오류다. abort 대신 errno를 보고하고 해당 자식만 _exit해야 한다. */
						raise(SIGABRT);
					}
					assert(0);
				}
				free(pipes1);
				pipes1=pipes2;
				nextStartIdx=unitCommandExtractor(oTokens,len,nextStartIdx,&pgmArgs);
					
			}
			fflush(NULL);
			close(pipes1[WRITE]);
			pid=fork();
			if(pid==-1) assert(0);
			else if(pid==CHILD){
				signal(SIGALRM,SIG_DFL);
				signal(SIGINT,SIG_DFL);
				signal(SIGQUIT,SIG_DFL);
				close(READ); dup(pipes1[READ]); close(pipes1[READ]);
				if(execvp(pgmArgs[0],pgmArgs)==-1){
					close(READ);
					terminating("Cannot execute program",argv0);
					raise(SIGABRT);
				}
				assert(0);
			}
			
			while(wait(&status)>0){
				if(status==134){
/* WRONG: 자식 오류 때문에 프로세스 그룹에 SIGINT를 보내면 셸/다른 프로세스까지 영향을 줄 수 있다. 자신이 만든 PID만 정확히 wait해야 한다. */
					kill(-getpid(),SIGINT);
					break;
				}
			}
			terminating(NULL,argv0);
			break;
		}
		case SETENV:
			unitCommandExtractor(oTokens,len,0,&pgmArgs);
			if(len==2) setenv(pgmArgs[1],"",1);
			else if(len==3)	setenv(pgmArgs[1],pgmArgs[2],1);
			terminating(NULL,argv0);
			break;
		case UNSETENV:
			unitCommandExtractor(oTokens,len,0,&pgmArgs);
			unsetenv(pgmArgs[1]);
			terminating(NULL,argv0);
			break;
		case CD:
			unitCommandExtractor(oTokens,len,0,&pgmArgs);
			if(len==1 && chdir(getenv("HOME"))==-1) assert(0);
			else if(len==2 && chdir(pgmArgs[1])==-1)
				terminating("No such file or directory",argv0);
			else terminating(NULL,argv0);
			break;
		case EXIT:
			exit(0);
			break;
	}
	return 0;
}
/*--------------------------------------------------------------------*/
//Signal
/*--------------------------------------------------------------------*/
static void sigquitHandler1(int iSignal)
{
/*When receive SIGQUIT for the second time within 5 seconds*/
/* WRONG: 시그널 핸들러에서 free/stdio/exit를 호출하는 것은 async-signal-safe하지 않다. 플래그와 write/alarm만 사용하고 정상 흐름에서 정리해야 한다. */
	if(oTokens) terminating(NULL,NULL);
	exit(0);
}
/*--------------------------------------------------------------------*/
static void sigquitHandler(int iSignal)
{
/*When receive SIGQUIT for the first time*/
	printf("Type Ctrl-\\ again within 5 seconds to exit\n");
	signal(SIGQUIT,sigquitHandler1);
	alarm(5);
	return;
}

/*--------------------------------------------------------------------*/
static void sigalrmHandler(int iSignal){
/*When user does not enter Ctrl-\ within 5 seconds*/
	signal(SIGQUIT,sigquitHandler);
	return;
}
/*--------------------------------------------------------------------*/
//main
/*--------------------------------------------------------------------*/
int main(int argc,char **argv)

/* Read a line from stdin, and write to stdout each number and word
   that it contains.  Repeat until EOF.  Return 0 iff successful. */

{
	signal(SIGINT,SIG_IGN);
	sigset_t sSet;
	sigemptyset(&sSet);
	sigaddset(&sSet,SIGINT);
	sigaddset(&sSet,SIGQUIT);
	sigaddset(&sSet,SIGALRM);
	sigprocmask(SIG_UNBLOCK,&sSet,NULL);

	signal(SIGALRM,sigalrmHandler);
	signal(SIGQUIT,sigquitHandler);

	char acLine[MAX_LINE_SIZE];
	dup(0);//restore stdin from fd=3
	dup(1);//restore stdout from fd=4
	
/* WRONG: 시작 파일은 현재 디렉터리가 아니라 HOME/.ishrc에서 찾아야 한다. 연 스트림도 닫아야 한다. */
	FILE* firstinput=fopen(".ishrc","r");
	if(firstinput!=NULL)
		while(fgets(acLine, MAX_LINE_SIZE, firstinput)!=NULL){
			printf("%% %s",acLine);
			singleLine(acLine,argv[0]);
		}

   	while(1){
		printf("%% ");
/* WRONG: EOF 반환을 검사하지 않아 Ctrl-D 뒤 이전 명령을 무한 반복한다. 긴 줄의 나머지도 하나의 잘못된 줄로 소비해야 한다. */
		fgets(acLine, MAX_LINE_SIZE, stdin);
		singleLine(acLine,argv[0]);
	}
	return 0;
}
