// $Id: RichRecRecoTracks.h,v 1.1.1.1 2002-07-28 10:46:21 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECRECOTRACKS_H 
#define RICHRECTOOLS_RICHRECRECOTRACKS_H 1

// Fram Gaudi framework
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// From tracking
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"

// From Rich Event model
#include "Event/RichRecTrack.h"

// Rich Detector
#include "RichDetTools/IRichDetInterface.h"

#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecRecoTracks RichRecRecoTracks.h 
 *
 *  Algorithm for forming RichRecTracks from reconstructed tracks
 *
 *  @author Christopher.Rob.Jones@cern.ch
 *  created Fri Mar 15 11:10:17 2002
 *
 */

class RichRecRecoTracks : public Algorithm {

public:

  /// Standard algorithm constructor
  RichRecRecoTracks (const std::string &name,
                     ISvcLocator *svcLoc);

  /// Default destructor
  ~RichRecRecoTracks();

  /// Gaudi framework initialize method
  virtual StatusCode initialize ();

  /// Gaudi framework execute method
  virtual StatusCode execute ();

  /// Gaudi framework finalize method
  virtual StatusCode finalize ();

private:

  /// Default constructor
  RichRecRecoTracks();

  /// Default copy constructor
  RichRecRecoTracks(const RichRecRecoTracks &right);

  /// Default copy constructor
  RichRecRecoTracks & operator=(const RichRecRecoTracks &right);

  /// String containing input reconstructed tracks location in TDS
  std::string m_recoTracksLocation;

  /// String containing output RichRecTrack location in TDS
  std::string m_richRecTracksLocation;

  /// RichRecSegment location in TDS
  std::string m_richRecSegmentLocation;

  /// Pointer to RichDetInterface tool
  IRichDetInterface * m_richDetInterface;

};

#endif // RICHRECTOOLS_RICHRECRECOTRACKS_H 
