// $Id: $
#ifndef PRKERNEL_IPRDEBUGTOOL_H 
#define PRKERNEL_IPRDEBUGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/LHCbID.h"

static const InterfaceID IID_IPrDebugTool ( "IPrDebugTool", 1, 0 );

/** @class IPrDebugTool IPrDebugTool.h PrKernel/IPrDebugTool.h
 *  Interface to the Pattern debug tool for the upgrade
 *
 *  @author Olivier Callot
 *  @date   2012-03-22
 */
class IPrDebugTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrDebugTool; }

  virtual bool matchKey( LHCb::LHCbID id, int key ) = 0;

  virtual void printKey( MsgStream& msg, LHCb::LHCbID id ) = 0;
  
};
#endif // PRKERNEL_IPRDEBUGTOOL_H
