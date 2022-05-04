INCLUDE_DIRS = 
LIB_DIRS = 
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS) -lm
LIBS= 
CPPLIBS= -L/usr/lib -lopencv_core -lopencv_flann -lopencv_video

HFILES= 
CFILES= seqgen.cpp

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	seqgen



clean:
	-rm -f *.o *.d
	-rm -f seqgen 


seqgen: seqgen.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o -lpthread -lrt `pkg-config --libs opencv` $(CPPLIBS)


	
depend:

.c.o:
	$(CC) $(CFLAGS) -c $<