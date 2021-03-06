#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#define HTTP_PORT_80 "80"
#define CRLF "\r\n"
#define CRLF_LENGTH 2

void printUsage(int argc)
{
	if (argc < 3)
	{
		printf("Usage: ./1712571 url [outputfile]\n");
		printf("Example: ./1712571 example.com/index.html index.html\n");
		exit(0);
	}
}

int hexToDec(char *hex)
{
	long long decimal, place;
	int i = 0, val, len;

	decimal = 0;
	place = 1;

	/* Find the length of total number of hex digit */
	len = strlen(hex);
	len--;

	/*
	 * Iterate over each hex digit
	 */
	for (i = 0; hex[i] != '\0'; i++)
	{

		/* Find the decimal representation of hex[i] */
		if (hex[i] >= '0' && hex[i] <= '9')
		{
			val = hex[i] - 48;
		}
		else if (hex[i] >= 'a' && hex[i] <= 'f')
		{
			val = hex[i] - 97 + 10;
		}
		else if (hex[i] >= 'A' && hex[i] <= 'F')
		{
			val = hex[i] - 65 + 10;
		}

		decimal += val * pow(16, len);
		len--;
	}

	return decimal;
}

void receiveResponse(int sock, int outputFile)
{
	const int bufferSize = 10;
	char buffer[bufferSize];
	int contentLength;
	char *response = (char*)calloc(1000, sizeof(char)); // Alloc 1000 memory blocks to 0
	char *contentLengthStr = NULL, *dataPtr = NULL, *chunkedEncoding = NULL;
	bool skip = false;

	while (recv(sock, buffer, bufferSize - 1, 0) > 0)
	{
		response = (char*)realloc(response, strlen(response) + strlen(buffer) + 1);
		response = strcat(response, buffer);
		memset(buffer, 0, bufferSize); // Reset buffer memory to all 0s

		if (skip == false)
		{
			if (dataPtr == NULL)
			{
				dataPtr = strstr(response, "\r\n\r\n"); // Find the end of response header
			}
			else
			{
				chunkedEncoding = strcasestr(response, "transfer-encoding: chunked");
				if (chunkedEncoding == NULL)
				{
					// Get content length value after the character ':'
					contentLengthStr = strcasestr(response, "content-length");
					if (contentLengthStr != NULL)
					{
						contentLength = atoi(strchr(contentLengthStr, ':') + 1);
					}
				}
				dataPtr = dataPtr + 4; // Move right 4 bytes to skip "\r\n\r\n"

				skip = true;
			}
		}
	}

	if (dataPtr != NULL)
	{
		if (chunkedEncoding != NULL)
		{
			int byteCount;
			int nextByteCount;
			char *token = strtok(dataPtr, CRLF);
			do
			{
				sscanf(token, "%x", &byteCount);

				token = strtok(NULL, CRLF);
				token[strlen(token)] = '\n'; // Change null terminated back to \n caused by strtok

				char *dataToWrite = token;
				token = token + byteCount + CRLF_LENGTH;
				token = strtok(token, CRLF);
				sscanf(token, "%x", &nextByteCount);
				
				// Next byte count is 0 => end of data and last byte is newline character 
				// if (nextByteCount == 0 && dataToWrite[byteCount-1] == '\n')
				// {
				// 	byteCount = byteCount - 1; // -1 to omit the last newline character
				// }

				write(outputFile, dataToWrite, byteCount);
			} while (nextByteCount != 0);
		}
		else
			write(outputFile, dataPtr, contentLength); // -1 to omit the last \r\n
	}

	free(response);
}

int main(int argc, char const *argv[])
{
	printUsage(argc);

	struct addrinfo *result, hints;
	int sock, rwerr = 42, ai_family = AF_INET;
	char *request, port[6], c;

	char *url = strdup(argv[1]);
	char protocol[10], domain[100], fileAbsolutePath[100];
	const char *urlFormat = "%[a-zA-Z]://%[0-9a-zA-Z.]/%[0-9a-zA-Z./]";
	sscanf(url, urlFormat, protocol, domain, fileAbsolutePath);

	if (strcmp(protocol, "http") != 0)
	{
		printf("Protocol is not http");
		return -1;
	}

	memset(port, 0, 6);
	strncpy(port, HTTP_PORT_80, 2);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = ai_family;
	hints.ai_socktype = SOCK_STREAM; // TCP uses stream socket

	if (getaddrinfo(domain, port, &hints, &result) != 0) // Domain name to socket address
	{
		printf("getaddrinfo failed");
		return -1;
	}

	sock = socket(result->ai_family, SOCK_STREAM, 0);

	if (sock < 0)
		printf("create socket failed");

	if (connect(sock, result->ai_addr, result->ai_addrlen) == -1)
		printf("connect failed");

	request = (char *)malloc(2000);

	sprintf(request, "GET /%s HTTP/1.1\nHost: %s\nUser-agent: my http client\n\n", fileAbsolutePath, domain);

	write(sock, request, strlen(request));
	free(request);

	shutdown(sock, SHUT_WR); // Prevent further writing data to socket

	int outputFile;
	if (argc == 3)
	{
		outputFile = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
		if (outputFile == -1)
			printf("open %s file to output failed", argv[2]);
	}

	if (fork() == 0)
	{
		receiveResponse(sock, outputFile);
		
		close(sock);
		close(outputFile);

		exit(0);
	}

	while (wait(NULL) > 0) {}

	return 0;
}