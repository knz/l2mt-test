SLC = slc -b l2mt_f
SLFLAGS =
MAXSIZE = 512
EXT = $(MAXSIZE).bin
BLOCKSIZES = 1,2,4,8,16,30
PROGRAMS = ca1d rgb2gray-fp rgb2gray-int
all: $(PROGRAMS:%=%.$(EXT))

%.$(EXT): %.c bench.c
	$(SLC) -o $@ $< -DMAXSIZE=$(MAXSIZE) -DBLOCKSIZES="$(BLOCKSIZES)" $(SLFLAGS)

clean:
	rm -f *.bin
	rm -f *.o *~ a.out
