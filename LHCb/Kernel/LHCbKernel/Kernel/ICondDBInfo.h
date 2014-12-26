#ifndef DETCOND_ICONDDBINFO_H
#define DETCOND_ICONDDBINFO_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "Kernel/CondDBNameTagPair.h"

/** @class ICondDBInfo ICondDBInfo.h DetCond/ICondDBInfo.h
 *
 *  Interface class to obtain information about CondDB
 *
 *  @author Marco Clemencic
 *  @author Marco Cattaneo
 *  @date   2007-05-10
 */
class ICondDBInfo : virtual public IInterface {
public:
  /// InterfaceID
  DeclareInterfaceID(ICondDBInfo, 2, 0);

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags) const = 0;

};
#endif // DETCOND_ICONDDBINFO_H
