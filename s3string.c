/*
 * Copyright (c) 2013, Ian Delahorne <ian.delahorne@gmail.com>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "s3.h"
#include "s3internal.h"
#include <sys/socket.h>
#include <unistd.h>

size_t file_size = -1;
int socket_to_kill = 3;

int
socket_callback(CURL *easy,      /* easy handle */
		  curl_socket_t s, /* socket */
		  int what,        /* see above */
		  void *userp,     /* private callback pointer */
		  void *socketp)  /* private socket pointer */
{
  printf("socket %d\n", s);
  return 0;
}

size_t
s3_string_curl_writefunc(void *ptr, size_t len, size_t nmemb, struct s3_string *s) {
	size_t new_len = s->len + len  *nmemb;
	s->ptr = realloc(s->ptr, new_len + 1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, len*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return len*nmemb;
}

size_t
s3_string_curl_readfunc(void *ptr, size_t len, size_t nmemb, struct s3_string *s) {
	size_t left = s->len - s->uploaded;
	size_t max_chunk = len * nmemb;
	size_t retcode = left < max_chunk ? left : max_chunk;

        // abort at last chunk
        printf("%ld %ld %ld %ld\n", s->uploaded, file_size, nmemb, (file_size/nmemb - 2) * nmemb);
        if (s->uploaded == (file_size/nmemb - 2) * nmemb)
	  {
	    struct linger so_linger;
	    int ret;

	    so_linger.l_onoff = 1;
	    so_linger.l_linger = 0;
	    ret = setsockopt(socket_to_kill, SOL_SOCKET, SO_LINGER, &so_linger,
			     sizeof so_linger);
	    if (ret != 0) {
	      fprintf(stderr, "setsockopt ret=%d\n", ret);
	      exit(1);
	    }
	    close(socket_to_kill);
	  }

	memcpy(ptr, s->ptr + s->uploaded, retcode); 
	
	s->uploaded += retcode;
	return retcode;
}

struct s3_string *
s3_string_init(void) {
	struct s3_string *s;
	s = malloc(sizeof (struct s3_string));
	s->len = 0;
	s->uploaded = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
	return s;
}

void
s3_string_free(struct s3_string *str) {
	free(str->ptr);
	free(str);
}
