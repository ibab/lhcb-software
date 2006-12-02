
//---------------------------------------------------------------------------
/** @file RichGhostTrackMoni.h
 *
 *  Header file for algorithm class : RichGhostTrackMoni
 *
 *  CVS Log :-
 *  $Id: RichGhostTrackMoni.h,v 1.2 2006-12-02 15:26:24 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event
#include "Event/RichRecTrack.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

// GSL
#include "gsl/gsl_math.h"

/// General RICH namespace
namespace Rich
{

  //---------------------------------------------------------------------------
  /** @class RichGhostTrackMoni RichGhostTrackMoni.h
   *
   *  Monitors the properties of ghost tracks
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //---------------------------------------------------------------------------

  class RichGhostTrackMoni : public RichRecTupleAlgBase
  {

  public:

    /// Standard constructor
    RichGhostTrackMoni( const std::string& name,
                        ISvcLocator* pSvcLocator );

    virtual ~RichGhostTrackMoni( ); ///< Destructor

    virtual StatusCode initialize();    // Algorithm initialization
    virtual StatusCode execute   ();    // Algorithm execution
    virtual StatusCode finalize  ();    // Algorithm finalization

  private: // data

    /// Track selector
    const Rich::IRichTrackSelector * m_trSelector;

    const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface

  };

}

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
