
//-----------------------------------------------------------------------------
/** @file RichSegmentCreator.h
 *
 *  Header file for tool : Rich::Rec::SegmentCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_RICHRECSEGMENTTOOL_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// Rich Kernel
#include "RichKernel/RichStatDivFunctor.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichKernel/IRichDetParameters.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class SegmentCreator RichSegmentCreator.h
     *
     *  Tool for the creation and manipulation of RichRecSegment objects
     *
     *  @attention
     *  This is a utility tool and should only used by the RichTrackCreator tools.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class SegmentCreator : public Rich::Rec::ToolBase,
                           virtual public ISegmentCreator,
                           virtual public IIncidentListener
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      SegmentCreator( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

      /// Destructor
      virtual ~SegmentCreator() {};

      /// Initialize method
      StatusCode initialize();

      /// Finalize method
      StatusCode finalize();

      /// Implement the handle method for the Incident service.
      /// This is used by the tool at the beginning of events to initialise
      /// a new container for the RichRecTracks
      void handle( const Incident& incident );

    public: // methods (and doxygen comments) inherited from public interface

      // Save a new RichRecSegment in the container
      void saveSegment( LHCb::RichRecSegment * segment ) const;

      // Create a new RichRecSegment
      LHCb:: RichRecSegment * newSegment( LHCb::RichTrackSegment* segment,
                                          LHCb::RichRecTrack* pTrk ) const;

      // Return a pointer to RichRecSegments
      LHCb::RichRecSegments * richSegments() const;

    private: // methods

      /// Initialise for each event
      void InitEvent();

      /// Finalise for each event
      void FinishEvent();

    private:  // Private data

      /// Pointer to RichExpectedTrackSignal interface
      const IExpectedTrackSignal * m_signal;

      /// Pointer to detector parameters tool
      const IDetParameters * m_detParams;

      /// Pointer to RichRecTracks
      mutable LHCb::RichRecSegments * m_segments;

      /// Location of RichRecSegments in TES
      std::string m_richRecSegmentLocation;

      /// Number of energy bins for each radiator
      std::vector<unsigned int> m_binsEn;

      // debug segment counting
      mutable std::vector<unsigned long long> m_segCount;
      mutable std::vector<unsigned long long> m_segCountLast;

      /// Number of events processed tally
      unsigned long long m_Nevts;

      /// Flag to indicate if the tool has been used in a given event
      mutable bool m_hasBeenCalled;

    };

    inline void SegmentCreator::InitEvent()
    {
      m_segments = 0;
      if (msgLevel(MSG::DEBUG))
      {
        m_segCountLast = m_segCount;
        m_hasBeenCalled = false;
      }
    }

    inline void SegmentCreator::FinishEvent()
    {
      if (msgLevel(MSG::DEBUG))
      {
        if ( m_hasBeenCalled ) ++m_Nevts;
      }
    }

  }
}

#endif // RICHRECTOOLS_RICHRECSEGMENTTOOL_H
