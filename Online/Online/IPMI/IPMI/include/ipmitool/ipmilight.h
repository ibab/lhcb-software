#include <ipmitool/ipmi_intf.h>
#include <ipmitool/ipmi_chassis.h>
struct ipmi_intf * ipmi_light_init_intf(    char *hname,
    char *uname,
    char *pword,
struct ipmi_cmd * cmdlist);
int ipmi_light_do_it(struct ipmi_intf *ipmi_main_intf,int argc, char ** argv);
int ipmi_light_Cleanup(struct ipmi_intf *ipmi_main_intf);

