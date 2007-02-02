
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.h
 *
 *  Header file for tool : Rich::Rec::PhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.h,v 1.27 2007-02-02 10:10:41 jonrob Exp $
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
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
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
