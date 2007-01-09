// $Id: ISTChargeSharingTool.h,v 1.2 2007-01-09 15:34:30 jvantilb Exp $
#ifndef ISTCHARGESHARINGTOOL_H
#define ISTCHARGESHARINGTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class ISTChargeSharingTool ISTChargeSharingTool.h
 *
 *  Interface Class for charge sharing ST strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ISTChargeSharingTool("ISTChargeSharingTool",0 ,0);

class ISTChargeSharingTool : virtual public IAlgTool {

public: 

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTChargeSharingTool; }
   
   /// calc sharinh
  virtual double sharing(const double relDist) const=0;

};

#endif // ISTCHARGESHARINGTOOL_H
