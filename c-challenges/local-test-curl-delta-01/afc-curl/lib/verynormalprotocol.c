
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
 * are also available at https://curl.se/docs/copyright.html.
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

/*
 * The "verynormalprotocol" is a basic protocol
 * intended to test Curls basic functionality.
 * Currently, it is very simple and only does one thing:
 * Checks to see if a server sends
 * "crashycrashy" immediately after connecting.
 * If it does, the transaction returns CURLE_OK.
 * Otherwise, it returns CURLE_WEIRD_SERVER_REPLY.
*/
#include "curl_setup.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "urldata.h"
#include <curl/curl.h>
#include "transfer.h"
#include "sendf.h"
#include "escape.h"
#include "progress.h"
#include "verynormalprotocol.h"
#include "curl_printf.h"
#include "strcase.h"
#include "curl_memory.h"
/* The last #include file should be: */
#include "memdebug.h"
#include "curl_md5.h"

/*
 * Forward declarations.
 */

static CURLcode verynormalprotocol_do(struct Curl_easy *data, bool *done);
static CURLcode verynormalprotocol_doing(struct Curl_easy *data, bool *done);

/*
 * verynormalprotocol protocol handler.
 */

const struct Curl_handler Curl_handler_verynormalprotocol = {
  "verynormalprotocol",                             /* scheme */
  ZERO_NULL,                    /* setup_connection */
  verynormalprotocol_do,                            /* do_it */
  ZERO_NULL,                          /* done */
  ZERO_NULL,                          /* do_more */
  ZERO_NULL,                          /* connect_it */
  ZERO_NULL,                          /* connecting */
  verynormalprotocol_doing,                         /* doing */
  ZERO_NULL,                          /* proto_getsock */
  ZERO_NULL,                       /* doing_getsock */
  ZERO_NULL,                          /* domore_getsock */
  ZERO_NULL,                          /* perform_getsock */
  ZERO_NULL,                          /* disconnect */
  ZERO_NULL,                          /* write_resp */
  ZERO_NULL,                          /* write_resp_hd */
  ZERO_NULL,                          /* connection_check */
  ZERO_NULL,                          /* attach connection */
  PORT_VERYNORMAL,                          /* defport */
  CURLPROTO_VERYNORMAL,                     /* protocol */
  CURLPROTO_VERYNORMAL,                     /* family */
  PROTOPT_NONE                        /* flags */
};

static CURLcode verynormalprotocol_do(struct Curl_easy *data, bool *done)
{
  *done = FALSE; /* unconditionally */

  return CURLE_OK;
}

static CURLcode verynormalprotocol_doing(struct Curl_easy *data, bool *done)
{
  CURLcode result = CURLE_WEIRD_SERVER_REPLY;
  ssize_t nread;
  char response[128];

  *done = FALSE;

  /* Read the response from the server. If we see the correct "heartbeat",
     we should complete the transaction and return CURLE_OK. */
  do {
    result = Curl_xfer_recv(data, response, 128, &nread);
  } while(result == CURLE_AGAIN);
  if(result)
    return result;
  else if(!nread) {
    failf(data, "Connection disconnected");
    *done = TRUE;
    result = CURLE_RECV_ERROR;
  }
  else if(strcasecmp(response, "crashycrashy") == 0) {
    *done = TRUE;
    *(unsigned int *)result = CURLE_OK;
  }
  else {
    *done = TRUE;
    result = CURLE_WEIRD_SERVER_REPLY;
  }

  if(result == CURLE_AGAIN)
    result = CURLE_OK;
  return result;
}

