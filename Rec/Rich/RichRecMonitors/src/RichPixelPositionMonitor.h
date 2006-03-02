
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.h
 *
 *  Header file for algorithm class : RichPixelPositionMonitor
 *
 *  CVS Log :-
 *  $Id: RichPixelPositionMonitor.h,v 1.1 2006-03-02 15:26:29 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPixelPositionMonitor_H
#define RICHRECMONITOR_RichPixelPositionMonitor_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
/** @class RichPixelPositionMonitor RichPixelPositionMonitor.h
 *
 *  Monitor class for general position and geometry of RichRecPixels
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichPixelPositionMonitor : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichPixelPositionMonitor( const std::string& name,
                            ISvcLocator* pSvcLocator );

  virtual ~RichPixelPositionMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  const IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface

  // variables for average PD positions
  Rich::Map<LHCb::RichSmartID::KeyType, double> m_xHits;
  Rich::Map<LHCb::RichSmartID::KeyType, double> m_yHits;
  Rich::Map<LHCb::RichSmartID::KeyType, double> m_zHits;
  Rich::Map<LHCb::RichSmartID::KeyType, int> m_hitCount;

};

#endif // RICHRECMONITOR_RichPixelPositionMonitor_H
