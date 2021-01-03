/*
 * Functions related to the HTTP protocol
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "fileIO.h"
#include "utils.h"
#include "HTTP1.h"

#define HEADER_MAX_LENGTH 2000
#define INPUT_BUFFER_SIZE 10

/**
 * Not currently used
 */
int is_http1(int socketfd) {
	/* The Request-Line begins with a method token, followed by the
	   Request-URI and the protocol version, and ending with CRLF. The
	   elements are separated by SP characters. No CR or LF is allowed
	   except in the final CRLF sequence. */

	char* request_line = peek_first_line(socketfd, HEADER_MAX_LENGTH, '\n');
	int result = 0;

	if (strstr(request_line, " HTTP/1.") != NULL) {
	    result = 1;
	}
	free(request_line);

	return result;
}

int http1_get_request_method(char* message) {
	if(strncmp("GET", message, 3) == 0) {
		return HTTP_GET_METHOD;
	}
	if(strncmp("POST", message, 4) == 0) {
		return HTTP_POST_METHOD;
	}

	return -1;
}

/**
 * Remember to free the memory address afterwards
 */
char* http1_get_request_url(char* message) {
	//Note: strtok breaks the original string
	char buffer[HEADER_MAX_LENGTH];
	strncpy(buffer, message, HEADER_MAX_LENGTH - 1);
	char* tempp;
	char* temp = strtok_r(message, " ", &tempp); //Method, can return NULL
	if(temp == NULL) {
		return NULL;
	}
	temp = strtok_r(NULL, " ", &tempp);
	#ifdef EXTRAPRINT
		printf("Extracted URL: %s\n", temp);
	#endif

	int length = strlen(temp) + 1;
	char* result = malloc(length);
	strncpy(result, temp, length);

	return result;
}

int http1_send_file(int socketfd, char* filepath) {
	long file_size = get_file_size(filepath);
	if(file_size < 0) {
		return -2;
	}

	char header[HEADER_MAX_LENGTH] = "HTTP/1.1 200 OK\nContent-Length: ";
	char buffer[20];
	sprintf(buffer, "%ld", file_size);
	strcat(header, buffer);
	strcat(header, "\nContent-Type: text/plain; charset=utf-8\n\n");

	int bytes_to_send = strlen(header);
	if(write(socketfd, header, bytes_to_send) != bytes_to_send) {
		perror("Error sending message (header)");
	}
	return file_to_socket(filepath, socketfd);
}

int http1_send_text(int socketfd, char* text, int status_code) {
	/* Example message:
	HTTP/1.1 200 OK
	Content-Length: 4
	Content-Type: text/plain; charset=utf-8

	koe0
	*/
	int text_length = strlen(text);
	char buffer[20];
	char header[HEADER_MAX_LENGTH] = "HTTP/1.1 ";
	sprintf(buffer, "%d ", status_code);
	strcat(header, buffer);

	switch(status_code) {
		case 200:
			strcat(header, "OK");
			break;
		case 201:
			strcat(header, "Created");
			break;
		case 204:
			strcat(header, "No content");
			break;
		case 400:
			strcat(header, "Bad Request");
			break;
		case 401:
			strcat(header, "Unauthorized");
			break;
		case 403:
			strcat(header, "Forbidden");
			break;
		case 404:
			strcat(header, "Not Found");
			break;
		case 405:
			strcat(header, "Method Not Allowed");
			break;
		case 411:
			strcat(header, "Length Required");
			break;
		case 500:
			strcat(header, "Internal Server Error");
			break;
		default:
			strcat(header, "Not Implemented");
	}

	strcat(header, "\nContent-Length: ");
	sprintf(buffer, "%d", text_length);
	strcat(header, buffer);
	strcat(header, "\nContent-Type: text/plain; charset=utf-8\n\n");

	int bytes_to_send = strlen(header);
	if(write(socketfd, header, bytes_to_send) != bytes_to_send) {
		perror("Error sending message (header)");
		return 0;
	}
	if(write(socketfd, text, text_length) != text_length) {
		perror("Error sending message (content)");
		return 0;
	}
	return 1;
}

int http1_receive_file(int socketfd, char* filepath) {
	/* Example message:
	POST /1 HTTP/1.1
	Content-Type: text/plain
	User-Agent: PostmanRuntime/7.26.8
	Accept: *\/*
	Postman-Token: 38ed03f9-24c5-4999-8916-7816aa4e2e8d
	Host: localhost:51030
	Accept-Encoding: gzip, deflate, br
	Connection: keep-alive
	Content-Length: 4

	koe1
	*/

	long message_length = http1_calculate_message_length(socketfd);
	if(message_length == -411) { //Content length was not specified
		http1_send_text(socketfd, "Content-Length must be specified", 411);
		printf("Received a message without Content-Length, aborting");
		return -1;
	}
	if(message_length < 0) {
		http1_send_text(socketfd, "Error calculating message length", 400);
		printf("Error calculating message length, aborting");
		return -1;
	}

	printf("Incoming HTTP message length: %ld bytes\n", message_length);

	FILE *handle;
	if((handle = fopen(filepath, "w")) == NULL) {
		perror("Error opening file for writing");
		return -1;
	}

	char message[INPUT_BUFFER_SIZE];
	long bytes_remaining = message_length;
	int block_writing = 4; //block writing until HTTP content

	while (bytes_remaining > 0) {
		int bytes_read = read(socketfd, &message, min_long(bytes_remaining, INPUT_BUFFER_SIZE));
		if (bytes_read < 1) {
			printf("Error while receiving an incoming message");
			fclose(handle);
			return -1;
		}
		write_to_file_after_http_delimeter(handle, message, bytes_read, &block_writing);

		bytes_remaining -= bytes_read;
		#ifdef EXTRAPRINT
			printf("\nbytes remaining: %ld\n", bytes_remaining);
			printf("%.*s", INPUT_BUFFER_SIZE, message);
		#endif
	}

	if (fclose(handle) == EOF) {
		printf("Error while closing file %s\n", filepath);
	}
	return 1;
}

long http1_calculate_message_length(int socketfd) {
	char message[HEADER_MAX_LENGTH];
	int bytes_read = recv(socketfd, &message, HEADER_MAX_LENGTH, MSG_PEEK);
	if(bytes_read < 4) {
		perror("recv calculating message size");
		return -1;
	}

	//Calculate header length
	int header_length = -1;
	for(int i = 3; i < bytes_read; i++) {
		//detect the empty line (CR LF CR LF) separating header and content - could also use strstr
		if(message[i-3] == '\r' && message[i-2] == '\n' && message[i-1] == '\r' && message[i] == '\n') {
			header_length = i + 1;
			break;
		}
	}
	if(header_length == -1) {
		return -1;
	}

	//Get content length
	char *ptr = strstr(message, "Content-Length:");
	if(ptr == NULL) {
		return -411;
	}

	ptr = ptr + 15; //skip the "Content-Length:" text in front of the value
	char buffer[20];
	for(int i = 0; i < 20; i++) {
		if(ptr[i] == '\r') {
			buffer[i] = '\0';
			break;
		}
		buffer[i] = ptr[i];
	}
	char *tmp;
	long content_length = strtol(buffer, &tmp, 10);

	#ifdef EXTRAPRINT
		printf("Header length: %d\n", header_length);
		printf("Content length buffer: %s\n", buffer);
		printf("Content length: %ld\n", content_length);
	#endif

	return header_length + content_length;
}

