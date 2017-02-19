SLC = slc -b l2mt_f
MAXSIZE = 512
BLOCKSIZES = 1,2,4,8,16,30
PROGRAMS = ca1d rgb2gray-fp rgb2gray-int
all: $(PROGRAMS:%=%$(MAXSIZE))

%$(MAXSIZE): %.c bench.c
	$(SLC) -o $@ $< -DMAXSIZE=$(MAXSIZE) -DBLOCKSIZES="$(BLOCKSIZES)"

clean:
	rm -f $(PROGRAMS:%=%[0-9]*)
	rm -f *.o *~ a.out
