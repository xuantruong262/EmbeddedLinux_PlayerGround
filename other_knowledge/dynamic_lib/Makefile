OBJ	=	main.o \
		cal.o

CFLAGS = -Wall -Wextra -fPIC -I.
all: main

main: main.o libcal.so
	gcc main.o -L. -lcal -o main

libcal.so: cal.o
	gcc -shared -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

run:
	LD_LIBRARY_PATH=. ./main
clean:
	rm -rf *o main *.so
