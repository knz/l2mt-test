SLC = slc -b l2mt_f
TABLE_SIZE = 10
BLOCKSIZES = 1,2,4,8,16,30

all: fft-run$(TABLE_SIZE)

fft-run$(TABLE_SIZE): fft-run.c fft_table2_$(TABLE_SIZE)_data.h
	$(SLC) -o $@ $< -DTABLE_SIZE=$(TABLE_SIZE) -DBLOCKSIZES="$(BLOCKSIZES)"

fft_table2_$(TABLE_SIZE)_data.h: gen_table_2.sh
	$(SHELL) -c "env TABLE_SIZE=$(TABLE_SIZE) ./$<" >$@.tmp || rm -f $@.tmp
	mv -f $@.tmp $@

clean:
	rm -f fft-run[0-9]*
	rm -f fft_table*.h
	rm -f *.o *~ a.out
