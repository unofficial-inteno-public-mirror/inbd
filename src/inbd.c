
#include <stdio.h>
#include <stdlib.h>

#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>

#include <netlink/attr.h>

#include <json-c/json.h>

#include "config.h"

#define INTENO_GENL_NAME "Inteno"
#define INTENO_GENL_GRP "notify"
#define SWITCH		"eth0"

/* attributes */
enum {
	INTENO_NL_UNSPEC,
	INTENO_NL_MSG,
	__INTENO_NL_MAX,
};
#define INTENO_NL_MAX (__INTENO_NL_MAX - 1)
#define MAX_MSG 120

static struct nla_policy inteno_nl_policy[__INTENO_NL_MAX] = {
        [INTENO_NL_MSG] = { .type = NLA_STRING },
};

static struct nlattr *attrs[__INTENO_NL_MAX];

static void
remove_char(char *buf, char ch)
{
	char newbuf[strlen(buf)+1];
	int i = 0;
	int j = 0;

	while (buf[i]) {
		newbuf[j] = buf[i];
		if (buf[i] != ch)
			j++;
		i++;
	}
	newbuf[j] = '\0';
	strcpy(buf, newbuf);
}

static void
json_get_var(json_object *obj, char *var, char *value)
{
	json_object_object_foreach(obj, key, val) {
		if(!strcmp(key, var)) {
			switch (json_object_get_type(val)) {
			case json_type_object:
				break;
			case json_type_array:
				break;
			case json_type_string:
				sprintf(value, "%s", json_object_get_string(val));
				break;
			case json_type_boolean:
				sprintf(value, "%d", json_object_get_boolean(val));
				break;
			case json_type_int:
				sprintf(value, "%d", json_object_get_int(val));
				break;
			default:
				break;
			}
		}
	}
}

static void
json_parse_and_get(char *str, char *var, char *value)
{
	json_object *obj;

	obj = json_tokener_parse(str);
	if (is_error(obj) || json_object_get_type(obj) != json_type_object) {
		return;
	}
	json_get_var(obj, var, value);
}


static int process_netlink_message(char *msg)
{
	char event[32];
	char data[128];
	char state[16];
	char port[16];
	char link[16];
	char cmd[MAX_MSG];

	sscanf(msg, "%s %[^\n]s", event, data);

	if (strcmp(event, "wifi.wps") == 0) {
		remove_char(data, '\'');
		json_parse_and_get(data, "state", state);
		if(strcmp(state, "off") == 0)
			system("ubus call led.wps set '{\"state\":\"off\"}'");
		else if (strcmp(state, "in process") == 0)
			system("ubus call led.wps set '{\"state\":\"notice\"}'");
		else if (strcmp(state, "overlap") == 0)
			system("ubus call led.wps set '{\"state\":\"alert\"}'");
		else if (strcmp(state, "error") == 0)
			system("ubus call led.wps set '{\"state\":\"alert\"}'");
		else if (strcmp(state, "success") == 0)
			system("ubus call led.wps set '{\"state\":\"ok\"}'");
/*	} else if (strcmp(event, "wifi.credentials") == 0) {*/
/*		snprintf(cmd, MAX_MSG, "wifi import %s", data);*/
/*		system(cmd);*/
	}
	else if (strcmp(event, "switch") == 0) {
		remove_char(data, '\'');
		json_parse_and_get(data, "port", port);
		json_parse_and_get(data, "link", link);

		if(strcmp(link, "up") == 0)
			strcpy(link, "add");
		else if (strcmp(link, "down") == 0)
			strcpy(link, "remove");

		if(strcmp(port, "0") == 0)
			strcpy(port, "2");
		else if (strcmp(port, "1") == 0)
			strcpy(port, "1");

		snprintf(cmd, MAX_MSG, "ACTION=%s INTERFACE=%s.%s /sbin/hotplug-call net", link, SWITCH, port);
		system(cmd);
	}

	return 0;
}

static int inteno_nl_parser(struct nl_msg *msg, void *arg)
{
        struct nlmsghdr *nlh = nlmsg_hdr(msg);
        char *data;
        int ret;
	char cmd[MAX_MSG];

        if (!genlmsg_valid_hdr(nlh, 0)){
                printf("Got invalid message\n");
                return 0;
        }

        ret = genlmsg_parse(nlh, 0, attrs, INTENO_NL_MSG, inteno_nl_policy);

        if (!ret){

                if (attrs[INTENO_NL_MSG] ) {
			data = nla_get_string(attrs[INTENO_NL_MSG]);
                        //printf("got message string (%s)\n", data);
			snprintf(cmd, MAX_MSG, "ubus send %s\n", data);
			system(cmd);
			process_netlink_message(data);
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
