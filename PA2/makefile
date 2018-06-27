all : cycle myfind compress_file uncompress_file

cycle : cycle.c
	gcc --std=gnu99 -Wall -o cycle cycle.c

myfind : myfind.c
	gcc --std=gnu99 -Wall -o myfind myfind.c

compress_file: compress_file.c
	gcc --std=gnu99 -Wall -o compress_file compress_file.c

uncompress_file: uncompress_file.c
	gcc --std=gnu99 -Wall -o uncompress_file uncompress_file.c

.PHONY: clean

clean : 
	find . -maxdepth 1 -type f -perm -100 -delete 