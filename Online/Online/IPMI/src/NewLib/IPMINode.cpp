#include "dis.hxx"
#include "IPMINode.h"
#include <time.h>
#include "ipmi_intf.h"
#include "ipmi_chassis.h"
#include "ipmi_session.h"
#include "log.h"
#include "ipmi_constants.h"
//static struct ipmi_cmd ipmitool_cmd_list[] = {
//  { ipmi_chassis_main, "chassis", "Get chassis status and set power state" },
//  { NULL ,"",""},
//};
IPMINode::IPMINode(string &nname, string &uname, string &passw) :
    m_Name(nname), m_username(uname), m_password(passw)
{
  m_intName = m_Name;// + "-ipmi";
  m_ipmiInterface = 0;
  m_state = uninited;
  m_pstate = Power_unknown;
  m_svalue = (int) Power_unknown;
  m_service = 0;
  m_lastUpdate = 0;
  m_cmd = 0;
  m_lastread = 0;
  m_retintf = 0;
}
IPMINode::~IPMINode()
{
  shutdown();
}
int IPMINode::shutdown()
{
//  if (m_state == inited)
//  {
  if (m_ipmiInterface != 0)
  {
//    if (m_ipmiInterface->session != 0)
//    {
//      delete (m_ipmiInterface->session);
//      m_ipmiInterface->session = 0;
//    }
    if (m_ipmiInterface->chassis != 0)
    {
      delete m_ipmiInterface->chassis;
      m_ipmiInterface->chassis = 0;
    }
    delete (m_ipmiInterface);
    m_ipmiInterface = 0;
//    }
//    ipmi_light_Cleanup(m_ipmiInterface);
    m_state = uninited;
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
  return 0;
}
int IPMINode::turnOn()
{
  if (!m_state == inited)
    return 1;
  int rc = m_ipmiInterface->chassis->ipmi_chassis_power_control(
      IPMI_CHASSIS_CTL_POWER_UP);
//  int argc=4;
//  char *argv[]={"","chassis","power","on"};
//  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::turnOff()
{
  if (!m_state == inited)
    return 1;
  int rc = m_ipmiInterface->chassis->ipmi_chassis_power_control(
      IPMI_CHASSIS_CTL_POWER_DOWN);
//  int argc=4;
//  char *argv[]={"","chassis","power","off"};
//  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::cycle()
{
  if (!m_state == inited)
    return 1;
  int rc = m_ipmiInterface->chassis->ipmi_chassis_power_control(
      IPMI_CHASSIS_CTL_POWER_CYCLE);
//  int argc=4;
//  char *argv[]={"","chassis","power","cycle"};
//  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::reset()
{
  if (!m_state == inited)
    return 1;
  int rc = m_ipmiInterface->chassis->ipmi_chassis_power_control(
      IPMI_CHASSIS_CTL_HARD_RESET);
//  int argc=4;
//  char *argv[]={"","chassis","power","reset"};
//  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
  return rc;
}
int IPMINode::getPowerState()
{
  int statcnt;
  statcnt = 0;
  PowerState oldstate = m_pstate;
  int rc;
  while (statcnt <2)
  {
    if (!m_state == inited)
      return 1;
    rc = m_ipmiInterface->chassis->ipmi_chassis_power_status();
  //  int argc=4;
  //  char *argv[]={"","chassis","power","status"};
  //  int rc=ipmi_light_do_it(m_ipmiInterface,argc,argv);
    if (rc == 0)
    {
      m_pstate = Power_off;
      break;
    }
    else if (rc == 1)
    {
      m_pstate = Power_on;
      break;
    }
    else
    {
//      printf("Getting Power State failed for node %s. retrying...\n", m_Name.c_str());
      m_pstate = Power_unknown;
      statcnt++;
      usleep(100000);
    }
  }
  if (m_pstate != oldstate)
  {
    //update State Service
    m_svalue = (int) m_pstate;
    m_service->updateService(m_svalue);
  }
  m_lastread = time(0);
  m_lastUpdate->updateService(m_lastread);
  return rc;
}
int IPMINode::setup(char * DimServerName)
{
  int rc=-1;

#if 0
  uint8_t target_addr = 0;
  uint8_t target_channel = 0;

  uint8_t transit_addr = 0;
  uint8_t transit_channel = 0;
  uint8_t target_lun = 0;
#endif
  uint8_t arg_addr = 0;
#if 0
  uint8_t addr = 0;
  uint16_t my_long_packet_size = 0;
  uint8_t my_long_packet_set = 0;
#endif
  uint8_t lookupbit = 0x10; /* use name-only lookup by default */

  if (m_state != uninited)
  {
    printf("shit\n");
  }
  if (m_ipmiInterface == 0)
  {
    m_ipmiInterface = new ipmi_intf();  //*ipmi_main_intf,*IntfPatt;

    /* if no interface was specified but a
     * hostname was then use LAN by default
     * otherwise the default is hardcoded
     * to use the first entry in the list
     */
    m_ipmiInterface->ipmi_intf_load((char*) "lan");
  }
  else
  {
    printf("shit again\n");

  }
  /* setup log */
  log_init(0, 0, verbose);

  /* run OEM setup if found */
#if 0
  if (oemtype != NULL && ipmi_oem_setup(ipmi_main_intf, oemtype) < 0)
  {
    lprintf(LOG_ERR, "OEM setup for \"%s\" failed", oemtype);
    ipmi_light_Cleanup(ipmi_main_intf);
    return 1;
  }
#endif
  /* set session variables */
  if (m_Name.size() != 0)
    m_ipmiInterface->ipmi_intf_session_set_hostname((char*) m_intName.c_str());
  if (m_username.size() != 0)
    m_ipmiInterface->ipmi_intf_session_set_username((char*) m_username.c_str());
  if (m_password.size() != 0)
    m_ipmiInterface->ipmi_intf_session_set_password((char*) m_password.c_str());
#if 0
  if (kgkey != NULL)
  m_ipmiInterface->ipmi_intf_session_set_kgkey((char*) kgkey);
  if (port > 0)
  m_ipmiInterface->ipmi_intf_session_set_port(port);
  if (authtype >= 0)
  m_ipmiInterface->ipmi_intf_session_set_authtype((uint8_t) authtype);
  if (privlvl > 0)
  m_ipmiInterface->ipmi_intf_session_set_privlvl((uint8_t) privlvl);
  else
#endif
  m_ipmiInterface->ipmi_intf_session_set_privlvl(IPMI_SESSION_PRIV_ADMIN); /* default */
  /* Adding retry and timeout for interface that support it */
#if 0
  if (retry > 0)
  m_ipmiInterface->ipmi_intf_session_set_retry(retry);
#endif
  m_ipmiInterface->ipmi_intf_session_set_timeout(2);
  m_ipmiInterface->ipmi_intf_session_set_lookupbit(lookupbit);
  m_ipmiInterface->ipmi_intf_session_set_sol_escape_char(
      (char) SOL_ESCAPE_CHARACTER_DEFAULT);
  m_ipmiInterface->ipmi_intf_session_set_cipher_suite_id(3);

  m_ipmiInterface->devnum = 0;

  /* setup device file if given */
  m_ipmiInterface->devfile = 0;

  /* Open the interface with the specified or default IPMB address */
  m_ipmiInterface->my_addr = arg_addr ? arg_addr : IPMI_BMC_SLAVE_ADDR;
  if (m_ipmiInterface->opened == 0)
  {
    if (m_ipmiInterface->open() < 0)
    {
//      ipmi_light_Cleanup(ipmi_main_intf);
      return 1;
    }
    else
    {
      m_ipmiInterface->opened = 1;
    }
  }
  /*
   * Attempt picmg discovery of the actual interface address unless
   * the users specified an address.
   *    Address specification always overrides discovery
   */
//  if (picmg_discover(ipmi_main_intf) && !arg_addr)
//  {
//    lprintf(LOG_DEBUG, "Running PICMG Get Address Info");
//    addr = ipmi_main_intf->ipmi_picmg_ipmb_address();
//    lprintf(LOG_INFO, "Discovered IPMB-0 address 0x%x", addr);
//  }
  /*
   * If we discovered the ipmb address and it is not the same as what we
   * used for open, Set the discovered IPMB address as my address if the
   * interface supports it.
   */
//  if (addr != 0 && addr != ipmi_main_intf->my_addr
//      && ipmi_main_intf->set_my_addr)
//  {
//    /*
//     * Only set the interface address on interfaces which support
//     * it
//     */
//    (void) ipmi_main_intf->set_my_addr(ipmi_main_intf, addr);
//  }
//
  /* If bridging addresses are specified, handle them */
//  if (target_addr > 0)
//  {
//    m_ipmiInterface->target_addr = target_addr;
//    m_ipmiInterface->target_lun = target_lun;
//    m_ipmiInterface->target_channel = target_channel;
//  }
//  if (transit_addr > 0)
//  {
//    /* sanity check, transit makes no sense without a target */
//    if ((transit_addr != 0 || transit_channel != 0)
//        && m_ipmiInterface->target_addr == 0)
//    {
//      lprintf(LOG_ERR, "Transit address/channel %#x/%#x ignored. "
//          "Target address must be specified!", transit_addr, transit_channel);
//      ipmi_light_Cleanup(ipmi_main_intf);
//      return 1;
//    }
//
//    m_ipmiInterface->transit_addr = transit_addr;
//    m_ipmiInterface->transit_channel = transit_channel;
//  }
  if (m_ipmiInterface->target_addr > 0)
  {
    /* must be admin level to do this over lan */
    m_ipmiInterface->ipmi_intf_session_set_privlvl( IPMI_SESSION_PRIV_ADMIN);
    /* Get the ipmb address of the targeted entity */
//    ipmi_main_intf->target_ipmb_addr =
//        ipmi_main_intf->ipmi_picmg_ipmb_address();
    lprintf(LOG_DEBUG,
        "Specified addressing     Target  %#x:%#x Transit %#x:%#x",
        m_ipmiInterface->target_addr, m_ipmiInterface->target_channel,
        m_ipmiInterface->transit_addr, m_ipmiInterface->transit_channel);
    if (m_ipmiInterface->target_ipmb_addr)
    {
      lprintf(LOG_INFO, "Discovered Target IPMB-0 address %#x",
          m_ipmiInterface->target_ipmb_addr);
    }
  }

  lprintf(LOG_DEBUG, "Interface address: my_addr %#x "
      "transit %#x:%#x target %#x:%#x "
      "ipmb_target %#x\n", m_ipmiInterface->my_addr,
      m_ipmiInterface->transit_addr, m_ipmiInterface->transit_channel,
      m_ipmiInterface->target_addr, m_ipmiInterface->target_channel,
      m_ipmiInterface->target_ipmb_addr);

  /* parse local SDR cache if given */
//  if (sdrcache != NULL)
//  {
//    ipmi_sdr_list_cache_fromfile(ipmi_main_intf, sdrcache);
//  }
//  /* Parse SEL OEM file if given */
//  if (seloem != NULL)
//  {
//    ipmi_oem_setup(ipmi_main_intf);   ipmi_sel_oem_init(seloem);
//  }
//  /* Enable Big Buffer when requested */
//  if ( my_long_packet_size != 0 ) {
//    /* Enable Big Buffer when requested */
//    if (!ipmi_oem_active( ipmi_main_intf,"kontron") ||
//        ipmi_main_intf->ipmi_kontronoem_set_large_buffer(my_long_packet_size ) == 0) {
//      printf("Setting large buffer to %i\n",my_long_packet_size);
//      my_long_packet_set = 1;
//      ipmi_main_intf->ipmi_intf_set_max_request_data_size(
//          my_long_packet_size);
//    }
//  }
//  ipmi_main_intf->cmdlist = cmdlist;
//  memcpy(IntfPatt,ipmi_main_intf,sizeof (struct ipmi_intf));
  if (m_ipmiInterface > 0)
  {
    if (m_ipmiInterface->opened != 0)
    {
      m_ipmiInterface->chassis = new ipmi_chassis(m_ipmiInterface);
      m_state = inited;
      m_servName = string(DimServerName) + "/" + m_Name + "/power_status";
      if (m_service == 0)
        m_service = new DimService(m_servName.c_str(), m_svalue);

      m_servName =  string(DimServerName) + "/" + m_Name + "/power_status_timestamp";
      if (m_lastUpdate == 0)
        m_lastUpdate = new DimService(m_servName.c_str(), m_svalue);

      m_cmdName = string(DimServerName) + "/" + m_Name + "/power_switch";
      if (m_cmd == 0)
        m_cmd = new DimCommand(m_cmdName.c_str(), "C");
      rc = 0;
    }
  }
  else
  {
    rc = 1;
  }
  return rc;
}
