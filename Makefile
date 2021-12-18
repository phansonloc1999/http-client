MSSV = 1712571

all: $(MSSV)

$(MSSV): src/$(MSSV).c
	mkdir -p output
	gcc src/$(MSSV).c -o output/$(MSSV) -lm

clean:
	rm -rf output/*

run: $(MSSV)
	./output/$(MSSV) http://jigsaw.w3.org/HTTP/ChunkedScript output/jigsaw_chunkedscript.html
	./output/$(MSSV) http://example.com output/example.com_index.html
	./output/$(MSSV) http://anglesharp.azurewebsites.net/Chunked output/azure_chunked.html