
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.h
 *
 *  Header file for tool : Rich::Rec::PhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.h,v 1.1.1.1 2007-11-26 17:25:46 jonrob Exp $
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

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class PhotonCreator RichPhotonCreator.h
     *
     *  Tool which performs the association between RichRecTracks and
     *  RichRecPixels to form RichRecPhotons
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class PhotonCreator : public PhotonCreatorBase
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PhotonCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

      /// Destructor
      virtual ~PhotonCreator(){}

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
      const IPhotonReconstruction * m_photonReco;

      /// Photon reconstruction tool runtime name
      std::string m_photonRecoName;

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONCREATOR_H
