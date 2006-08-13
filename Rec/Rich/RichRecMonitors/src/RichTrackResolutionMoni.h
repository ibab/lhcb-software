
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.h
 *
 *  Header file for algorithm class : RichTrackResolutionMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackResolutionMoni.h,v 1.5 2006-08-13 17:13:15 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"
#include "Event/MCRichSegment.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTrackInfoTool.h"

// Kernel
#include "RichKernel/RichGeomFunctions.h"

//---------------------------------------------------------------------------
/** @class RichTrackResolutionMoni RichTrackResolutionMoni.h
 *
 *  Monitors the resolution of the tracking information, in terms important
 *  for the RICH reconstruction.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichTrackResolutionMoni : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichTrackResolutionMoni( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichTrackResolutionMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  const IRichMCTrackInfoTool * m_mcTkInfo; ///< MC track information
  const Rich::IRichTrackSelector * m_trSelector; ///< Track selector

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
