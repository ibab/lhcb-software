
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.h
 *
 *  Header file for algorithm class : RichCherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.h,v 1.3 2006-08-13 17:13:15 jonrob Exp $
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
#include "RichKernel/IRichParticleProperties.h"
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
  
  const IRichParticleProperties * m_richPartProp; ///< Rich Particle properties
  const IRichRecMCTruthTool* m_richRecMCTruth;   ///< Pointer to RichRecMCTruthTool interface
  const IRichCherenkovAngle * m_ckAngle;         ///< Rich Cherenkov angle calculator tool

  double m_minBeta;        ///< minimum beta value for 'saturated' tracks

  /// Track selector
  const Rich::IRichTrackSelector * m_trSelector;

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
