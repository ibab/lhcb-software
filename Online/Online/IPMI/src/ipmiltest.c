#include <stddef.h>
#include <stdlib.h>
#include <ipmitool/ipmilight.h>
struct ipmi_cmd ipmitool_cmd_list[] = {
  { ipmi_chassis_main, "chassis", "Get chassis status and set power state" },
  { NULL },
};
int csv_output = 0;
int verbose = 0;

int main(int argc, char ** argv)
{
  struct ipmi_intf *rc;
  int ret;
//	 -U root -I lan -H hlta0101-ipmi -vvvv -P lhcbipmi
  rc = ipmi_light_init_intf("hlta0101-ipmi","root","lhcbipmi", ipmitool_cmd_list);
  ret = ipmi_light_do_it(rc,argc,argv);
  ret = ipmi_light_do_it(rc,argc,argv);
  ipmi_light_Cleanup(rc);
  if (ret < 0)
    exit(ret);
  else
    exit(ret);
}
