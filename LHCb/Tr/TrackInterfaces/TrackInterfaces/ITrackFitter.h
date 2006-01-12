// $Id: ITrackFitter.h,v 1.7 2006-01-12 15:24:26 erodrigu Exp $
#ifndef TRACKINTERFACES_ITRACKFITTER_H 
#define TRACKINTERFACES_ITRACKFITTER_H 1

// Include files
// -------------
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class Track;
};

static const InterfaceID IID_ITrackFitter ( "ITrackFitter", 1, 0 );

/** @class ITrackFitter ITrackFitter.h TrackInterfaces/ITrackFitter.h
 *  
 *  Interface for a track fitting tool.
 *
 *  @author Jose A. Hernando, Eduardo Rodrigues
 *  @date   2005-05-25
 *
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */
class ITrackFitter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackFitter; }

  //! fit a track 
  virtual StatusCode fit( LHCb::Track& track ) = 0;

};
#endif // TRACKINTERFACES_ITRACKFITTER_H
