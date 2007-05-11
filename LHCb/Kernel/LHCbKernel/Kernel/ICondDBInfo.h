// $Id: ICondDBInfo.h,v 1.1 2007-05-11 10:02:51 marcocle Exp $
#ifndef DETCOND_ICONDDBINFO_H 
#define DETCOND_ICONDDBINFO_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "Kernel/CondDBNameTagPair.h"

static const InterfaceID IID_ICondDBInfo ( "ICondDBInfo", 1, 0 );

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

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICondDBInfo; }

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */ 
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags) const = 0;

};
#endif // DETCOND_ICONDDBINFO_H
