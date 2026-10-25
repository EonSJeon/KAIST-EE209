#include <stdio.h>
#include <stdlib.h>
#include "str.h"

#define FIND_STR        "-f"
#define REPLACE_STR     "-r"
#define DIFF_STR        "-d"

#define MAX_STR_LEN 1023

#define FALSE 0
#define TRUE  1

/*
Assignment2 sgrep.c
20180569 Jeon Sangeon
*/

typedef enum {
  INVALID,
  FIND,
  REPLACE,
  DIFF
} CommandType;
/*--------------------------------------------------------------------*/
/* PrintUsage()
   print out the usage of the Simple Grep Program                     */
/*--------------------------------------------------------------------*/
void 
PrintUsage(const char* argv0) 
{
  const static char *fmt = 
    "Simple Grep (sgrep) Usage:\n"
    "%s [COMMAND] [OPTIONS]...\n"
    "\nCOMMNAD\n"
    "\tFind: -f [search-string]\n"
    "\tReplace: -r [string1] [string2]\n"
    "\tDiff: -d [file1] [file2]\n";

  /* CORRECTED: 사용법도 오류 경로에서는 stderr로 보낸다. */
  fprintf(stderr, fmt, argv0);
}
/*-------------------------------------------------------------------*/
/* DoFind()
   1. Argument validation
   - File argument length is longer than 1023B-> return False
   2. Input line validation
   - Input lines is longer than 1023B-> return False
   3. Check & print out the lines contain a given string (search-string)
      -> return True if there's no problem*/
/*-------------------------------------------------------------------*/
int
DoFind(const char *pcSearch)
{
  char buf[MAX_STR_LEN + 2]; 

  if (StrGetLength(pcSearch)>MAX_STR_LEN){
	fprintf(stderr, "Error: argument is too long\n");
	return FALSE;
  }

  while (fgets(buf, sizeof(buf), stdin)) {
    	if (StrGetLength(buf) > MAX_STR_LEN) {
      		fprintf(stderr, "Error: input line is too long\n");
      		return FALSE;
    	}
	if(StrSearch(buf,pcSearch)) printf("%s",buf);
  }
  /* CORRECTED: 입력 오류를 정상 EOF로 취급하지 않는다. */
  if (ferror(stdin)) {
    fprintf(stderr, "Error: failed to read standard input\n");
    return FALSE;
  }
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoReplace()
   1. Argument validation
   - File argument length is longer than 1023B-> return False
   - If word1 is an empty string -> return False
   2. Input line validation
   - Input lines is longer than 1023B-> return False
   3. Replace the string and print out the replaced string
      ->return True if there's no problem*/
/*-------------------------------------------------------------------*/
int
DoReplace(const char *pcString1, const char *pcString2)
{
  char buf[MAX_STR_LEN + 2];
  char* bufEnd;
  char* str1FoundPt;
  /* CORRECTED: 길이는 반환형과 같은 size_t로 유지한다. */
  size_t len1=StrGetLength(pcString1);
  size_t len2=StrGetLength(pcString2);

  if (len1>MAX_STR_LEN || len2>MAX_STR_LEN){
	fprintf(stderr, "Error: argument is too long\n");
	return FALSE;
  }
  if (!*pcString1){
	fprintf(stderr, "Error: Can't replace an empty substring\n");
	return FALSE;
  }

  while (fgets(buf, sizeof(buf), stdin)) {
	bufEnd=buf, str1FoundPt=buf;
    	if (StrGetLength(buf) > MAX_STR_LEN) {
      		fprintf(stderr, "Error: input line is too long\n");
      		return FALSE;
    	}
	while((str1FoundPt=StrSearch(bufEnd,pcString1))){
		while(bufEnd<str1FoundPt){
			printf("%c",*bufEnd);
			bufEnd++;
		}
		printf("%s",pcString2);
		bufEnd=str1FoundPt+len1;
	}
	printf("%s",bufEnd);
  }
  /* CORRECTED: 입력 오류를 정상 EOF로 취급하지 않는다. */
  if (ferror(stdin)) {
    fprintf(stderr, "Error: failed to read standard input\n");
    return FALSE;
  }
  return TRUE;
}
/*-------------------------------------------------------------------*/
/* DoDiff()
   1. Argument validation
   - File argument length is longer than 1023B-> return False
   2. Input line validation
   - Input lines is longer than 1023B-> return False
   3. Print out any different line with the following format
      file1@linenumber:file1's line
      file2@linenumber:file2's line
   4. One of the files ends earlier than the other-> return False
   ->return True if there's no problem*/
/*-------------------------------------------------------------------*/
int
DoDiff(const char *fn1, const char *fn2)
{
  FILE*fp1, *fp2;
  char buf1 [MAX_STR_LEN+2];
  char buf2 [MAX_STR_LEN+2];
  size_t len1, len2;
  int line=0;
  int isempty1,isempty2;
  int result=FALSE;

  if(StrGetLength(fn1)>MAX_STR_LEN || StrGetLength(fn2)>MAX_STR_LEN){
	fprintf(stderr,"Error: argument is too long\n"); return FALSE;
  }
  if(!(fp1=fopen(fn1,"r"))){
	fprintf(stderr,"Error: failed to open file %s\n",fn1); return FALSE;
  }
  if(!(fp2=fopen(fn2,"r"))){
	fprintf(stderr,"Error: failed to open file %s\n",fn2);
        /* CORRECTED: 두 번째 열기 실패도 첫 FILE을 닫는다. */
        fclose(fp1);
        return FALSE;
  }

  while(1) {
	isempty1=!fgets(buf1, sizeof(buf1), fp1); isempty2=!fgets(buf2, sizeof(buf2), fp2);
        /* CORRECTED: 읽기 실패를 확인한 뒤 EOF를 판정한다. */
	if (ferror(fp1) || ferror(fp2)) {
                fprintf(stderr, "Error: failed to read input file\n");
                break;
        }
	if(isempty1&&isempty2) { result=TRUE; break; }
        /* CORRECTED: EOF 오류도 현재 검사하는 줄 번호를 쓴다. */
	line++;
	if (isempty1){
		fprintf(stderr,"Error: %s ends early at line %d\n",fn1,line);
		break;
	}
	else if (isempty2){
		fprintf(stderr,"Error: %s ends early at line %d\n",fn2,line);
		break;
	}

	if((len1=StrGetLength(buf1))>MAX_STR_LEN){
		fprintf(stderr,"Error: input line %s is too long\n",fn1);
                break;
	}
	if((len2=StrGetLength(buf2))>MAX_STR_LEN){
		fprintf(stderr,"Error: input line %s is too long\n",fn2);
                break;
	}

	if(buf1[len1-1]=='\n') buf1[len1-1]='\0';
	if(buf2[len2-1]=='\n') buf2[len2-1]='\0';
	if(StrCompare(buf1,buf2)){
                /* CORRECTED: PDF 형식은 파일명@숫자:내용이다. */
		printf("%s@%d:%s\n",fn1,line,buf1);
		printf("%s@%d:%s\n",fn2,line,buf2);
	}
  }
  /* CORRECTED: 성공/오류 모두 한 경로에서 두 FILE을 닫는다. */
  fclose(fp1);
  fclose(fp2);
  return result;
}
/*-------------------------------------------------------------------*/
/* CommandCheck() 
   - Parse the command and check number of argument. 
   - It returns the command type number
   - This function only checks number of argument. 
   - If the unknown function is given or the number of argument is 
   different from required number, this function returns FALSE.
   
   Note: You SHOULD check the argument rule later                    */
/*-------------------------------------------------------------------*/ 
int
CommandCheck(const int argc, const char *argv1)
{
  int cmdtype = INVALID;
   
  /* check minimum number of argument */
  if (argc < 3)
    return cmdtype;
   
  /* check command type */ 
  if (StrCompare(argv1, FIND_STR) == 0) {
    if (argc != 3)
      return FALSE;    
    cmdtype = FIND;       
  }
  else if (StrCompare(argv1, REPLACE_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = REPLACE;
  }
  else if (StrCompare(argv1, DIFF_STR) == 0) {
    if (argc != 4)
      return FALSE;
    cmdtype = DIFF;
  }
  else return FALSE;
   
  return cmdtype;
}
/*-------------------------------------------------------------------*/
int 
main(const int argc, const char *argv[]) 
{
  int type, ret=FALSE;
   
  /* Do argument check and parsing */
  if (argc < 2 || !(type = CommandCheck(argc, argv[1]))) {
    fprintf(stderr, "Error: argument parsing error\n");
    PrintUsage(argv[0]);
    return (EXIT_FAILURE);
  }
   
  /* Do appropriate job */
  switch (type) {
  case FIND:
    ret = DoFind(argv[2]);
    break;
  case REPLACE:
    ret = DoReplace(argv[2], argv[3]);
    break;
  case DIFF:
    ret = DoDiff(argv[2], argv[3]);
    break;
  } 

  return (ret)? EXIT_SUCCESS : EXIT_FAILURE;
}
