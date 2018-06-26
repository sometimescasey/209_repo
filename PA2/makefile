all : cycle myfind

cycle : cycle.c
	gcc --std=gnu99 -Wall -o cycle cycle.c

myfind : myfind.c
	gcc --std=gnu99 -Wall -o myfind myfind.c

.PHONY: clean

clean : 
	find . -maxdepth 1 -type f -perm -100 -delete 