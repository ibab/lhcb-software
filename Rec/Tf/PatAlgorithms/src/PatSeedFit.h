// $Id: PatSeedFit.h,v 1.3 2009-11-10 14:21:26 wouter Exp $
#ifndef INCLUDE_PATSEEDFIT_H
#define INCLUDE_PATSEEDFIT_H 1

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrackInterfaces/IPatSeedFit.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "Event/STLiteCluster.h"

static const InterfaceID IID_PatSeedFit("PatSeedFit", 1, 0);

// forward declarations
class PatSeedTool;
namespace Tf {
  class IOTHitCreator;
}
class PatForwardHit;
typedef PatForwardHit PatFwdHit;

/** @class PatSeedFit PatSeedFit.h
 *  
 * provide a convenient interface to the internal fit used in the PatSeeding
 * algorithm in the pattern reco
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2009-01-28
 */
class PatSeedFit : public GaudiTool, virtual public IPatSeedFit, virtual public IIncidentListener {

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatSeedFit; }

  /// Standard Constructor
  PatSeedFit(const std::string& type,
             const std::string& name,
             const IInterface* parent);

  virtual ~PatSeedFit(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize

  virtual StatusCode fitSeed( const std::vector<LHCb::LHCbID> lhcbIDs, 
			      std::vector<LHCb::State> *states) const;
  /// fit a PatSeedTrack
  virtual StatusCode fitTrack( PatSeedTrack& track, double maxChi2,
		  unsigned minPlanes, bool xOnly, bool forceDebug ) const; 

private:
  /// patseed tool (handles the actual track fit)
  PatSeedTool* m_patSeedTool;
  /// IT detector
  DeSTDetector* m_itDet ;
  /// OT hit creator
  const Tf::IOTHitCreator* m_othitcreator;
  /// momentum estimator
  const ITrackMomentumEstimate *m_momentumTool ;
  /// magnetic field service
  ILHCbMagnetSvc*  m_magFieldSvc;
  /// cache for STLiteClusters (so we do not have to reget them in every event)
  mutable LHCb::STLiteCluster::FastContainer* m_stLiteContainer;

  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_dRatio;
  double m_initialArrow;
  double m_maxChi2;
  double m_zReference;
  double m_momentumScale;

  /// return a track constructed from an ITOT stub
  /** construct a track from an ITOT stub
   * @param hits   list of hits from which to construct the track
   * @param staIT  station in IT which has enough hits to form a stub
   *               (if -1, such a station is unknown or unavailable, and
   *               the method falls back on using whatever hits are available)
   * @returns a PatSeedTrack
   */
  PatSeedTrack getTrackITOT(std::vector<PatFwdHit>& hits, int staIT = -1) const;
  /// return a track constructed by fitting the x only projection first then y
  /** construct a track from x hits in three different stations, then adding
   * stereo hits
   * @param hits   list of hits from which to construct the track
   * @returns a PatSeedTrack
   */
  PatSeedTrack getTrackXY(std::vector<PatFwdHit>& hits) const;

  /// make an OT cluster from ihit and a suitable one in hits (if possible)
  void makeCluster(std::vector<PatFwdHit>& hits, PatFwdHit& ihit,
      const PatFwdHit*& in, double& x, double& z, bool& isCluster) const;

  /// handle incidents (specifically clear m_stLiteContainer at EndEvent)
  virtual void handle(const Incident& incident);
};
#endif // INCLUDE_PATSEEDFIT_H

