// $Id: TrackRemoveDoubleHits.h,v 1.2 2008-05-26 15:45:59 lnicolas Exp $
#ifndef _TrackRemoveDoubleHits_H_
#define _TrackRemoveDoubleHits_H_

/** @class TrackRemoveDoubleHits TrackRemoveDoubleHits Tr/TrackUtils/TrackRemoveDoubleHits.h
 *
 *  Removes Double hits in TT or IT ladders or in OT modules
 *
 *  @author L. Nicolas
 *  @date   14/09/2007
 */

//===========================================================================
// Includes
//===========================================================================
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/Track.h"
#include "Event/Node.h"
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Event/OTTime.h"
//===========================================================================

class TrackRemoveDoubleHits: public GaudiAlgorithm {
  friend class AlgFactory<TrackRemoveDoubleHits>;

public:

  // Constructors and destructor
  TrackRemoveDoubleHits(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~TrackRemoveDoubleHits();  

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  bool isHighThreshold ( const LHCb::LHCbID& theLHCbID );
  double charge ( const LHCb::LHCbID& theLHCbID );

  int m_nTTHits;
  int m_nITHits;
  int m_nOTHits;

private:

  //======================================================================
  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;

  const LHCb::Tracks* m_tracks;
  std::string m_tracksPath;

  bool m_keepHighThreshold;
  bool m_keepHighCharge;

  // TT stuff
  std::string m_ttLiteClustersPath;
  const STLiteClusters* m_ttLiteClusters;
  std::string m_ttClustersPath;
  const LHCb::STCluster::Container* m_ttClusters;

  // IT stuff
  std::string m_itLiteClustersPath;
  const STLiteClusters* m_itLiteClusters;
  std::string m_itClustersPath;
  const LHCb::STCluster::Container* m_itClusters;

  // OT stuff
  std::string m_otTimesPath;
  const LHCb::OTTimes m_otTimes;
  //======================================================================

};

#endif // _TrackRemoveDoubleHits_H_
