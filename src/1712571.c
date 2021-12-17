#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define HTTP_PORT_80 "80"

void printUsage(int argc)
{
	if (argc < 3)
	{
		printf("Usage: ./1712571 url [outputfile]\n");
		printf("Example: ./1712571 example.com/index.html index.html\n");
		exit(0);
	}
}

void receiveResponse(int sock, int outfile)
{
	const int bufferSize = 16;
	char buffer[bufferSize];
	int receivedLen = 0, contentLength;
	char *response = (char *)malloc(0);
	char *contentLengthStr = NULL;
	char *dataPtr = NULL;
	bool skip = false;

	while ((receivedLen = recv(sock, buffer, bufferSize - 1, 0)) > 0)
	{
		buffer[receivedLen] = '\0'; // Last is the null terminating character
		response = (char *)realloc(response, strlen(response) + strlen(buffer) + 1);
		sprintf(response, "%s%s", response, buffer); // Append buffer to response

		if (skip == false)
		{
			if (dataPtr == NULL)
			{
				dataPtr = strstr(response, "\r\n\r\n"); // Find the end of response header
			}
			else
			{
				// Get content length value after the character ':'
				contentLengthStr = strstr(response, "Content-Length");
				if (contentLengthStr != NULL)
				{
					contentLength = atoi((char *)(strchr(contentLengthStr, ':') + 1));
				}

				dataPtr = dataPtr + 4; // Move right 4 bytes to skip \r\n\r\n

				skip = true;
			}
		}
	}

	write(outfile, dataPtr, strlen(dataPtr));
}

int main(int argc, char const *argv[])
{
	printUsage(argc);

	struct addrinfo *result, hints;
	int sock, rwerr = 42, ai_family = AF_INET;
	char *request, port[6], c;

	char *url = strdup(argv[1]);
	char protocol[4];
	char domain[100];
	char rest[100];
	sscanf(url, "%[a-zA-Z]://%[0-9a-zA-Z.]/%[0-9a-zA-Z./]", protocol, domain, rest);

	if (strcmp(protocol, "http") != 0)
	{
		printf("Protocol is not http");
		return -1;
	}

	memset(port, 0, 6);
	strncpy(port, HTTP_PORT_80, 2);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = ai_family;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(domain, port, &hints, &result) != 0)
	{
		printf("getaddrinfo failed");
		return -1;
	}

	sock = socket(result->ai_family, SOCK_STREAM, 0);

	if (sock < 0)
		printf("create socket failed");

	if (connect(sock, result->ai_addr, result->ai_addrlen) == -1)
		printf("connect failed");

	request = (char *)malloc(1000);

	sprintf(request, "GET /%s HTTP/1.1\nHost: %s\nUser-agent: my http client\n\n", rest, domain);
	printf("%s\n\n\n\n", request);

	write(sock, request, strlen(request));

	shutdown(sock, SHUT_WR); // Prevent further writing data to socket

	int outfile;
	if (argc == 3)
	{
		outfile = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
		if (outfile == -1)
			printf("open %s file to output failed", argv[2]);
	}

	receiveResponse(sock, outfile);

	close(sock);
	close(outfile);

	return 0;
}