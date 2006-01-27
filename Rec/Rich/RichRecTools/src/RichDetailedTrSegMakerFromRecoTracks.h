
//---------------------------------------------------------------------------------
/** @file RichDetailedTrSegMakerFromRecoTracks.h
 *
 *  Header file for tool : RichDetailedTrSegMakerFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichDetailedTrSegMakerFromRecoTracks.h,v 1.1 2006-01-27 09:14:17 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTRSEGMAKERFROMRECOTRACKS_H
#define RICHTOOLS_RICHTRSEGMAKERFROMRECOTRACKS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event model
#include "Event/Track.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichParticleProperties.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"

// Track Extrapolator
#include "TrackInterfaces/ITrackExtrapolator.h"

// constants
#include "Kernel/PhysicalConstants.h"

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
 *  @todo Reduce the verbose level printout, once tool is stabilised
 *  @todo Update UMS dependencies to be more 'fine grained'
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

  /** Correct the entrance point for the C4F10 radiators due to the fact the aerogel
   *  is contained inside this medium. This means the start of the visable C4F10
   *  segment is the aerogel exit point, and not the C4F10 entrance point.
   *
   *  @param state        State information to correct
   *  @param refState     Reference starting state.
   */
  void fixC4F10EntryPoint( LHCb::State *& state,
                           const LHCb::State * refState = 0  ) const;

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

  /// UMS Update method for Rich1 nominal radius of curvature
  StatusCode updateR1nomRad();

  /// UMS Update method for Rich2 nominal radius of curvature
  StatusCode updateR2nomRad();

private: // data

  /// Ray tracing tool
  const IRichRayTracing* m_rayTracing;

  /// Pointer to RichParticleProperties interface
  const IRichParticleProperties * m_richPartProp;

  /// Rich1 and Rich2 detector elements
  DeRich* m_rich[Rich::NRiches];

  /// typedef of array of DeRichRadiators
  typedef boost::array<const DeRichRadiator*, Rich::NRadiatorTypes> Radiators;
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
  ITrackExtrapolator * m_trExt2; ///< Secondary (backup if primary fails) track extrapolation tool
  std::string m_Ext1; ///< Primary track extrapolation tool name
  std::string m_Ext2; ///< Secondary track extrapolation tool name

  /// Flags to turn on/off individual radiators
  std::vector<bool> m_usedRads;

  /// Flag to indicate if extrapolation should always be done from the reference states
  bool m_extrapFromRef;

};

#endif // RICHTOOLS_RICHTRSEGMAKERFROMRECOTRACKS_H
