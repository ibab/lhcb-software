
//---------------------------------------------------------------------------------
/** @file RichDetailedTrSegMakerFromRecoTracks.h
 *
 *  Header file for tool : RichDetailedTrSegMakerFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichDetailedTrSegMakerFromRecoTracks.h,v 1.12 2006-11-22 19:01:41 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTRSEGMAKERFROMRECOTRACKS_H
#define RICHTOOLS_RICHTRSEGMAKERFROMRECOTRACKS_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event model
#include "Event/Track.h"
#include "Event/State.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichKernel/IRichRadiatorTool.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"

// Track Extrapolator
#include "TrackInterfaces/ITrackExtrapolator.h"

// GSL
#include "gsl/gsl_math.h"

//---------------------------------------------------------------------------------
/** @class RichDetailedTrSegMakerFromRecoTracks RichDetailedTrSegMakerFromRecoTracks.h
 *
 *  Tool to create RichTrackSegments from Tracks.
 *
 *  Uses the tracking extrapolation tools to access the state information at the
 *  entrance and exit points to the radiators, which is then used to create the
 *  RichTrackSegments using a detailed approach.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2002
 *
 *  @todo Remove temporary hack to fix up lack of hole for beampipe in RICH1 gas
 *  @todo Look into optimising a little how the radiator intersections are handled. 
 *        Can probably be a little faster.
 *  @todo Consider moving 'radiator fixups' into radiator intersection tool
 */
//---------------------------------------------------------------------------------

class RichDetailedTrSegMakerFromRecoTracks : public RichToolBase,
                                             virtual public IRichTrSegMaker
{

public: // Methods for Gaudi Framework

  /// Standard Constructor
  RichDetailedTrSegMakerFromRecoTracks( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Standard Destructor
  virtual ~RichDetailedTrSegMakerFromRecoTracks( );

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // Create RichTrackSegments for a given tracking object
  int constructSegments( const ContainedObject* track,
                         std::vector<LHCb::RichTrackSegment*>& segments ) const;

private: // methods

  /** Find all intersections with the given radiator volume(s)
   *  @return The number of radiator intersections
   */
  unsigned int 
  getRadIntersections( const Gaudi::XYZPoint&   point,     ///< The start point
                       const Gaudi::XYZVector&  direction, ///< The direction from the start point
                       const DeRichRadiator * rad,         ///< The radiator
                       RichRadIntersection::Vector & intersections ///< The intersections with the given radiator
                       ) const;

  /** Get just the first intersection with the radiator volume
   *  @return boolean indicating if an intersection was found or not
   */
  bool getNextInterPoint( const Gaudi::XYZPoint&   point,     ///< The start point
                          const Gaudi::XYZVector&  direction, ///< The direction from the start point
                          const DeRichRadiator * rad,         ///< The radiator
                          Gaudi::XYZPoint& interP             ///< The first intersection point
                          ) const;

  /** Correct the entrance point for the Rich1Gas radiators due to the fact the aerogel
   *  is contained inside this medium. This means the start of the visable Rich1Gas
   *  segment is the aerogel exit point, and not the Rich1Gas entrance point.
   *
   *  @param state        State information to correct
   *  @param refState     Reference starting state.
   */
  void fixRich1GasEntryPoint( LHCb::State *& state,
                              const LHCb::State * refState = 0 ) const;

  /** Correct the exit state to the point where the track traverses the spherical mirror
   *
   *  @param radiator     Pointer to the apropriate radiator detector element
   *  @param state        State information to correct
   *  @param refState     Reference starting state.
   */
  void correctRadExitMirror( const DeRichRadiator* radiator,
                             LHCb::State *& state,
                             const LHCb::State * refState = 0  ) const;

  /** Extrapolate a state to a new z position
   *
   * @param stateToMove  The state to extrapolate
   * @param z            The z position to extrapolate the state to
   * @param refState     Reference starting state.
   *
   * @return The status of the extrapolation
   * @retval StatusCode::SUCCESS State was successfully extrapolated to the new z position
   * @retval StatusCode::FAILURE State could not be extrapolated to the z position.
   *         State remains unaltered.
   */
  StatusCode moveState( LHCb::State *& stateToMove,
                        const double z,
                        const LHCb::State * refState = 0 ) const;

  /// Access primary track extrapolator tool
  inline ITrackExtrapolator * primaryExtrapolator() const
  {
    return m_trExt1;
  }

  /// Access on-demand backup track extrapolator tool
  inline ITrackExtrapolator * backupExtrapolator() const
  {
    if ( !m_trExt2 ) { m_trExt2 = tool<ITrackExtrapolator>( m_trExt2Name ); }
    return m_trExt2;
  }

  /// Creates the middle point information
  StatusCode createMiddleInfo( const Rich::RadiatorType rad,
                               LHCb::State *& fState,
                               const LHCb::State * fStateRef,
                               LHCb::State *& lState,
                               const LHCb::State * lStateRef,
                               Gaudi::XYZPoint & midPoint,
                               Gaudi::XYZVector & midMomentum,
                               LHCb::RichTrackSegment::StateErrors & errors ) const;

private: // data

  /// Ray tracing tool
  const IRichRayTracing* m_rayTracing;

  /// Pointer to RichParticleProperties interface
  const IRichParticleProperties * m_richPartProp;

  /// Pointer to the radiator intersections tool
  const IRichRadiatorTool * m_radTool;

  /// Rich1 and Rich2 detector elements
  DeRich* m_rich[Rich::NRiches];

  /// typedef of array of DeRichRadiators
  typedef boost::array<const DeRichRadiator *, Rich::NRadiatorTypes> Radiators;
  /// Array of radiators
  Radiators m_radiators;

  /// Allowable tolerance on state z positions
  std::vector<double> m_zTolerance;

  /// Nominal z positions of states at RICHes
  std::vector<double> m_nomZstates;

  // shifts for mirror correction
  std::vector<double> m_mirrShift;

  // sanity checks on state information
  std::vector<double> m_minStateDiff;

  // Track extrapolators
  ITrackExtrapolator * m_trExt1; ///< Primary track extrapolation tool
  mutable ITrackExtrapolator * m_trExt2; ///< Secondary (backup if primary fails) track extrapolation tool
  std::string m_trExt1Name; ///< Primary track extrapolation tool name
  std::string m_trExt2Name; ///< Secondary track extrapolation tool name

  /// Flags to turn on/off individual radiators
  std::vector<bool> m_usedRads;

  /// Flag to indicate if extrapolation should always be done from the reference states
  bool m_extrapFromRef;

  /// Minimum state movement in z to bother with
  double m_minZmove;

  // temporary hacks

  /// Min radius at entry for each radiator (temp hack)
  std::vector<double> m_minEntryRad;

  /// Min radius at exit for each radiator (temp hack)
  std::vector<double> m_minExitRad;

  /// Job option to define type of track segments to create
  std::string m_trSegTypeJO;

  /// Type of track segments to create
  LHCb::RichTrackSegment::SegmentType m_trSegType;

};

#endif // RICHTOOLS_RICHTRSEGMAKERFROMRECOTRACKS_H
