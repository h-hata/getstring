shell:token.o rule.o shell.c
	gcc -o shell -D MAIN -Wconversion -Wall shell.c token.o rule.o
.c.o:
	gcc -Wall -Wconversion  -c $< -ggdb
clean:
	rm -fr *.o tags core* shell

