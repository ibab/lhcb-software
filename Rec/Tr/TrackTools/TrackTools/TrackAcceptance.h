#ifndef TRACKTOOLS_TRACKACCEPTANCE_H
#define TRACKTOOLS_TRACKACCEPTANCE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// LHCbKernel
#include "Relations/IAssociator.h" 

// Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCVeloHit.h"

// Associators
#include "TrackInterfaces/ITrackReconstructible.h"

// Det
#include "VeloDet/DeVelo.h"
#include "STDet/STDetectionStation.h"
#include "OTDet/DeOTStation.h"

/** @class TrackAcceptance TrackAcceptance.h "TrackTools/TrackAcceptance.h"
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
 *  @author Jeroen van Tilburg
 *  @date   2003-07-04
 */

class TrackAcceptance: public GaudiTool,
                       virtual public ITrackReconstructible {
public:
  /// Typedefs
  typedef IAssociator<MCParticle, MCHit>     MCHitAsct;
  typedef IAssociator<MCParticle, MCVeloHit> MCVeloHitAsct;

  /// Constructor
  TrackAcceptance( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// destructor
  virtual ~TrackAcceptance();

  /// Tool initialization
  virtual StatusCode initialize();

  /** This method checks if the MCParticle is in the Velo acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasVelo        ( MCParticle* mcPart );

  /** This method checks if the MCParticle is in the TT acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasTT          ( MCParticle* mcPart );

  /** This method checks if the MCParticle is in the T-station acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasSeed        ( MCParticle* mcPart );

  /** This method checks if the MCParticle is in the long track acceptance.
   *  @return bool is true if MCParticle is in acceptance
   *  @param  mcPart pointer to the MCParticle
   */
  virtual bool hasVeloAndSeed ( MCParticle* mcPart );

private:

  /// Velo Detector information
  DeVelo* m_velo;

  // ST Detector information
  STDetectionStation* m_TT1Station; ///< Pointer to TT Station 1
  STDetectionStation* m_TT2Station; ///< Pointer to TT Station 2
  STDetectionStation* m_IT1Station; ///< Pointer to IT Station 1
  STDetectionStation* m_IT2Station; ///< Pointer to IT Station 2
  STDetectionStation* m_IT3Station; ///< Pointer to IT Station 3

  // OT Detector information
  DeOTStation* m_OT1Station;        ///< Pointer to OT Station 1
  DeOTStation* m_OT2Station;        ///< Pointer to OT Station 2
  DeOTStation* m_OT3Station;        ///< Pointer to OT Station 3

  // associators
  MCVeloHitAsct* m_p2VeloHitAsct;   ///< Pointer to MCParticle 2 VeloMCHit Asct.
  MCHitAsct* m_p2ITHitAsct;         ///< Pointer to MCParticle 2 IT MCHit Asct.
  MCHitAsct* m_p2OTHitAsct;         ///< Pointer to MCParticle 2 OT MCHit Asct.

  // job options
  std::string m_p2VeloHitAsctName; ///< name of the MCParticle 2 VeloMCHit Asct.
  std::string m_p2ITHitAsctName;   ///< name of the MCParticle 2 IT MCHit Asct.
  std::string m_p2OTHitAsctName;   ///< name of the MCParticle 2 OT MCHit Asct.
  int m_minNVeloRHits;             ///< Minimum number of r velo hits 
  int m_minNVeloPhiHits;           ///< Minimum number of phi velo hits
  int m_minNTTHits;                ///< Minimum number of TT hits
  int m_minNT1Hits;                ///< Minimum number of T1 hits
  int m_minNT2Hits;                ///< Minimum number of T2 hits
  int m_minNT3Hits;                ///< Minimum number of T3 hits
};

#endif // TRACKTOOLS_TRACKACCEPTANCE_H
