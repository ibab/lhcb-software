// $Id: ICondDBCnvSvc.h,v 1.4 2005-10-18 15:40:50 marcocle Exp $
#ifndef DETCOND_ICONDDBCNVSVC_H 
#define DETCOND_ICONDDBCNVSVC_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IInterface.h"

static const InterfaceID IID_ICondDBCnvSvc ( "ICondDBCnvSvc", 1, 0 );

class ICondDBAccessSvc;

/** @class ICondDBCnvSvc ICondDBCnvSvc.h DetCond/ICondDBCnvSvc.h
 *  
 *  Interface to access the special methods of CondDBCnvSvc.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-14
 */
class ICondDBCnvSvc : virtual public IInterface {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICondDBCnvSvc; }

  /// Return a reference to the known list of access services.
  virtual std::vector<ICondDBAccessSvc*> &accessServices() = 0;
  /// Return a reference to the known list of access services. (const version)
  virtual const std::vector<ICondDBAccessSvc*> &accessServices() const = 0;

protected:

private:

};
#endif // DETCOND_ICONDDBCNVSVC_H
