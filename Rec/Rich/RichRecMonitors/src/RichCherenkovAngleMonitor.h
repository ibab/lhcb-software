
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.h
 *
 *  Header file for algorithm class : RichCherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.h,v 1.1 2005-11-07 09:37:19 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"
#include "RichRecBase/RichTrackSelector.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

//---------------------------------------------------------------------------
/** @class RichCherenkovAngleMonitor RichCherenkovAngleMonitor.h
 *
 *  Template for monitor algorithms. For cut 'n' paste ...
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichCherenkovAngleMonitor : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichCherenkovAngleMonitor( const std::string& name,
                             ISvcLocator* pSvcLocator );

  virtual ~RichCherenkovAngleMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth;   ///< Pointer to RichRecMCTruthTool interface
  const IRichCherenkovAngle * m_ckAngle;         ///< Rich Cherenkov angle calculator tool

  /// Track selector
  RichTrackSelector m_trSelector;

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
