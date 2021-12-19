MSSV = 1712571

all: $(MSSV)

$(MSSV): src/$(MSSV).c
	mkdir -p exe
	gcc src/$(MSSV).c -o exe/$(MSSV) -lm

clean:
	rm -rf exe/*

run: $(MSSV)
	./exe/$(MSSV) http://jigsaw.w3.org/HTTP/ChunkedScript exe/jigsaw_chunkedscript.html
	./exe/$(MSSV) http://example.com/index.html exe/example.com_index.html
	./exe/$(MSSV) http://anglesharp.azurewebsites.net/Chunked exe/azure_chunked.html
	./exe/$(MSSV) http://info.cern.ch/ exe/infocern_index.html
	@echo "Done! Check exe folder"