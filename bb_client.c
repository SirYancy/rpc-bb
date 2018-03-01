/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bb.h"

CLIENT *setup_rpc(char *host);

int main (int argc, char *argv[])
{
    CLIENT *clnt;
	char *host;

    if (argc < 2) {
        printf("usage: %s server_host\n", argv[0]);
        exit(1);
    }

    host = argv[1];
    clnt = setup_rpc(host);



#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */

    exit (0);
}

void post(char *user, char *article, CLIENT *clnt)
{
	bool_t  *result_1;

	result_1 = post_1(user, article, clnt);
	if (result_1 == (bool_t *) NULL) {
		clnt_perror (clnt, "call failed");
	}

}

CLIENT *setup_rpc(char *host)
{
    CLIENT *clnt;

#ifndef	DEBUG
	clnt = clnt_create (host, BULLETIN_PROG, BULLETIN_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

    return clnt;

}

void
bulletin_prog_1(char *host)
{
	char * *result_2;
	char * *result_3;
	int choose_1_id;
	bool_t  *result_4;
	int reply_1_id;
	char *reply_1_user;
	char *reply_1_Article;

#ifndef	DEBUG
	clnt = clnt_create (host, BULLETIN_PROG, BULLETIN_VERSION, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_2 = read_1(clnt);
	if (result_2 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = choose_1(choose_1_id, clnt);
	if (result_3 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_4 = reply_1(reply_1_id, reply_1_user, reply_1_Article, clnt);
	if (result_4 == (bool_t *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


