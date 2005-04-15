#ifndef TRACKTOOLS_TRACKSELECTOR_H 
#define TRACKTOOLS_TRACKSELECTOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Event
#include "Event/MCParticle.h"
#include "Event/Track.h"

// local
#include "TrackTools/ITrackSelector.h"

// Forward declarations
class ITrackReconstructible;

/** @class TrackSelector TrackSelector.h "TrackTools/TrackSelector.h"
 *
 *  This tool selects Tracks and MCParticles based on certain criteria.
 *  These criteria, which can be set with job-options, are:
 *  - uniqueFlag:  if true then only unique tracks are selected.
 *  - errorFlag:   if true then tracks which had an error are not selected.
 *  - minP:        minimum momentum of the track or MCParticle.
 *  - maxP:        minimum momentum of the track or MCParticle.
 *  - tracktypes:  a list of tracktypes to be selected for the tracks, but also
 *                 the MCParticles (see also below). If it is left blank then
 *                 all track types are selected.
 *  - mcParticles: the criterium-tool for MCParticles (e.g. TrReconstructible or
 *                 TrAcceptance).
 *  The type of a track is a local identifyer for the different track types.
 *  It is also defined as a enum (ETrackType):
 *  0 = unknown 
 *  1 = velo
 *  2 = seed
 *  3 = longtrack
 *  4 = upstream
 *  5 = downstream
 *  6 = veloBack
 *
 *  @author Jeroen van Tilburg
 *  @date   2003-07-28
 */

class TrackSelector : public GaudiTool, virtual public ITrackSelector
{
public:

  /// Enum for the different track type
  enum ETrackType { unknown=0, velo=1, seed=2, longtrack=3, 
                    upstream=4, downstream=5, veloBack=6 }; 

  /// Constructor
  TrackSelector( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent );

  /// Destructor
  virtual ~TrackSelector( ); 

  /// Tool initialization
  virtual StatusCode initialize();

  /// Tool finalization
  virtual StatusCode finalize();

  /** Select the Track.
   *  @return True if the track is selected; false otherwise.
   *  @param  track Input track, which is checked for selection.
   */
  virtual bool select( Track* track ) const;

  /** Select the MCParticle.
   *  @return True if the MCParticle is selected; false otherwise.
   *  @param  mcParticle Input MCParticle, which is checked for selection. 
   */
  virtual bool select( MCParticle* mcParticle );

  /** Select the Track only by tracktype, unique and error-flag .
   *  @return True if the track is selected; false otherwise.
   *  @param  track Input track, which is checked for selection.
   */
  virtual bool selectTrackType( Track* track ) const;

  /** Select the MCParticle only by tracktype.
   *  @return True if the MCParticle is selected; false otherwise.
   *  @param  mcParticle Input MCParticle, which is checked for selection.
   */
  virtual bool selectTrackType( MCParticle* mcParticle );

  /** Get the local tracktype identifyer of the Track.
   *  @return Local tracktype identifyer.
   *  @param  track Input track.
   */
  virtual int getTrackType( Track* track ) const;

  /** Get the local tracktype identifyer of the MCParticle.
   *  @return Local tracktype identifyer.
   *  @param  mcPart Input MCParticle.
   */
  virtual int getTrackType( MCParticle* mcPart );

  /** Set the tracktype of a Track.
   *  @return StatusCode.
   *  @param  tracktype Input local tracktype identifyer of the track.
   *  @param  track     Track of which the tracktype will be set.
   */
  virtual StatusCode setTrackType( const int tracktype, 
                                   Track*& track ) const;

  /** Set the tracktype of a (true) Track with an MCParticle  
   *  @return StatusCode.
   *  @param  mcPart MCParticle which will determine type of the (true) track.
   *  @param  track  Track of which the type will be set.
   */
  virtual StatusCode setTrackType( MCParticle* mcPart,
                                   Track*& track );

protected:

private:

  // Acceptance/reconstructible tool
  ITrackReconstructible* m_mcParticleJudge; ///< Pointer to MCParticle judge

  // Store previously calculated values
  ETrackType m_previousTrackType;   ///< Previous tracktype identifier
  MCParticle* m_previousMCParticle; ///< Previously requested MCParticle

  // job options
  bool m_uniqueFlag;                ///< To monitor unique tracks only  
  bool m_errorFlag;                 ///< To monitor only correctly fitted tracks
  double m_minP;                    ///< minimum momentum for the tracks
  double m_maxP;                    ///< maximum momentum for the tracks
  std::vector<int> m_tracktypes;    ///< Track types of the monitored tracks
  /// Name of the tool to decide which MCParticles should have been found
  std::string m_mcParticleJudgeName;

};
#endif // TRACKTOOLS_TRACKSELECTOR_H 
