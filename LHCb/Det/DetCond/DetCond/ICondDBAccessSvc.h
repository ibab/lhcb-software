// $Id: ICondDBAccessSvc.h,v 1.1 2005-02-09 08:30:53 marcocle Exp $
#ifndef DETCOND_ICONDDBACCESSSVC_H 
#define DETCOND_ICONDDBACCESSSVC_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include <GaudiKernel/IInterface.h>

// from COOL
#include "CoolKernel/types.h"
#include "CoolKernel/IValidityKey.h"

static const InterfaceID IID_ICondDBAccessSvc ( "ICondDBAccessSvc", 1, 0 );

/** @class ICondDBAccessSvc ICondDBAccessSvc.h DetCond/ICondDBAccessSvc.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-01-11
 */
class ICondDBAccessSvc : virtual public IInterface {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICondDBAccessSvc; }

  // Utilities:

  /// Used to obtain direct access to the database.
  virtual cool::IDatabasePtr& database() = 0;

  /*
  /// Retrieve a CondDBObject from the database. Returns NULL if not found.
  virtual cool::IObjectPtr getCondDBObject(const std::string &path,
                                           const cool::IValidityKey &timePoint,
                                           const cool::IChannelId &channelId=0) = 0;
  */

  //  createFolder

protected:

private:

};
#endif // DETCOND_ICONDDBACCESSSVC_H
