
//---------------------------------------------------------------------------
/** @file RichPhotonGeomMonitor.h
 *
 *  Header file for algorithm class : RichPhotonGeomMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonGeomMonitor.h,v 1.2 2006-08-13 17:13:15 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"
#

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRecGeomTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

//---------------------------------------------------------------------------
/** @class RichPhotonGeomMonitor RichPhotonGeomMonitor.h
 *
 *  Template for monitor algorithms. For cut 'n' paste ...
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

class RichPhotonGeomMonitor : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichPhotonGeomMonitor( const std::string& name,
                         ISvcLocator* pSvcLocator );

  virtual ~RichPhotonGeomMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth;    ///< Pointer to RichRecMCTruthTool interface
  const IRichCherenkovAngle * m_ckAngle;          ///< Rich Cherenkov angle calculator tool
  const IRichRecGeomTool * m_geomTool;            ///< Simple geometrical questions
  const Rich::IRichTrackSelector * m_trSelector;  ///< Track selector

};

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
