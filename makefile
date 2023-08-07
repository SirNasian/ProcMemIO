CC      = gcc
INCLUDE = include

lib/procmemio/libprocmemio.so : src/linux.c
	@mkdir -p lib/procmemio
	$(CC) -shared -o $@ -I$(INCLUDE) -fPIC $<

lib/procmemio/procmemio.dll : src/windows.c
	@mkdir -p lib/procmemio
	$(CC) -shared -o $@ -I$(INCLUDE) -fPIC $<

clean :
	rm -rf lib
