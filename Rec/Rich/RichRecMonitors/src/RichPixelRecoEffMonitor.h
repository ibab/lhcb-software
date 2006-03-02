
//-----------------------------------------------------------------------------
/** @file RichPixelRecoEffMonitor.h
 *
 *  Header file for algorithm class : RichPixelRecoEffMonitor
 *
 *  CVS Log :-
 *  $Id: RichPixelRecoEffMonitor.h,v 1.1 2006-03-02 15:26:29 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPixelRecoEffMonitor_H
#define RICHRECMONITOR_RichPixelRecoEffMonitor_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
//#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/MCRichDigitSummary.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

//-----------------------------------------------------------------------------
/** @class RichPixelRecoEffMonitor RichPixelRecoEffMonitor.h
 *
 *  Monitor class for the reconstruction efficiency for RichRecPixels
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichPixelRecoEffMonitor : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichPixelRecoEffMonitor( const std::string& name,
                           ISvcLocator* pSvcLocator );

  virtual ~RichPixelRecoEffMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Pointer to RichRecMCTruthTool interface
  const IRichRecMCTruthTool* m_richRecMCTruth; 

  /// Pointer to RichMCTruthTool
  const IRichMCTruthTool * m_truth;

  /// Raw Buffer Decoding tool
  const IRichRawBufferToSmartIDsTool * m_decoder;

  /// Pointer to RICH system detector element
  const DeRichSystem * m_richSys;

};

#endif // RICHRECMONITOR_RichPixelRecoEffMonitor_H
