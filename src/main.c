#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define HTTP_PORT_80 "80"

int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		printf("Usage: ./1712571 url [outputfile]\n");
		printf("Example: ./1712571 example.com/index.html index.html\n");
		return -1;
	}

	struct addrinfo *result, hints;
	int sock, rwerr = 42, ai_family = AF_INET;
	char *request, buf[16], port[6], c;

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

	request = (char*)malloc(1000);

	sprintf(request, "GET /%s HTTP/1.1\nHost: %s\nUser-agent: my http client\n\n", rest, domain);
	printf("%s\n\n\n\n", request);

	write(sock, request, strlen(request));

	shutdown(sock, SHUT_WR);

	int outfile;
	if (argc == 3)
	{
		outfile = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if (outfile == -1) printf("open %s file to output failed", argv[2]);
	}

	while (rwerr > 0)
	{
		rwerr = read(sock, buf, 16);
		if (argc != 3) write(1, buf, rwerr);
		else write(outfile, buf, rwerr);
	}

	close(sock);
	close(outfile);

	return 0;
}