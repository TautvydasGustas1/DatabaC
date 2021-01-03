/*
 ============================================================================
 Name        : DatabaC / main.c
 Author      : Iivo Raitahila
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "TCPServer.h"
#include "string.h"

int main(int argc, char *argv[]) {
	printf("\nDatabaC v1.0 2021-01-03\n=======================\n");
	if(argc == 1 || argc >= 4 || (argc == 2 && strcmp(argv[1], "use_env") != 0)) {
		printf("usage:\n");
		printf("config from environment variables LISTEN_PORT and FILE_DIR: %s use_env\n", argv[0]);
		printf("config from command line: %s LISTEN_PORT FILE_DIR\n", argv[0]);
		printf("example: %s 8080 /home/iivo/databac\n", argv[0]);
		return EXIT_FAILURE;
	}
	
	long port;
	char* directory;
	char *tmp;
	
	if(argc == 2) {
		printf("Loading config from environment variables\n");
		if(getenv("LISTEN_PORT") == NULL || getenv("FILE_DIR") == NULL) {
			printf("LISTEN_PORT and/or FILE_DIR environment variables are not set\n");
			return EXIT_FAILURE;
		}
		port = strtol(getenv("LISTEN_PORT"), &tmp, 10);
		directory = getenv("FILE_DIR");
	} else {
		printf("Loading config from command line parameters\n");
		port = strtol(argv[1], &tmp, 10);
		directory = argv[2];
	}

	printf("Using port: %ld\n", port);
	printf("Using file directory: %s\n", directory);
	
	start(port, directory);

	return EXIT_SUCCESS;
}
