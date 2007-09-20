// $Id: TrackRemoveDoubleITHits.h,v 1.2 2007-09-20 15:14:40 lnicolas Exp $
#ifndef _TrackRemoveDoubleITHits_H_
#define _TrackRemoveDoubleITHits_H_

/** @class TrackRemoveDoubleITHits TrackRemoveDoubleITHits Tr/TrackUtils/TrackRemoveDoubleITHits.h
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
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"

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

  bool isHighThreshold ( const LHCb::LHCbID& theLHCbID );
  double charge ( const LHCb::LHCbID& theLHCbID );

private:

  //======================================================================
  DeITDetector* m_itTracker;           ///< Pointer to the IT XML geom
  std::string   m_itTrackerPath;       ///< Name of the IT XML geom path
  const LHCb::Tracks* m_tracks;
  std::string m_tracksPath;

  bool m_keepHighThreshold;
  std::string m_itLiteClustersPath;
  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;
  const STLiteClusters* m_itLiteClusters;

  bool m_keepHighCharge;
  std::string m_itClustersPath;
  const LHCb::STCluster::Container* m_itClusters;
  //======================================================================

};

#endif // _TrackRemoveDoubleITHits_H_
