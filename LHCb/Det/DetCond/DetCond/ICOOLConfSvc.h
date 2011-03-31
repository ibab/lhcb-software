#ifndef DETCOND_ICOOLCONFSVC_H
#define DETCOND_ICOOLCONFSVC_H 1

// Include files
#include <GaudiKernel/IInterface.h>

// Forward declarations
namespace coral {
  class IConnectionService;
}
namespace cool {
  class IDatabaseSvc;
}

/** @class ICOOLConfSvc ICOOLConfSvc.h DetCond/ICOOLConfSvc.h
 *
 *  Class used as interface to instantiate a COOL application and configure it
 *  (and CORAL).
 *
 *  @author Marco CLEMENCIC
 *  @date   2007-12-07
 */
class ICOOLConfSvc : virtual public IInterface {
public:
  /// InterfaceID
  DeclareInterfaceID(ICOOLConfSvc, 3, 0);

  /// Access to the CORAL connection service used by COOL (if needed).
  virtual coral::IConnectionService& connectionSvc() = 0;

  /// Get the COOL Database service (used to connect to the databases).
  virtual cool::IDatabaseSvc& databaseSvc() = 0;

};

#endif // DETCOND_ICOOLCONFSVC_H
