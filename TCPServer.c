/*
 * Server loop for TCP clients. Forks a new process for each client
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "HTTP1.h"
#include "utils.h"
#include "fileIO.h"

//TODO: Mutex for file writes

void process_request(int socketfd, char* file_dir) {

    char message[1000];
    int bytes_read = recv(socketfd, &message, 1000, MSG_PEEK);
	if(bytes_read > 0) {
//			printf("message: %s\n", message);
		int method = http1_get_request_method(&message[0]);
		#ifdef EXTRAPRINT
			printf("Method: %d\n", method);
		#endif

		if(method != HTTP_GET_METHOD && method != HTTP_POST_METHOD) {
			http1_send_text(socketfd, "Allowed methods: GET, POST", 405);
			printf("Request aborted\n");
			return;
		}

		char* url = http1_get_request_url(&message[0]);
		long number = get_number_from_url(url);
		printf("Requested %s, using file %ld.txt\n", url, number);
		free(url);

		char* filepath = get_file_path(number, file_dir);
		printf("METHOD ", method);
		if(method == HTTP_GET_METHOD) {
			if(access(filepath, F_OK) == 0 ) { //file exists
				if(http1_send_file(socketfd, filepath) == -2) {
					http1_send_text(socketfd, "Error while reading the file", 500);
				}
			} else { //file doesn't exist
				http1_send_text(socketfd, "File not found", 404);
			}
		} else { //POST
			if(http1_receive_file(socketfd, filepath) > 0) {
				http1_send_text(socketfd, "File saved", 201);
			} else {
				http1_send_text(socketfd, "Something went wrong - check logs", 500);
			}
		}
		free(filepath);
		printf("Request done\n");
	} else {
		perror("Process request");
	}
}

int start(long port, char* file_dir) {

	int listenfd, newsockfd, childpid;
	unsigned int clilen;
	struct sockaddr_in cli_addr, serv_addr;

	if ((listenfd = socket(PF_INET, SOCK_STREAM, PF_UNSPEC)) < 0) { // Open a TCP socket
		fatal_error("Creating stream socket");
	}

	// Bind our local address so that the client can connect to us
	bzero((void *) &serv_addr, sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
	if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
		fatal_error("Binding local address");
	}

	listen(listenfd, 5); // use larger BACKLOG value for busy servers
	printf("Server started. Må!\n");
	while(1) {
		// Wait for a connection from a client process (in "accept")
		clilen = sizeof (cli_addr);
		newsockfd = accept(listenfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			perror("Accept");
			continue;
		}

		if ((childpid = fork()) < 0) {
			fatal_error("Can't fork");
		} else if (childpid == 0) { // child process
			close(listenfd); // close original socket
			process_request(newsockfd, file_dir);
			close(newsockfd);
			exit(0);
		} else { // parent process
			close(newsockfd);
		}
	}

	return 0;
}
