all : parentcreates childcreates checkpasswd

parentcreates : parentcreates.c
	gcc -Wall --std=gnu99 -o parentcreates parentcreates.c

childcreates : childcreates.c
	gcc -Wall --std=gnu99 -o childcreates childcreates.c

checkpasswd : checkpasswd.c
	gcc -Wall --std=gnu99 -o checkpasswd checkpasswd.c