/*
 * fileIO.h
 *
 *  Created on: Dec 25, 2020
 *      Author: iivo
 */

#ifndef FILEIO_H_
#define FILEIO_H_

	char* get_file_path(int number, char* directory);
	long get_file_size(char* filepath);
	int file_to_socket(char* filepath, int socketfd);
	int write_to_file(FILE* handle, char* data, int n);
	int write_to_file_after_http_delimeter(FILE* handle, char* data, int n, int* block_writing);

#endif /* FILEIO_H_ */
