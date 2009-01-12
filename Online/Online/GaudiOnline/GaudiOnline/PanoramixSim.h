// $Id: PanoramixSim.h,v 1.1 2009-01-12 16:31:00 frankb Exp $
#ifndef GAUDISVC_PANORAMIXSIM_H
#define GAUDISVC_PANORAMIXSIM_H 1

// Framework includes
#include "GaudiOnline/EventRunable.h"

/*
 *   LHCb namespace declaration
 */
namespace LHCb  {

  /** Class definition of PanoramixSim.
    *
    * This is the online extension of the runnable of the application manager.
    * The runable handles the actual run() implementation of the
    * ApplicationMgr object.
    * This implementation just calls ApplicationMgr::nextEvent(...).
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class PanoramixSim : public EventRunable  {

    int prompt();
    StatusCode reinit();
  public:
    /// Standard Constructor
    PanoramixSim(const std::string& nam, ISvcLocator* svcLoc);
    /// Standard Destructor
    virtual ~PanoramixSim();
    /// IRunable implementation : Run the class implementation
    virtual StatusCode run();
  };
}      // End namespace LHCb
#endif // GAUDISVC_PANORAMIXSIM_H
