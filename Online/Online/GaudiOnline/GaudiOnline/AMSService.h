//	============================================================
//
//	AMSService.h
//	------------------------------------------------------------
//
//	Package   : GaudiOnline
//
//	Author    : Markus Frank
//
//	===========================================================
#ifndef GAUDIONLINE_AMSSERVICE_H
#define GAUDIONLINE_AMSSERVICE_H 1

// Framework include files
#include "GaudiKernel/Service.h"

// Forward declarations
class ISvcLocator;

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class AMSService AMSService.h GaudiOnline/AMSService.h
    *
    *  @author Markus Frank
    *  @version 1.0
    */
  class AMSService : public Service   {
  public:
    /// Standard Constructor
    AMSService(const std::string& name, ISvcLocator* svc);
    /// Standard Destructor
    virtual ~AMSService();
    /// IService implementation: Initialize the service
    virtual StatusCode initialize();
    /// IService implementation: Finalize the service
    virtual StatusCode finalize();
  };
}      // End namespace LHCb
#endif //  GAUDIONLINE_AMSSERVICE_H
