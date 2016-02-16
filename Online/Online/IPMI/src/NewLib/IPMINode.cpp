#include "dis.hxx"
#include <Applib/IPMINode.h>
#include <time.h>
extern "C"
{
  #include <NewLib/ipmi_intf.h>
  #include <Online/IPMI/src/NewLib/ipmi_chassis.h>
}
static struct ipmi_cmd ipmitool_cmd_list[] = {
  { ipmi_chassis_main, "chassis", "Get chassis status and set power state" },
  { NULL ,"",""},
};
IPMINode::IPMINode (string &nname,string &uname,string &passw):m_Name(nname),m_username(uname),m_password(passw)
{
  m_intName = m_Name+"-ipmi";
  m_ipmiInterface = 0;
  m_state = uninited;
  m_pstate = Power_unknown;
  m_svalue = (int)Power_unknown;
  m_service = 0;
  m_lastUpdate = 0;
  m_cmd = 0;
  m_lastread = 0;
  m_retintf = 0;
}
IPMINode::~IPMINode()
{
  if (m_ipmiInterface)
  {
    shutdown();
  }
  if (m_service)
  {
    delete (m_service);
    m_service = 0;
  }
  if (m_cmd)
  {
    delete (m_cmd);
    m_cmd = 0;
  }
  if (m_lastUpdate)
  {
    delete m_lastUpdate;
    m_lastUpdate = 0;
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
int IPMINode::cycle()
{
  if (!m_state == inited) return 1;
  int argc=4;
  char *argv[]={"","chassis","power","cycle"};
  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::reset()
{
  if (!m_state == inited) return 1;
  int argc=4;
  char *argv[]={"","chassis","power","reset"};
  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::getPowerState()
{
  if (!m_state == inited) return 1;
  int argc=4;
  char *argv[]={"","chassis","power","status"};
  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  PowerState oldstate=m_pstate;
  if (rc == 0)
    m_pstate = Power_off;
  else if (rc == 1)
    m_pstate = Power_on;
  else
  {
    printf ("Getting Power State failed for node %s\n",m_Name.c_str());
    m_pstate = Power_unknown;
  }
  if (m_pstate != oldstate)
  {
    //update State Service
    m_svalue = (int)m_pstate;
    m_service->updateService(m_svalue);
  }
  m_lastread = time(0);
  m_lastUpdate->updateService(m_lastread);
  return rc;
}
int IPMINode::setup(char * DimServerName)
{
  int rc;
  m_servName = string(DimServerName)+"/"+m_Name+"/power_status";
  m_cmdName = string(DimServerName)+"/"+m_Name+"/power_switch";
  m_service = new DimService(m_servName.c_str(),m_svalue);
  m_servName = string(DimServerName)+"/"+m_Name+"/last_Reading";
  m_lastUpdate = new DimService(m_servName.c_str(),m_svalue);
  m_cmd = new DimCommand(m_cmdName.c_str(),"C");
  m_ipmiInterface = ipmi_light_init_intf((char*)m_intName.c_str(),
      (char*)m_username.c_str(),
      (char*)m_password.c_str(),
      ipmitool_cmd_list);
  m_retintf = m_ipmiInterface;
  if (m_ipmiInterface>0)
  {
    m_state = inited;
    rc = 0;
  }
  else
  {
    rc = 1;
  }
  return rc;
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
