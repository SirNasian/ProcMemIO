CC      = gcc
INCLUDE = include

linux : lib/procmemio/libprocmemio.so lib/procmemio/libprocmemio.a
windows : lib/procmemio/procmemio.dll

lib/procmemio/libprocmemio.a : obj/linux.o
	@mkdir -p lib/procmemio
	ar rcs $@ $^

lib/procmemio/libprocmemio.so : obj/linux.o
	@mkdir -p lib/procmemio
	$(CC) -shared -o $@ -I$(INCLUDE) -fPIC $<

lib/procmemio/procmemio.dll : src/windows.c
	@mkdir -p lib/procmemio
	$(CC) -shared -o $@ -I$(INCLUDE) -fPIC $<

obj/linux.o : src/linux.c
	@mkdir -p obj
	$(CC) -o $@ -I$(INCLUDE) -c $<

clean :
	rm -rf lib obj

.PHONY : linux windows
