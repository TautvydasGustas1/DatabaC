/*
 * Functions related to handling files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "utils.h"

#define BUFFER_SIZE 20
#define MAX_PATH_LENGTH (4096 + 255)
#define SEGMENT_SIZE 536

/**
 * Remember to free the memory address afterwards
 */
char* get_file_path(int number, char* directory) {
	char* path = malloc(MAX_PATH_LENGTH);
	if(path == NULL) {
		fatal_error("Can't reserve memory");
	}
	strcpy(path, directory);
	char buffer[BUFFER_SIZE];
	sprintf(buffer, "/%d", number);
	strcat(path, buffer);
	strcat(path, ".txt");
	return path;
}

long get_file_size(char* filepath) {
	FILE *handle = fopen(filepath, "r");
	if(handle == NULL) {
		perror("Error opening file for reading");
		return -1;
	}

	fseek(handle, 0L, SEEK_END);
	long int result = ftell(handle);

	if (fclose(handle) == EOF) {
		printf("Error while closing file %s\n", filepath);
	}
	return result;
}

int file_to_socket(char* filepath, int socketfd) {

	FILE *handle = fopen(filepath, "r");
	if(handle == NULL) {
		perror("Error opening file for reading");
		return -1;
	}

	char buffer[SEGMENT_SIZE];
	int buffer_pos = 0;
	int c;
	while ((c = fgetc(handle)) != EOF) {
		//Read char by char, but send in larger segments to lower overhead
//		printf("%c", c);
		buffer[buffer_pos] = c;
		buffer_pos++;
		if(buffer_pos == SEGMENT_SIZE) { //send message when buffer is full
			if(write(socketfd, &buffer, SEGMENT_SIZE) != SEGMENT_SIZE) {
				perror("Error sending message (MSS buffer)");
			}
			buffer_pos = 0;
		}
	}
	if(buffer_pos != 0) { //send the last (non full) buffer
		if(write(socketfd, &buffer, buffer_pos) != buffer_pos) {
			perror("Error sending message (half buffer)");
		}
	}

	if (fclose(handle) == EOF) {
		printf("Error while closing file %s\n", filepath);
	}

	return 1;
}

/**
 * Start to write to file after 4 \n or \r characters
 */
int write_to_file_after_http_delimeter(FILE* handle, char* data, int n, int* block_writing) {
	for (int i = 0; i < n; i++){
		if(*block_writing == 0) {
			fputc(data[i], handle);
		} else if(data[i] == '\n' || data[i] == '\r') {
			(*block_writing)--;
		} else {
			(*block_writing) = 4;
		}
	}

	return 1;
}

int write_to_file(FILE* handle, char* data, int n) {

//	for (int i = 0; data[i] != 0; i++){ //Write until end of string
	for (int i = 0; i < n; i++){
		fputc(data[i], handle);
	}

	return 1;
}
