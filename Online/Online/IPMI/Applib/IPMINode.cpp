#include <Applib/IPMINode.h>
extern "C"
{
  #include <ipmitool/ipmi_intf.h>
  #include <ipmitool/ipmi_chassis.h>
}
static struct ipmi_cmd ipmitool_cmd_list[] = {
  { ipmi_chassis_main, "chassis", "Get chassis status and set power state" },
  { NULL ,"",""},
};
IPMINode::IPMINode (string &nname,string &uname,string &passw):m_Name(nname),m_username(uname),m_password(passw)
{
  m_ipmiInterface = 0;
  m_state = uninited;
  m_pstate = Power_unknown;
}
IPMINode::~IPMINode()
{
  if (m_ipmiInterface)
  {
    shutdown();
  }
}
int IPMINode::turnOn()
{
  if (!m_state == inited) return 1;
  int argc=4;
  char *argv[]={"","chassis","power","on"};
  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::turnOff()
{
  if (!m_state == inited) return 1;
  int argc=4;
  char *argv[]={"","chassis","power","off"};
  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::getPowerState()
{
  if (!m_state == inited) return 1;
  int argc=4;
  char *argv[]={"","chassis","power","status"};
  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  if (rc == 0)
    m_pstate = Power_on;
  else
    m_pstate = Power_off;
  return rc;
}
int IPMINode::setup()
{
  m_ipmiInterface = ipmi_light_init_intf((char*)m_Name.c_str(),
      (char*)m_username.c_str(),
      (char*)m_password.c_str(),
      ipmitool_cmd_list);
  if (m_ipmiInterface) m_state = inited;
  return 0;
}
int IPMINode::shutdown()
{
  if (m_state == inited)
  {
    ipmi_light_Cleanup(m_ipmiInterface);
    m_ipmiInterface = 0;
    m_state = uninited;
  }
  return 0;
}
