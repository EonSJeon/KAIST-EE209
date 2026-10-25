# WRONG: 현재 macOS에는 수업용 gcc209가 없으므로 이 설정만으로 빌드되지 않는다. cc 기본값과 명시적 덮어쓰기를 지원해야 한다.
CC= gcc209
CCFLAGS= -D_BSD_SOURCE -D_GNU_SOURCE

all:ish
clobber: clean
	rm -f *~ \#*\# core
clean:
	rm -f ish *.o

ish: dynarray.o ish.o
	$(CC) $(CCFLAGS) dynarray.o ish.o -o ish

dynarray.o: dynarray.c dynarray.h
	$(CC) $(CCFLAGS) -c dynarray.c

ish.o: ish.c dynarray.h
	$(CC) $(CCFLAGS) -c ish.c
