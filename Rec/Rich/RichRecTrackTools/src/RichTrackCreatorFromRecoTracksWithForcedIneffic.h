
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracksWithForcedIneffic.h
 *
 *  Header file for tool : Rich::Rec::TrackCreatorFromRecoTracksWithForcedIneffic
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromRecoTracksWithForcedIneffic.h,v 1.2 2007-12-05 17:39:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTrackCreatorFromRecoTracksWithForcedIneffic_H
#define RICHRECTOOLS_RichTrackCreatorFromRecoTracksWithForcedIneffic_H 1

// base class
#include "RichTrackCreatorFromRecoTracks.h"

// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

namespace Rich
{
  namespace Rec
  {

    //------------------------------------------------------------------------------------------------------
    /** @class TrackCreatorFromRecoTracksWithForcedIneffic RichTrackCreatorFromRecoTracksWithForcedIneffic.h
     *
     *  Inherits from TrackCreatorFromRecoTracks and adds forced tracking inefficiency, by
     *  randomly rejecting a configurable fraction of tracks.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //------------------------------------------------------------------------------------------------------

    class TrackCreatorFromRecoTracksWithForcedIneffic : public TrackCreatorFromRecoTracks
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      TrackCreatorFromRecoTracksWithForcedIneffic( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent );

      /// Destructor
      virtual ~TrackCreatorFromRecoTracksWithForcedIneffic() { }

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Returns a RichRecTrack object pointer for given ContainedObject.
      // In this implementation the ContainedObject must be a reconstruction 'Track'.
      virtual LHCb::RichRecTrack * newTrack ( const ContainedObject * obj ) const;

    private: // data

      /// Fraction of tracks to reject
      double m_rejFrac;

      /// random number generator
      mutable Rndm::Numbers m_rndm;

    };

  }
}

#endif // RICHRECTOOLS_RichTrackCreatorFromRecoTracksWithForcedIneffic_H
