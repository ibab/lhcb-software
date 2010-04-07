// $Id: ISTPedestalSimTool.h,v 1.1 2010-04-07 09:27:37 mneedham Exp $
#ifndef ISTPEDESTALSIMTOOL_H
#define ISTPEDESTALSIMTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class ISTChargeSharingTool ISTChargeSharingTool.h
 *
 *  Interface Class for charge sharing ST strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ISTPedestalSimTool("ISTPedestalSimTool",0 ,0);

namespace LHCb{
  class STChannelID;
}

class ISTPedestalSimTool : virtual public IAlgTool {

public: 

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTPedestalSimTool; }
   
   /// calc sharinh
  virtual double pedestal(const LHCb::STChannelID& chan) const=0;

};

#endif // ISTPedestalSimTool_H


