
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorFromMCRichOpticalPhotons.h
 *
 *  Header file for RICH reconstruction tool : RichPhotonCreatorFromMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorFromMCRichOpticalPhotons.h,v 1.6 2005-06-23 15:14:12 jonrob Exp $
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
/** @class RichPhotonCreatorFromMCRichOpticalPhotons RichPhotonCreatorFromMCRichOpticalPhotons.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons. MC cheating is performed
 *  by only creating true cherenkov photons using MCRichOpticalPhoton
 *  information and also by using the MC geometrical information instead of
 *  the reconstructed values (angles etc.).
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

class RichPhotonCreatorFromMCRichOpticalPhotons : public RichPhotonCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPhotonCreatorFromMCRichOpticalPhotons( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorFromMCRichOpticalPhotons(){}

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
  const IRichRecMCTruthTool * m_mcRecTool;  ///< Rich Reconstruction MC Truth tool

};

#endif // RICHRECMCTOOLS_RICHPHOTONCREATORFROMMCRICHOPTICALPHOTONS_H
