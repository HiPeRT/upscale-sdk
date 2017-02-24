CFLAGS += -O0 -g3
# CFLAGS += -Wall
# CFLAGS += -DDEBUG
#CFLAGS+=-Wl,-kill-at
LDFLAGS += -lm
# CSRCS=dealer.c sched.c
CSRCS=psoc_mapper.c sched.c dag.c
EXENAME=psoc_mapper
LIBNAME=$(EXENAME).dll
#CC=gcc per MingW
all:
	$(CC) $(CSRCS) $(CFLAGS) -o $(EXENAME) $(LDFLAGS)

install: all
	mkdir -p $(PREFIX)/bin
	cp psoc_mapper $(PREFIX)/bin
 
# uninstall:
# 	rm -f $(PREFIX)/bin/psoc_mapper

clean:
	rm -f $(EXENAME) $(LIBNAME)

run:
	./$(EXENAME) -f -a regression-tests/14_test-melani1.dot regression-tests/15_test-melani2.dot

ARCHFLAGS=-m64
# HGFLAGS=-I"/cygdrive/c/Program Files/Java/jdk1.8.0_73/include" -I"/cygdrive/c/Program Files/Java/jdk1.8.0_73/include/win32"
HGFLAGS=-I"C:\Program Files\Java\jdk1.8.0_73\include" -I"C:\Program Files\Java\jdk1.8.0_73\include\win32" -DHGT
DEPLOYDIR="D:\UniMoRe\Workspace\hipert.hg\lib"
# Form MinGW > mingw32-make.exe lib
lib:
	gcc ${ARCHFLAGS} ${HGFLAGS} -Wall -c jni_wrapper.c -o jni_wrapper.o
	gcc ${ARCHFLAGS} ${CFLAGS} -c sched.c -o sched.o
	gcc ${ARCHFLAGS} ${CFLAGS} -c dag.c -o dag.o
	gcc ${ARCHFLAGS} ${CFLAGS} -c psoc_mapper.c -o psoc_mapper.o
	gcc ${ARCHFLAGS} -Wl,-kill-at -shared -o $(LIBNAME) psoc_mapper.o dag.o sched.o jni_wrapper.o
	cp ${LIBNAME} ${DEPLOYDIR}

libdump:
	objdump -D $(LIBNAME) > $(LIBNAME).dis