
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrackCreator.h
 *
 *  Header file for RICH Global PID tool : Rich::Rec::GlobalPID::TrackCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-03-01
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDTrackCreator_H
#define RICHGLOBALPID_RichGlobalPIDTrackCreator_H 1

// Base class
#include "RichGlobalPIDToolBase.h"
#include "IRichGlobalPIDTrackCreator.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"

// Event
#include "Event/RichRecTrack.h"
#include "Event/RichGlobalPIDTrack.h"

// GSL
#include "gsl/gsl_sf_erf.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      /** @class TrackCreator RichGlobalPIDTrackCreator.h
       *
       *  Tool to create RichGlobalPIDTrack track objects for the RICH
       *  Global PID algorithm
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2008-03-01
       *
       *  @todo Remove temporary hack "correcting" final DLL values once
       *        likelihood algorithm is rewritten
       */
      class TrackCreator : public Rich::Rec::GlobalPID::ToolBase,
                           virtual public ITrackCreator
      {

      public:

        /// Standard constructor
        TrackCreator( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

        /// Destructor
        virtual ~TrackCreator( );

        // Initialize method
        virtual StatusCode initialize();

      public:

        // Create a GPID track from a RichRecTrack
        virtual LHCb::RichGlobalPIDTrack * createTrack( LHCb::RichRecTrack * track ) const;

        // Finalise the given RichGlobalPIDtrack
        virtual void finaliseTrack( LHCb::RichGlobalPIDTrack * track ) const;

      private:

        /// Pointer to RichExpectedTrackSignal
        const IExpectedTrackSignal * m_tkSignal;

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDTrackCreator_H
