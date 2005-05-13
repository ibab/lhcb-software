// $Id: ITrackPtKick.h,v 1.1 2005-05-13 12:57:52 erodrigu Exp $
#ifndef TRACKTOOLS_ITRACKPTKICK_H 
#define TRACKTOOLS_ITRACKPTKICK_H 1

// Include files
// -------------
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward declarations
class State;

static const InterfaceID IID_ITrackPtKick ( "ITrackPtKick", 1, 0 );

/** @class ITrackPtKick ITrackPtKick.h TrackTools/ITrackPtKick.h
 *  
 *
 *  @author Marco Cattaneo
 *  @date   2002-01-19
 */
class ITrackPtKick : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackPtKick; }

  // Estimate the momentum P of a State
  virtual StatusCode calculate( State* State ) const = 0;

protected:

private:

};
#endif // TRACKTOOLS_ITRACKPTKICK_H
