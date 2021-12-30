s.o: stack.h stack.c
	gcc -c stack.c

driver: expand.c stack.o 
	gcc expand.c stack.o -lc -o expand

clean:
	rm -f *.o expand