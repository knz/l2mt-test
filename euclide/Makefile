SLC = slc -b l2mt_f
FRAME_RATIO = 16
BLOCKSIZE = 4
EXT = $(FRAME_RATIO).$(BLOCKSIZE).bin

PROGRAMS = \
  euclide1-pllsf-1d \
  euclide1-pllsf-2d \
  euclide1-pllsf-3d \
  euclide2-detectsat-1d \
  euclide3-ssb-1d \
  euclide4-nlcp-1d \
  euclide5-srptb-1d \
  euclide6-dcr-1d

TARGETS = $(foreach R,16 8 4 2 1, \
             $(foreach B,1 2 4 8 16 30, \
                 $(PROGRAMS:%=%.$(R).$(B).bin)))
.SUFFIXES: .bin

all: $(TARGETS)

clean:
	rm -f *.bin
	rm -f *.o *~ a.out

run: all
	for p in $(PROGRAMS); do \
	   $(MAKE) run-$$p; \
	done

run-%:
	for r in 16 8 4 2 1; do \
	 for b in 1 2 4 8 16 30; do \
	   echo "Running $* for frame size $$(expr 2048 / $$r), block size $$b"; \
	     echo running: $*.$$r.$$b.bin; \
	     slr $*.$$r.$$b.bin; \
	   done; \
	   echo; \
	 done; \
	done

.SECONDEXPANSION:
%.bin: $$(basename $$(basename $$*)).c euclide.h bench.c slext.h
	R=`echo $*|cut -d. -f2`; \
	B=`echo $*|cut -d. -f3`; \
	$(SLC) -o $@ $< -DFRAME_RATIO=$$R -DBLOCKSIZE=$$B

