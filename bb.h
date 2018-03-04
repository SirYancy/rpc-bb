/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _BB_H_RPCGEN
#define _BB_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct post_1_argument {
	char *user;
	char *Article;
};
typedef struct post_1_argument post_1_argument;

struct reply_1_argument {
	int id;
	char *user;
	char *Article;
};
typedef struct reply_1_argument reply_1_argument;

#define BULLETIN_PROG 0x13131345
#define BULLETIN_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define Post 1
extern  bool_t * post_1(char *, char *, CLIENT *);
extern  bool_t * post_1_svc(char *, char *, struct svc_req *);
#define Read 2
extern  char ** read_1(CLIENT *);
extern  char ** read_1_svc(struct svc_req *);
#define choose 3
extern  char ** choose_1(int , CLIENT *);
extern  char ** choose_1_svc(int , struct svc_req *);
#define Reply 4
extern  bool_t * reply_1(int , char *, char *, CLIENT *);
extern  bool_t * reply_1_svc(int , char *, char *, struct svc_req *);
extern int bulletin_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define Post 1
extern  bool_t * post_1();
extern  bool_t * post_1_svc();
#define Read 2
extern  char ** read_1();
extern  char ** read_1_svc();
#define choose 3
extern  char ** choose_1();
extern  char ** choose_1_svc();
#define Reply 4
extern  bool_t * reply_1();
extern  bool_t * reply_1_svc();
extern int bulletin_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_post_1_argument (XDR *, post_1_argument*);
extern  bool_t xdr_reply_1_argument (XDR *, reply_1_argument*);

#else /* K&R C */
extern bool_t xdr_post_1_argument ();
extern bool_t xdr_reply_1_argument ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_BB_H_RPCGEN */