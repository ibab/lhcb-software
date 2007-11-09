// $Id: IPatDebugTool.h,v 1.1 2007-11-09 09:29:10 cattanem Exp $
#ifndef TRACKINTERFACES_IPATDEBUGTOOL_H 
#define TRACKINTERFACES_IPATDEBUGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class LHCbID;
};

static const InterfaceID IID_IPatDebugTool ( "IPatDebugTool", 1, 0 );

/** @class IPatDebugTool IPatDebugTool.h TrackInterfaces/IPatDebugTool.h
 *  Interface for debug tool in Pat
 *
 *  @author Olivier Callot
 *  @date   2007-10-25
 */
class IPatDebugTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatDebugTool; }

  virtual bool matchKey( LHCb::LHCbID& id, int key ) = 0;

  virtual void printKey( MsgStream& msg, LHCb::LHCbID& id ) = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IPATDEBUGTOOL_H
