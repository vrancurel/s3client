/*
 * Copyright (c) 2013,2014 Ian Delahorne <ian.delahorne@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.  
 */

#include "s3.h"

#ifdef LINUX
#include <bsd/string.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include <curl/curl.h>

extern size_t file_size;

int main (int argc, char **argv) {
	struct S3 *s3; 

	char *s3_key_id = getenv("AWS_ACCESS_KEY_ID");
	char *s3_secret = getenv("AWS_SECRET_KEY");
	char *p;

	if (argc != 3) {
	  fprintf(stderr, "usage: s2datatest host size\n");
	  return 1;
	}

	file_size = strtoul(argv[2], NULL, 10);
	
	if (s3_key_id == NULL || s3_secret == NULL) {
		fprintf(stderr, "Error: Environment variable AWS_ACCESS_KEY_ID or AWS_SECRET_KEY not set\n");
		return 1;
	}

	s3 = s3_init(s3_key_id, s3_secret, "zenko.local");

	p = malloc(file_size);
	if (p == NULL) {
	  perror ("malloc");
	  return 1;
	}
	
	while (1)
	  s3_data_put(s3, argv[1], "application/octet-stream", p, file_size);
	
	s3_free(s3);

	return 0;
}
