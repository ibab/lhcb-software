
//---------------------------------------------------------------------------------
/** @file RichTrSegMakerFromTrStoredTracks.h
 *
 *  Header file for tool : RichTrSegMakerFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichTrSegMakerFromTrStoredTracks.h,v 1.8 2005-02-02 10:12:48 jonrob Exp $
 *
 *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   14/01/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHTRSEGMAKERFROMTRSTOREDTRACKS_H
#define RICHTOOLS_RICHTRSEGMAKERFROMTRSTOREDTRACKS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichTrSegMaker.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/IRichRayTracing.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"

// Tr Extrapolator
#include "TrKernel/ITrExtrapolator.h"

// CLHEP
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"

//---------------------------------------------------------------------------------
/** @class RichTrSegMakerFromTrStoredTracks RichTrSegMakerFromTrStoredTracks.h
 *
 *  Tool to create RichTrackSegments from TrStoredTracks. Use the tracking extrapolation tools
 *  to access the state information at the entrance and exit points to the radiators, which is
 *  then used to create the RichTrackSegments.
 *
 *  @author Chris Jones         Christopher.Rob.Jones@cern.ch
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   14/01/2002
 */
//---------------------------------------------------------------------------------

class RichTrSegMakerFromTrStoredTracks : public RichToolBase,
                                         virtual public IRichTrSegMaker {

public: // Methods for Gaudi Framework

  /// Standard Constructor
  RichTrSegMakerFromTrStoredTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

  /// Standard Destructor
  virtual ~RichTrSegMakerFromTrStoredTracks( );

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // Create RichTrackSegments for a given tracking object
  int constructSegments( const ContainedObject* track, 
                         std::vector<RichTrackSegment>& segments ) const;

private: // methods

  /** Correct the entrance point for the C4F10 radiators due to the fact the aerogel
   *  is contained inside this medium. This means the start of the visable C4F10
   *  segment is the aerogel exit point, and not the C4F10 entrance point.
   *
   *  @param state State information to correct
   */
  void fixC4F10EntryPoint( TrStateP * state ) const;

  /** Correct the exit state to the point the track traverses the spherical mirror
   *
   *  @param radiator  Pointer to the apropriate radiator detector element
   *  @param state     State information to correct
   */
  void correctRadExitMirror( DeRichRadiator* radiator, 
                             TrStateP * state ) const;

  /** Extrapolate a state to a new z position
   *
   * @param state  The state to extrapolate
   * @param z      The z position to extrapolate the state to
   *
   * @return The status of the extrapolation
   * @retval StatusCode::SUCCESS State was successfully extrapolated to the new z position
   * @retval StatusCode::FAILURE State could not be extrapolated to the z position. Remains unaltered.
   */
  StatusCode moveState( TrStateP* state,
                        const double z ) const;

private: // data

  /// Rich1 and Rich2
  DeRich* m_rich[Rich::NRiches];

  /// Spherical mirror nominal centre of curvature
  HepPoint3D m_nominalCoC[Rich::NRiches][2];

  /// Spherical mirror radius of curvature
  double m_nomSphMirrorRadius[Rich::NRiches];

  /// typedef of array of DeRichRadiators
  typedef boost::array<DeRichRadiator*, Rich::NRadiatorTypes> Radiators;
  /// Array of radiators
  Radiators m_radiators;

  /// Allowable tolerance on state z positions
  double m_zTolerance[Rich::NRadiatorTypes];

  /// Nominal z positions of states at RICHes
  double m_nomZstates[4];

  // shifts for mirror correction
  double m_mirrShift[Rich::NRiches];

  // sanity checks on state information
  double m_minStateDiff[Rich::NRadiatorTypes];

  /// Ray tracing tool
  IRichRayTracing* m_rayTracing;

  // Track extrapolators
  ITrExtrapolator * m_trExt1; ///< Primary track extrapolation tool
  ITrExtrapolator * m_trExt2; ///< Secondary (backup if primary fails) track extrapolation tool
  std::string m_Ext1; ///< Primary track extrapolation tool name
  std::string m_Ext2; ///< Secondary track extrapolation tool name

  /// Flags to turn on/off individual radiators
  std::vector<bool> m_usedRads;

};


#endif // RICHTOOLS_RICHTRSEGMAKERFROMTRSTOREDTRACKS_H
