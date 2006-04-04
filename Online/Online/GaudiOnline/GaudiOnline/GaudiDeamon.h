#ifndef ONLINEKERNEL_GaudiDeamon_H
#define ONLINEKERNEL_GaudiDeamon_H

// Framework includes
#include "GaudiOnline/DimTaskFSM.h"
// Forward declarations
class IAppMgrUI;


/*
 * LHCb namespace declaration
 */
namespace LHCb  {
  /** @class GaudiDeamon GaudiDeamon.h GaudiOnline/GaudiDeamon.h
    *
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class GaudiDeamon : public DimTaskFSM  {
  protected:
    /// Main appliation manager object
    IAppMgrUI*  m_appMgr;
    /// Property: name of runable object
    std::string m_runable;
    /// Property: name of event loop object
    std::string m_evtloop;
    /// Property: name of message service object
    std::string m_msgsvcType;
    /// Property: main jonb options
    std::string m_mainOptions;

  public:
    GaudiDeamon(IInterface*);
    virtual ~GaudiDeamon();
    /// Run the complete job (from intialize to terminate)
    virtual StatusCode run();
    virtual StatusCode unload();
  };
}
#endif // ONLINEKERNEL_GaudiDeamon_H
