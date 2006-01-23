
//---------------------------------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingMCRichOpticalPhotons.h
 *
 *  Header file for RICH reconstruction tool : RichPhotonPredictorUsingMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonPredictorUsingMCRichOpticalPhotons.h,v 1.5 2006-01-23 14:09:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//---------------------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPHOTONPREDICTORUSINGMCRICHOPTICALPHOTOS_H
#define RICHRECMCTOOLS_RICHPHOTONPREDICTORUSINGMCRICHOPTICALPHOTOS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

//---------------------------------------------------------------------------------------------------
/** @class RichPhotonPredictorUsingMCRichOpticalPhotons RichPhotonPredictorUsingMCRichOpticalPhotons.h
 *
 *  Tool which performs the association between RichRecSegments and
 *  RichRecPixels to form RichRecPhotons.  This particular instance uses
 *  MCRichOpticalPhoton objects to select only true Cherenkov photon combinations.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//---------------------------------------------------------------------------------------------------

class RichPhotonPredictorUsingMCRichOpticalPhotons : public RichRecToolBase,
                                                     virtual public IRichPhotonPredictor
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPhotonPredictorUsingMCRichOpticalPhotons( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonPredictorUsingMCRichOpticalPhotons(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // Public interface methods

  // Is it possible to make a photon candidate using this segment and pixel.
  bool photonPossible( LHCb::RichRecSegment * segment,
                       LHCb::RichRecPixel * pixel ) const;

private: // private data

  // Pointers to tool instances
  const IRichRecMCTruthTool * m_mcRecTool;  ///< MC Truth tool

};

#endif // RICHRECMCTOOLS_RICHPHOTONPREDICTORUSINGMCRICHOPTICALPHOTOS_H
