
//---------------------------------------------------------------------------
/** @file RichTrackPIDAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::TrackPIDAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichTrackPIDAlg_H
#define RICHRECALGORITHMS_RichTrackPIDAlg_H 1

// STL
#include <string>

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichPID.h"

// Event
#include "Event/ProcStatus.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class TrackPIDAlg RichTrackPIDAlg.h
     *
     *  (test) algorithm to run RICH PID tool on various tracks
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class TrackPIDAlg : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      TrackPIDAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~TrackPIDAlg();   ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution

    private:

      /// Pointer to RICH PID tool
      const IRichPID * m_pidTool;

      /// Track Selector
      const Rich::Rec::ITrackSelector * m_trSelector;

      /// Nickname of RichPID tool to use
      std::string m_pidToolName;

      /// Input location for tracks
      std::string m_trTracksLocation;

      /// Number of tracks to PID at once
      unsigned int m_nAtOnce;

    };

  }
}

#endif // RICHRECALGORITHMS_RichTrackPIDAlg_H
