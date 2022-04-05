all: set-freezer forktest

set-freezer: set-freezer.c
	gcc -o set-freezer set-freezer.c

forktest: forktest.c
	gcc -o forktest forktest.c

clean:
	rm -rf set-freezer forktest

.PHONY: $(PHONY)