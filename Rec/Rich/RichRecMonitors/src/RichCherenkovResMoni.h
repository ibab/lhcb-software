
//---------------------------------------------------------------------------
/** @file RichCherenkovResMoni.h
 *
 *  Header file for algorithm class : RichCherenkovResMoni
 *
 *  CVS Log :-
 *  $Id: RichCherenkovResMoni.h,v 1.5 2006-08-13 17:13:15 jonrob Exp $
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

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

//---------------------------------------------------------------------------
/** @class RichCherenkovResMoni RichCherenkovResMoni.h
 *
 *  Monitors the resolution of the tracking information, in terms important
 *  for the RICH reconstruction.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichCherenkovResMoni : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichCherenkovResMoni( const std::string& name,
                        ISvcLocator* pSvcLocator );

  virtual ~RichCherenkovResMoni( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth;   ///< Pointer to RichRecMCTruthTool interface
  const IRichCherenkovAngle * m_ckAngle;         ///< Rich Cherenkov angle calculator tool
  const IRichCherenkovResolution * m_ckAngleRes; ///< Cherenkov angle resolution tool
  const Rich::IRichTrackSelector * m_trSelector; ///< Track selector

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
