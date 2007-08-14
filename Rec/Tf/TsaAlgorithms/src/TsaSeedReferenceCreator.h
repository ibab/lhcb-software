// $Id: TsaSeedReferenceCreator.h,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
#ifndef _TsaSeedReferenceCreator_H
#define _TsaSeedReferenceCreator_H

#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackManipulator.h"
#include "GaudiKernel/IIncidentListener.h"

#include <string>

namespace LHCb{
  class Track;
  class Measurement;
}
class ITrajPoca;

#include "TsaKernel/SeedTrack.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class SeedReferenceCreator TsaSeedReferenceCreator.h
     *
     * Implementation of ITrackManipulator for Tsa Tracks
     *
     *  @author M.Needham
     *  @date   30/12/2005
     */

    class SeedReferenceCreator: public GaudiTool,
                                virtual public ITrackManipulator,
                                virtual public IIncidentListener
    {

    public:

      /** constructer */
      SeedReferenceCreator(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

      /** destructer */
      virtual ~SeedReferenceCreator();

      /** intialize */
      StatusCode initialize();

      /** add reference info to the track */
      StatusCode execute(LHCb::Track& aTrack) const;

      /** Implement the handle method for the Incident service.
       *  This is used to inform the tool of software incidents.
       *
       *  @param incident The incident identifier
       */
      void handle( const Incident& incident );

    private:

      void initEvent() const;

      void addReference(LHCb::Measurement* meas, const SeedTrack* seedTrack,
                        const double qOverP, const int ambiguity) const;

      ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

      // Job options
      bool m_setLRAmbiguities;            ///< Flag to set the OT LR ambiguities


      mutable SeedTracks* m_seeds;
      mutable bool m_configured;

    };

  }
}

#endif
