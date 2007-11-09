// $Id: IPatVeloDebugTool.h,v 1.1 2007-11-09 07:48:44 ocallot Exp $
#ifndef TRACKINTERFACES_IPATVELODEBUGTOOL_H 
#define TRACKINTERFACES_IPATVELODEBUGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/LHCbID.h"

static const InterfaceID IID_IPatVeloDebugTool ( "IPatVeloDebugTool", 1, 0 );

/** @class IPatVeloDebugTool IPatVeloDebugTool.h TrackInterfaces/IPatVeloDebugTool.h
 *  Interface for debug tool in PatVelo
 *
 *  @author Olivier Callot
 *  @date   2007-10-25
 */
class IPatVeloDebugTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatVeloDebugTool; }

  virtual bool matchKey( LHCb::LHCbID& id, int key ) = 0;

  virtual void printKey( MsgStream& msg, LHCb::LHCbID& id ) = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IPATVELODEBUGTOOL_H
