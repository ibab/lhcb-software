
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.h
 *
 *  Header file for RICH reconstruction tool : RichPhotonCreatorWithMCSignal
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithMCSignal.h,v 1.2 2005-05-28 16:45:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
#define RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------------------------
/** @class RichPhotonCreatorWithMCSignal RichPhotonCreatorWithMCSignal.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons. MC cheating is performed
 *  by for true Cherenkov photon candidates referring to the associated
 *  MCRichOpticalPhoton to get the true Cherenkov angles etc. For fake photons
 *  the reconstructed quantities are used.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

class RichPhotonCreatorWithMCSignal : public RichPhotonCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPhotonCreatorWithMCSignal( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorWithMCSignal(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

private: // private methods

  /// Form a Photon candidate from a Segment and a pixel.
  virtual RichRecPhoton * buildPhoton( RichRecSegment * segment,
                                       RichRecPixel * pixel,
                                       const RichRecPhotonKey key ) const;
  
private: // private data

  // Pointers to tool instances
  IRichRecMCTruthTool * m_mcRecTool;  ///< Rich Reconstruction MC Truth tool
  IRichPhotonCreator * m_mcPhotCr;    ///< Delegated photon creator for MC photons
  IRichPhotonCreator * m_recoPhotCr;  ///< Delegated photon creator for reco photons

};

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
