MSSV = 1712571

all: $(MSSV)

$(MSSV): src/$(MSSV).c
	mkdir output
	gcc src/$(MSSV).c -o output/$(MSSV) -lm

clean:
	rm -rf output

run: $(MSSV)
	./output/$(MSSV) http://jigsaw.w3.org/HTTP/ChunkedScript output/out.html