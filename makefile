CC      = gcc
INCLUDE = include

linux : lib/libprocmemio.so lib/libprocmemio.a
windows : lib/procmemio.dll

lib/libprocmemio.a : obj/linux.o
	@mkdir -p lib
	ar rcs $@ $^

lib/libprocmemio.so : obj/linux.o
	@mkdir -p lib
	$(CC) -shared -o $@ -I$(INCLUDE) -fPIC $<

lib/procmemio.dll : src/windows.c
	@mkdir -p lib
	$(CC) -shared -o $@ -I$(INCLUDE) -fPIC $<

obj/linux.o : src/linux.c
	@mkdir -p obj
	$(CC) -o $@ -I$(INCLUDE) -c $<

clean :
	rm -rf lib obj

.PHONY : linux windows
