/*
 * HTTP1.h
 *
 *  Created on: Dec 25, 2020
 *      Author: iivo
 */

#ifndef HTTP1_H_
#define HTTP1_H_

	#define HTTP_GET_METHOD 1
	#define HTTP_POST_METHOD 2

	int is_http1(int socketfd);
	int http1_send_file(int socketfd, char* filepath);
	int http1_send_text(int socketfd, char* text, int status_code);
	int http1_get_request_method(char* message);
	char* http1_get_request_url(char* message);
	int http1_receive_file(int socketfd, char* filepath);

#endif /* HTTP1_H_ */
