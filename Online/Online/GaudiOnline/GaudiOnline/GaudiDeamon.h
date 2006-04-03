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
#if 0
/// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IAppMgrUI; }
  /// Configure the job
  virtual StatusCode configure() = 0;
  /// Terminate the job
  virtual StatusCode terminate() = 0;
  /// Initialize the job
  virtual StatusCode initialize() = 0;
  /// Finalize the job
  virtual StatusCode finalize() = 0;
  /// Process the next maxevt events
  virtual StatusCode nextEvent(int maxevt) = 0;

  /// The identifying name of the AppMgrUI object.
  virtual const std::string& name() const = 0;
  // current state of the AppMgrUI object
  virtual const std::string& stateName()  const = 0;
#endif
