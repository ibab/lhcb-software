// $Id: TrackRemoveDoubleITHits.h,v 1.1 2007-09-14 14:19:10 lnicolas Exp $
#ifndef _TrackRemoveDoubleITHits_H_
#define _TrackRemoveDoubleITHits_H_

/** @class TrackRemoveDoubleITHits TrackRemoveDoubleITHits Align/AlignTrTools/TrackRemoveDoubleITHits.h
 *
 *  Removes Double hits in IT ladders
 *
 *  @author L. Nicolas
 *  @date   14/09/2007
 */

//===========================================================================
// Includes
//===========================================================================
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/Track.h"

// STDet
#include "STDet/DeITDetector.h"
//===========================================================================

class TrackRemoveDoubleITHits: public GaudiAlgorithm {
  friend class AlgFactory<TrackRemoveDoubleITHits>;

public:

  // Constructors and destructor
  TrackRemoveDoubleITHits(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TrackRemoveDoubleITHits();  

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  //======================================================================
  DeITDetector* m_itTracker;           ///< Pointer to the IT XML geom
  std::string   m_itTrackerPath;       ///< Name of the IT XML geom path
  const LHCb::Tracks* m_tracks;
  std::string m_tracksPath;
  //======================================================================

};

#endif // _TrackRemoveDoubleITHits_H_
