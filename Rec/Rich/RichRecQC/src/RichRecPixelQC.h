
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.h
 *
 *  Header file for algorithm class : RichRecPixelQC
 *
 *  CVS Log :-
 *  $Id: RichRecPixelQC.h,v 1.2 2006-03-22 10:02:29 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichRecPixelQC_H
#define RICHRECMONITOR_RichRecPixelQC_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

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
/** @class RichRecPixelQC RichRecPixelQC.h
 *
 *  Monitor class for the pixel reconstruction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichRecPixelQC : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichRecPixelQC( const std::string& name,
                  ISvcLocator* pSvcLocator );

  virtual ~RichRecPixelQC( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Get various MC history flags for given RichSmartID
  void getHistories( const RichSmartID id, 
                     bool & isBkg,
                     bool & isHPDQCK,
                     bool & isSignal,
                     bool & isAerogelCK,
                     bool & isC4F10CK,
                     bool & isCF4CK ) const;

  /// Print summary for given RICH
  void printRICH( const Rich::DetectorType rich ) const;

private: // data

  /// Pointer to RichRecMCTruthTool interface
  const IRichRecMCTruthTool* m_richRecMCTruth;

  /// Pointer to RichMCTruthTool
  const IRichMCTruthTool * m_truth;

  /// Raw Buffer Decoding tool
  const IRichRawBufferToSmartIDsTool * m_decoder;

  /// Pointer to RICH system detector element
  const DeRichSystem * m_richSys;

  // tallies

  std::vector<unsigned int> m_pixelsRaw;
  std::vector<unsigned int> m_bkgsRaw;
  std::vector<unsigned int> m_npdqcksRaw;
  std::vector<unsigned int> m_signalRaw;
  std::vector<unsigned int> m_radHitsRaw;

  std::vector<unsigned int> m_pixels;
  std::vector<unsigned int> m_bkgs;
  std::vector<unsigned int> m_npdqcks;
  std::vector<unsigned int> m_signal;
  std::vector<unsigned int> m_radHits;

  unsigned int m_nEvts;

};

#endif // RICHRECMONITOR_RichRecPixelQC_H
