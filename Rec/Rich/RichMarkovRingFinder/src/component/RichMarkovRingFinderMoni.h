
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.h
 *
 *  Header file for algorithm class : RichMarkovRingFinderMoni
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderMoni.h,v 1.18 2006-08-13 17:11:15 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichMarkovRingFinderMoni_H
#define RICHRECMONITOR_RichMarkovRingFinderMoni_H 1

#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

//---------------------------------------------------------------------------
/** @class RichMarkovRingFinderMoni RichMarkovRingFinderMoni.h
 *
 *  Runs various calibration tasks for the markov ring finder
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichMarkovRingFinderMoni : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichMarkovRingFinderMoni( const std::string& name,
                            ISvcLocator* pSvcLocator );

  virtual ~RichMarkovRingFinderMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Pointer RichDetector ray tracing tool
  const IRichRayTracing * m_rayTrace;

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_idTool;

  /// Track selector
  const Rich::IRichTrackSelector * m_trSelector;

};

#endif // RICHRECMONITOR_RichMarkovRingFinderMoni_H
