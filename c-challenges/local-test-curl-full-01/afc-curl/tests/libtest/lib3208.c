/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include "test.h"
#include "testutil.h"
#include "warnless.h"
#include "memdebug.h"


static size_t header_callback(char *ptr, size_t size, 
                              size_t nmemb, void *userdata)
{
    fprintf(stderr, "Header received: %s\n", ptr);
    fprintf(stderr, "Size received: %d\n", size);
    size_t fullSize = (nmemb * size);
    fprintf(stderr, "full size received: %d\n", fullSize);
    int *numHeaders = (int *)userdata;
    const char *prefix = NULL;
    unsigned long lenprefix;
    if(*numHeaders == 0) {
        prefix = "Content-Length";
    }
    else if(*numHeaders == 1) {
        prefix = "Accept-ranges";
    }
    else if(*numHeaders == 2) {
        prefix = "File-body";
    }
    else if(*numHeaders == 3) {
        prefix = "Last-Modified";
    }
    else {
        return fullSize;
    }

    fprintf(stderr, "Checking len prefix\n");
    lenprefix = strlen(prefix);
    if(fullSize < lenprefix) {
        fprintf(stderr, "ERROR: Size less than lenprefix\r\n");
        return CURLE_WRITE_ERROR;
    }

    fprintf(stderr, "Comparing strings\n");
    int comp = strncmp(ptr, prefix, lenprefix);
    if(comp) {
        fprintf(stderr, 
            "Header different than expected. Number: %d  Header: %s\r\n",
            *numHeaders, ptr);
        return CURLE_WRITE_ERROR;
    }

    fprintf(stderr, "Increasing numHeaders from %d\n", *numHeaders);
    *numHeaders = *numHeaders + 1;
    return fullSize;
}

CURLcode test(char *URL)
{
    fprintf(stderr, "URL received: %s\n", URL);
    int nexpheaders;
    nexpheaders = 4;
    CURL *curl;
    CURLcode res = CURLE_OK;
    int *numHeaders = 0;
    fprintf(stderr, "About to global init received\n");
    curl_global_init(CURL_GLOBAL_DEFAULT);
    fprintf(stderr, "About to easy init received\n");
    curl = curl_easy_init();
    if(!curl) {
        fprintf(stderr, "curl_easy_init() failed\n");
        return TEST_ERR_MAJOR_BAD;
        goto test_cleanup;
    }
    fprintf(stderr, "About to set URL\n");
    int lenSub = strlen(URL) - 5;
    fprintf(stderr, "Allocating %d chars.\n", lenSub);
    char *subURL = (char *) malloc(lenSub + 1);
    strncpy(subURL, URL, lenSub);
    subURL[lenSub] = '\0';
    fprintf(stderr, "subURL: %s\n",subURL);
    test_setopt(curl, CURLOPT_URL, subURL);
    test_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    test_setopt(curl, CURLOPT_HEADERDATA, &numHeaders);
    test_setopt(curl, CURLOPT_HEADER, 1L);
    fprintf(stderr, "About to perform\n");
    res = curl_easy_perform(curl);
    fprintf(stderr, "After performing\n");
test_cleanup:
    fprintf(stderr, "Cleaning up easy\n");
    curl_easy_cleanup(curl);
    fprintf(stderr, "Cleaning up global\n");
    curl_global_cleanup();
    return res;
}
