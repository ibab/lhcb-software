// $Id: IPatDebugTrackTool.h,v 1.1 2009-04-01 08:23:10 ocallot Exp $
#ifndef PATKERNEL_IPATDEBUGTRACKTOOL_H 
#define PATKERNEL_IPATDEBUGTRACKTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"

static const InterfaceID IID_IPatDebugTrackTool ( "IPatDebugTrackTool", 1, 0 );

/** @class IPatDebugTrackTool IPatDebugTrackTool.h PatKernel/IPatDebugTrackTool.h
 *  Interface to get truth information on a track
 *
 *  @author Olivier Callot
 *  @date   2009-03-31
 */
class IPatDebugTrackTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatDebugTrackTool; }

  virtual std::vector<int> keys( const LHCb::Track* track ) = 0;

protected:

private:

};
#endif // PATKERNEL_IPATDEBUGTRACKTOOL_H
