all: first

first: first.c
	gcc -g -Wall -Werror -fsanitize=address cacheSimulator.c -lm -o cacheSimulator

clean:
	rm -f first
