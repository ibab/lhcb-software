
//-----------------------------------------------------------------------------
/** @file RichPhotonCreatorCheatedTrackDir.h
 *
 *  Header file for tool : RichPhotonCreatorCheatedTrackDir
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorCheatedTrackDir.h,v 1.2 2006-12-01 16:18:23 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONCREATOR_H
#define RICHRECTOOLS_RICHPHOTONCREATOR_H 1

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------
/** @class RichPhotonCreatorCheatedTrackDir RichPhotonCreatorCheatedTrackDir.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons. For signal photos, cheats by using the
 *  MC track direction at emission point.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichPhotonCreatorCheatedTrackDir : public RichPhotonCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPhotonCreatorCheatedTrackDir( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreatorCheatedTrackDir(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

protected: // methods

  /// Form a Photon candidate from a Segment and a pixel.
  virtual LHCb::RichRecPhoton * buildPhoton( LHCb::RichRecSegment * segment,
                                             LHCb::RichRecPixel * pixel,
                                             const RichRecPhotonKey key ) const;

private: // private data

  /// Rich Reconstruction MC Truth tool
  const IRichRecMCTruthTool * m_mcRecTool; 

  /// Delegated photon creator for reco photons
  const IRichPhotonCreator * m_recoPhotCr; 

};

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
