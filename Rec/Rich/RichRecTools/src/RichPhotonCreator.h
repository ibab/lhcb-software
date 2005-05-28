
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.h
 *
 *  Header file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.h,v 1.22 2005-05-28 13:10:53 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONCREATOR_H
#define RICHRECTOOLS_RICHPHOTONCREATOR_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPhotonCreatorBase.h"

// Interfaces
#include "RichKernel/IRichPhotonReconstruction.h"

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
  virtual RichRecPhoton * buildPhoton( RichRecSegment * segment,
                                       RichRecPixel * pixel,
                                       const RichRecPhotonKey key ) const;

private: // private data

  // Pointers to tool instances
  IRichPhotonReconstruction * m_photonReco; ///< Pointer to photon reconstruction tool

};

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
