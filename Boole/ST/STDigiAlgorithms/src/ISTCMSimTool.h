// $Id: ISTCMSimTool.h,v 1.1 2010-04-07 09:27:37 mneedham Exp $
#ifndef ISTCMSIMTOOL_H
#define ISTCMSIMTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class ISTCMSimTool ISTCMSimTool.h
 *
 *  Interface Class for simulating pedestal
 *
 *  @author M.Needham
 *  @date   14/3/2010
 */

// Declaration of the interface ID ( interface id, major version, minor version)
 
namespace LHCb{
  class STChannelID;
}

static const InterfaceID IID_ISTCMSimTool("ISTCMSimTool",0 ,0);

class ISTCMSimTool : virtual public IAlgTool {

public: 

   /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTCMSimTool; }
   
   /// calc sharinh
  virtual double noise(const LHCb::STChannelID& chan) const=0;

};

#endif // ISTCMSimTool_H


