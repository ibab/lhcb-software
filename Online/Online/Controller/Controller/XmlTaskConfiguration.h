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
#ifndef ONLINE_CONTROLLER_XMLTASKCONFIGURATION_H
#define ONLINE_CONTROLLER_XMLTASKCONFIGURATION_H

// C/C++ include files
#include <string>
#include <vector>

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
    /// String with the partition name
    std::string  m_partition;
    /// String with the name of the configuration file
    std::string  m_config;
    /// String with the location of the run-info python file
    std::string  m_runinfo;
    /// The processing mode (NORMAL,FORKING,CHECKPOINTING)
    std::string  m_mode;
    /// The number of instances to be forked
    int          m_instances;

  public:
    /// Standard constructor
    XmlTaskConfiguration(const std::string& partition, 
			 const std::string& config, 
			 const std::string& runinfo, 
			 const std::string& mode, 
			 int num_moore);
    /// Default destructor
    virtual ~XmlTaskConfiguration();
    /// Analyse the configuration file and attach the corresponding slaves to the FSM machine
    bool attachTasks(Machine& machine, const std::string& slave_type);
    /// Analyse the configuration file and attach the corresponding slaves to the FSM machine
    bool getTasks(std::vector<std::string>& tasks);
  };   //  End class XmlTaskConfiguration
}      //  End namespace 
#endif //  ONLINE_CONTROLLER_XMLTASKCONFIGURATION_H

