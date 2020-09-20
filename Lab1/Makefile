#my_malloc : my_malloc.c
#	$(CC) $(CFLAGS) -Wno-deprecated-declarations -o my_malloc my_malloc.c
#
malloc_driver : malloc_driver.c my_malloc.c
	$(CC) $(CFLAGS) -Wno-deprecated-declarations -o malloc_driver malloc_driver.c my_malloc.c