
//---------------------------------------------------------------------------
/** @file RichRecSummaryAlg.h
 *
 *  Header file for algorithm class : RichRecSummaryAlg
 *
 *  CVS Log :-
 *  $Id: RichRecSummaryAlg.h,v 1.1 2006-06-14 21:55:26 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// utilities
#include "RichRecBase/RichTrackSelector.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/ProcStatus.h"
#include "Event/RichSummaryTrack.h"

// tool interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

//---------------------------------------------------------------------------
/** @class RichRecSummaryAlg RichRecSummaryAlg.h
 *
 *  Algorithm to fill the reconstruction summary data objects for the RICH
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   07/06/2006
 */
//---------------------------------------------------------------------------

class RichRecSummaryAlg : public RichRecAlgBase
{

public:

  /// Standard constructor
  RichRecSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRecSummaryAlg();   ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private:   // Private data members

  /// Rich Cherenkov angle calculator tool
  const IRichCherenkovAngle * m_ckAngle;

  /// Cherenkov angle resolution tool
  const IRichCherenkovResolution * m_ckAngleRes;

  /// Track selector
  RichTrackSelector m_trSelector;

  /// Location to store the summary tracks
  std::string m_summaryLoc;

  /// Number of sigmas to select photons, for each radiator
  std::vector<double> m_nSigma;

};

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
