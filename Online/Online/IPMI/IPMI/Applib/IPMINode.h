#ifndef IPMISRV_H
#define IPMISRV_H
#include <string>
using namespace std;
extern "C"
{
  #include <ipmitool/ipmilight.h>
}
enum IPMIState
{
  uninited,
  inited,
  on,
  off
};
enum PowerState
{
  Power_unknown,
  Power_on,
  Power_off
};
class IPMINode
{
  public:
    string m_Name;
    string m_username;
    string m_password;
    struct ipmi_intf *m_ipmiInterface;
    IPMINode (string &nname,string &uname,string &passw);
    ~IPMINode();
    IPMIState m_state;
    PowerState m_pstate;
    int turnOn();
    int turnOff();
    int getPowerState();
    int setup();
    int shutdown();
};
#endif
