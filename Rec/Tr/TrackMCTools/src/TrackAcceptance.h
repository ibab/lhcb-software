// $Id: TrackAcceptance.h,v 1.8 2007-05-31 15:21:38 cattanem Exp $
#ifndef TRACKMCTOOLS_TRACKACCEPTANCE_H
#define TRACKMCTOOLS_TRACKACCEPTANCE_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Kernel/MCInterfaces
#include "MCInterfaces/ITrackReconstructible.h"

// Det
#include "VeloDet/DeVelo.h"
#include "STDet/DeSTStation.h"
#include "OTDet/DeOTStation.h"

namespace LHCb
{
  class MCParticle;
  class MCHit;
}

/** @class TrackAcceptance TrackAcceptance.h
 *
 *  TrackAcceptance is a ITrackReconstructible tool. It can be used
 *  to ask whether a MCParticle is in the @em tracking-acceptance. 
 *  An MCParticle is in the acceptance when it has left more than a certain
 *  amount of MCHits in a part of the tracking system. These cuts can be set
 *  with job-options. The default cuts are:
 *   - minNVeloRHits = 3
 *   - minNVeloPhiHits = 3
 *   - minNTTHits = 1
 *   - minNT1Hits = 1
 *   - minNT2Hits = 1
 *   - minNT3Hits = 1
 *
 *  Moved to LHCb v20r0. Adapted code to use updated Det packages.
 *  @author Edwin Bos
 *  @date   2006-02-02
 *
 *  @author Jeroen van Tilburg
 *  @date   2003-07-04
 */

class TrackAcceptance: public GaudiTool,
                       virtual public ITrackReconstructible {
public:

  /// Typedefs
  //  typedef IAssociator<MCParticle, MCHit> MCHitAsct;

  /// Constructor
  TrackAcceptance( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Default destructor
  virtual ~TrackAcceptance();

  /// Tool initialization
  virtual StatusCode initialize();

  /** This method checks if the MCParticle is in the Velo acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasVelo        ( const LHCb::MCParticle* mcPart ) const ;

  /** This method checks if the MCParticle is in the TT acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasTT          ( const LHCb::MCParticle* mcPart ) const ;

  /** This method checks if the MCParticle is in the T-station acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasSeed        ( const LHCb::MCParticle* mcPart ) const ;

  /** This method checks if the MCParticle is in the long track acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasVeloAndSeed ( const LHCb::MCParticle* mcPart ) const ;

private:
  /// Velo Detector information
  DeVelo* m_velo;

  // ST Detector information
  DeSTStation* m_TT1Station; ///< Pointer to TT Station 1
  DeSTStation* m_TT2Station; ///< Pointer to TT Station 2
  DeSTStation* m_IT1Station; ///< Pointer to IT Station 1
  DeSTStation* m_IT2Station; ///< Pointer to IT Station 2
  DeSTStation* m_IT3Station; ///< Pointer to IT Station 3

  // OT Detector information
  DeOTStation* m_OT1Station;        ///< Pointer to OT Station 1
  DeOTStation* m_OT2Station;        ///< Pointer to OT Station 2
  DeOTStation* m_OT3Station;        ///< Pointer to OT Station 3

  // job options
  int m_minNVeloRHits;             ///< Minimum number of r velo hits 
  int m_minNVeloPhiHits;           ///< Minimum number of phi velo hits
  int m_minNTTHits;                ///< Minimum number of TT hits
  int m_minNT1Hits;                ///< Minimum number of T1 hits
  int m_minNT2Hits;                ///< Minimum number of T2 hits
  int m_minNT3Hits;                ///< Minimum number of T3 hits

};

#endif // TRACKMCTOOLS_TRACKACCEPTANCE_H
