// $Id: TrackCriteriaSelector.h,v 1.5 2006-03-06 18:58:17 erodrigu Exp $
#ifndef TRACKMCTOOLS_TRACKCRITERIASELECTOR_H 
#define TRACKMCTOOLS_TRACKCRITERIASELECTOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Event
#include "Event/MCParticle.h"
#include "Event/Track.h"

// from TrackMCInterfaces
#include "TrackMCInterfaces/ITrackCriteriaSelector.h"
#include "TrackMCInterfaces/ITrackReconstructible.h"

using namespace LHCb;

/** @class TrackCriteriaSelector TrackCriteriaSelector.h "TrackMCTools/TrackCriteriaSelector.h"
 *
 *  This tool selects Tracks and MCParticles based on certain criteria.
 *  These criteria, which can be set with job options, are:
 *  - UniqueFlag:  if true then only unique tracks are selected
 *  - ValidFlag:   if true then only valid tracks are selected
 *  - MinP:        minimum momentum of the track or MCParticle
 *  - MaxP:        minimum momentum of the track or MCParticle
 *  - TrackTypes:  a list of track types to be selected for the tracks,
 *                 but also the MCParticles (see also below).
 *                 If it is left blank then all track types are selected
 *  - MCParticles: the criterium-tool for MCParticles
 *                 (e.g. TrackReconstructible or TrackAcceptance)
 *  The type of a track is taken from the available types defined
 *  in Track.h
 *
 *  Moved to LHCb v20r0. Adapted code to use updated Det packages.
 *  @author Edwin Bos
 *  @date   2006-02-02
 *
 *  2005-05-04 : Eduardo Rodrigues (adaptations to new track event model)
 *
 *  @author Jeroen van Tilburg
 *  @date   2003-07-28
 */

class TrackCriteriaSelector : public GaudiTool,
                              virtual public ITrackCriteriaSelector
{
public:
  /// Constructor
  TrackCriteriaSelector( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent );

  /// Destructor
  virtual ~TrackCriteriaSelector( ); 

  /// Tool initialization
  virtual StatusCode initialize();

  /** Select the Track.
   *  @return True, if the track is selected; false otherwise.
   *  @param  track, Input track, which is checked for selection.
   */
  virtual bool select( Track* track ) const;

  /** Select the MCParticle.
   *  @return True, if the MCParticle is selected; false otherwise.
   *  @param  mcParticle, Input MCParticle, which is checked for selection.
   */
  virtual bool select( MCParticle* mcParticle );

  /** Select the Track only by track type, unique- and valid-flag .
   *  @return True, if the track is selected; false otherwise.
   *  @param  track, Input track, which is checked for selection.
   */
  virtual bool selectByTrackType( Track* track ) const;

  /** Select the MCParticle only by track type.
   *  @return True, if the MCParticle is selected; false otherwise.
   *  @param  mcParticle, Input MCParticle, which is checked for selection.
   */
  virtual bool selectByTrackType( MCParticle* mcParticle );

  /** Get the track type identifyer of the MCParticle.
   *  @return Track type identifyer as defined in Track.h
   *  @param  mcParticle, Input MCParticle.
   */
  virtual unsigned int trackType( MCParticle* mcPart );

  /** Set the track type of a Track with an MCParticle's type.
   *  @return StatusCode.
   *  @param  mcPart, MCParticle which will determine the type of the (true) Track.
   *  @param  track,  Track of which the type will be set.
   */
  virtual StatusCode setTrackType( MCParticle* mcPart,
                                   Track*& track );

private:

  // Reconstructibility tool
  ITrackReconstructible* m_mcParticleJudge; ///< Pointer to MCParticle judge

  // Store previously calculated values
  int         m_previousTrackType;  ///< Previous track type identifier
  MCParticle* m_previousMCParticle; ///< Previously requested MCParticle

  // Job options
  bool m_uniqueFlag;                ///< To monitor unique tracks only  
  bool m_validFlag;                 ///< To monitor only correctly fitted tracks
  double m_minP;                    ///< minimum momentum for the tracks
  double m_maxP;                    ///< maximum momentum for the tracks
  std::vector<int> m_tracktypes;    ///< Track types of the monitored tracks
  /// Name of the tool to decide which MCParticles should have been found
  std::string m_mcParticleJudgeName;

};
#endif // TRACKMCTOOLS_TRACKCRITERIASELECTOR_H 
