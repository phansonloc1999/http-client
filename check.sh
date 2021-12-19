#!/bin/bash

printf "Checking jigsaw chunked script"
curl -v http://jigsaw.w3.org/HTTP/ChunkedScript -o output/result.html &> /dev/null && \
diff output/result.html output/jigsaw_chunkedscript.html

printf "\n\nChecking example.com"
curl -v http://example.com -o output/result.html &> /dev/null && \
diff output/result.html output/example.com_index.html

printf "\n\nChecking azurewebsites chunked"
curl -v http://anglesharp.azurewebsites.net/Chunked -o output/result.html &> /dev/null && \
diff output/result.html output/azure_chunked.html

printf "\n\nChecking info cern"
curl -v http://info.cern.ch/ -o output/result.html &> /dev/null && \
diff output/result.html output/infocern_index.html