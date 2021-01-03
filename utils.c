/*
 * Common utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

void fatal_error(char *message) { //simple error handling
    perror(message);
    exit(EXIT_FAILURE);
}

/**
 * Remember to free the memory address afterwards
 */
char* peek_first_line(int fd, int maxlength, char delimeter) {
	char* buffer = malloc(maxlength);
	if(buffer == NULL) {
		fatal_error("Can't reserve memory");
	}

	int bytes_read = recv(fd, buffer, maxlength, MSG_PEEK);
	if(bytes_read <= 0) {
		return NULL;
	}

	for(int i = 0; i < maxlength; i++) {
		if(buffer[i] == delimeter) {
			buffer[i] = 0; //terminate string here
			break;
		}
	}

	return buffer;
}

/**
 * Returns zero by default
 */
long get_number_from_url(char* url) {
	char buffer[100];
	int buffercnt = 0;
	for(int i = 1; i < 100; i++) {
		if(url[i] == '1' || url[i] == '2' || url[i] == '3' || url[i] == '4' || url[i] == '5' ||
				url[i] == '6' || url[i] == '7' || url[i] == '8' || url[i] == '9' || url[i] == '0') {
			buffer[buffercnt] = url[i];
			buffercnt++;
		} else {
			buffer[buffercnt] = '\0';
			break;
		}
	}

	char *tmp;
	long number = strtol(buffer, &tmp, 10);
	return number;
}

long min_long(long a, long b) {
	if(a < b) {
		return a;
	}
	return b;
}
