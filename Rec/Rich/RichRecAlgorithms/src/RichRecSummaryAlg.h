
//---------------------------------------------------------------------------
/** @file RichRecSummaryAlg.h
 *
 *  Header file for algorithm class : RichRecSummaryAlg
 *
 *  CVS Log :-
 *  $Id: RichRecSummaryAlg.h,v 1.4 2006-12-19 09:46:30 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/ProcStatus.h"
#include "Event/RichSummaryTrack.h"

// tool interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"

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

  /// Pointer to RichExpectedTrackSignal tool
  const IRichExpectedTrackSignal * m_tkSignal; 

  /// Track selector
  const Rich::IRichTrackSelector * m_trSelector;

  /// Location to store the summary tracks
  std::string m_summaryLoc;

  /// Number of sigmas to select photons, for each radiator
  std::vector<double> m_nSigma;

};

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
