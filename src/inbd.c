
#include <stdio.h>
#include <stdlib.h>

#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>

#include <netlink/attr.h>

#include "config.h"

#define INTENO_GENL_NAME "Inteno"
#define INTENO_GENL_GRP "notify"

/* attributes */
enum {
	INTENO_NL_UNSPEC,
	INTENO_NL_MSG,
	__INTENO_NL_MAX,
};
#define INTENO_NL_MAX (__INTENO_NL_MAX - 1)

static struct nla_policy inteno_nl_policy[__INTENO_NL_MAX] = {
        [INTENO_NL_MSG] = { .type = NLA_STRING },
};

static struct nlattr *attrs[__INTENO_NL_MAX];

static int inteno_nl_parser(struct nl_msg *msg, void *arg)
{
        struct nlmsghdr *nlh = nlmsg_hdr(msg);
        char *data;
        int ret;

        if (!genlmsg_valid_hdr(nlh, 0)){
                printf("Got invalid message\n");
                return 0;
        }

        ret = genlmsg_parse(nlh, 0, attrs, INTENO_NL_MSG, inteno_nl_policy);

        if (!ret){

                if (attrs[INTENO_NL_MSG] ) {
                        printf("got message string (%s)\n", nla_get_string(attrs[INTENO_NL_MSG]) );
                }
        }
        return 0;
}

int main (int c, char* argv[])
{
        struct nl_sock *sock;
	int err;
	struct nl_cache *family_cache;
        static int grp;

	struct nl_dump_params params = {
		.dp_type = NL_DUMP_LINE,
		.dp_fd = stdout,
	};

        sock = nl_socket_alloc();
        if(!sock){
                printf("Error: could not allocate socket\n");
                exit(1);
        }

        err = nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, inteno_nl_parser, NULL);
//        err = nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, inteno_nl_parser, NULL);

	if ((err = genl_connect(sock)) < 0){
                printf("Error: %s\n", nl_geterror(err));
                exit(1);
        }

        if ((grp = genl_ctrl_resolve_grp(sock, INTENO_GENL_NAME, INTENO_GENL_GRP)) < 0){
                printf("Error: %s (%s grp %s)\n", nl_geterror(grp), INTENO_GENL_NAME, INTENO_GENL_GRP);
                exit(1);
        }

        nl_socket_add_membership(sock, grp);

        /* seq number dont really work for multicast so dissable check */
        nl_socket_disable_seq_check(sock);

        while (1) {
                err = nl_recvmsgs_default(sock);
                if (err < 0){
                        printf("Error: %s (%s grp %s)\n", nl_geterror(err), INTENO_GENL_NAME, INTENO_GENL_GRP);
                }
        }
}
