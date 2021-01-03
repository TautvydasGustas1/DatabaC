/*
 * utils.h
 *
 *  Created on: Dec 25, 2020
 *      Author: iivo
 */

#ifndef UTILS_H_
#define UTILS_H_

	void fatal_error(char *sptr);
	char* peek_first_line(int fd, int maxlength, char delimeter);
	long get_number_from_url(char* url);
	long http1_calculate_message_length(int socketfd);
	long min_long(long a, long b);

#endif /* UTILS_H_ */
