
//---------------------------------------------------------------------------
/** @file RichPhotonSignalMonitor.h
 *
 *  Header file for algorithm class : RichPhotonSignalMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonSignalMonitor.h,v 1.4 2006-12-01 16:34:07 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGeomEff.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

//---------------------------------------------------------------------------
/** @class RichPhotonSignalMonitor RichPhotonSignalMonitor.h
 *
 *  Monitors the photon signals.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichPhotonSignalMonitor : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichPhotonSignalMonitor( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichPhotonSignalMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth;   ///< Pointer to RichRecMCTruthTool interface
  const IRichExpectedTrackSignal * m_tkSignal;   ///< Pointer to RichExpectedTrackSignal tool
  const IRichGeomEff * m_geomEffic; ///< Pointer to ray-tracing RichGeomEff tool
  const IRichRefractiveIndex * m_refIndex; ///< Refractive index tool
  const Rich::IRichTrackSelector * m_trSelector;  ///< Track selector

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
