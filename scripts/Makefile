

all: ${patsubst %.msk,%.o,${wildcard *_*.msk}}

%.o: %.msk
	@echo building $<...
	@../main $< >$<.log
	g++ $@ lib.cpp -o $*_test
	
clean:
	rm -f *.o
