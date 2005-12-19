// $Id: ISTChargeSharingTool.h,v 1.1.1.1 2005-12-19 15:43:16 mneedham Exp $
#ifndef _ISTCHARGESHARINGTOOL_H
#define _ISTCHARGESHARINGTOOL_H

#include "GaudiKernel/IAlgTool.h"

/** @class ISTChargeSharingTool ISTChargeSharingTool.h ITAlgorithms/ISTChargeSharingTool.h
 *
 *  Interface Class for charge sharing IT strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ISTChargeSharingTool("ISTChargeSharingTool", 0 , 0); 

class ISTChargeSharingTool : virtual public IAlgTool {

public: 

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTChargeSharingTool; }
   
   /// calc sharinh
  virtual double sharing(const double relDist) const=0;

};

#endif // _ISTCHARGESHARINGTOOL_H



