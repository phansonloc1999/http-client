MSSV = 1712571

all: $(MSSV)

$(MSSV): src/$(MSSV).c
	gcc src/$(MSSV).c -o output/$(MSSV) -lm

clean:
	rm -f output/*

run: $(MSSV)
	./output/$(MSSV) http://jigsaw.w3.org/HTTP/ChunkedScript output/out.html