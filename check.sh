#!/bin/bash

printf "Checking jigsaw chunked script"
curl -v http://jigsaw.w3.org/HTTP/ChunkedScript -o exe/result.html &> /dev/null && \
diff exe/result.html exe/jigsaw_chunkedscript.html

printf "\n\nChecking example.com"
curl -v http://example.com -o exe/result.html &> /dev/null && \
diff exe/result.html exe/example.com_index.html

printf "\n\nChecking azurewebsites chunked"
curl -v http://anglesharp.azurewebsites.net/Chunked -o exe/result.html &> /dev/null && \
diff exe/result.html exe/azure_chunked.html

printf "\n\nChecking info cern"
curl -v http://info.cern.ch/ -o exe/result.html &> /dev/null && \
diff exe/result.html exe/infocern_index.html