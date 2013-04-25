/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_XMLTASKCONFIGURATION_H
#define ONLINE_FINITESTATEMACHINE_XMLTASKCONFIGURATION_H

// C/C++ include files
#include <string>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine   {

  // Forward declarations
  class Machine;

  /**@class XmlTaskConfiguration  XmlTaskConfiguration.h FiniteStateMachine/XmlTaskConfiguration.h
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct XmlTaskConfiguration  {
  protected:
    /// String with the name of the configuration file
    std::string  m_config;
  public:
    /// Standard constructor
    XmlTaskConfiguration(const std::string& config);
    /// Default destructor
    virtual ~XmlTaskConfiguration();
    /// Analyse the configuration file and attach the corresponding slaves to the FSM machine
    bool attachTasks(Machine& machine,  const std::string& partition, const std::string& runinfo, const std::string& mode, int num_moore);
  };   //  End class XmlTaskConfiguration
}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_XMLTASKCONFIGURATION_H

