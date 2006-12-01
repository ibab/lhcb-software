
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.h
 *
 *  Header file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.h,v 1.26 2006-12-01 17:05:09 cattanem Exp $
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
#include "RichRecBase/IRichPhotonReconstruction.h"

//-----------------------------------------------------------------------------
/** @class RichPhotonCreator RichPhotonCreator.h
 *
 *  Tool which performs the association between RichRecTracks and
 *  RichRecPixels to form RichRecPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichPhotonCreator : public RichPhotonCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPhotonCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~RichPhotonCreator(){}

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

  /// Pointer to photon reconstruction tool
  const IRichPhotonReconstruction * m_photonReco;

  /// Photon reconstruction tool runtime name
  std::string m_photonRecoName;

};

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
