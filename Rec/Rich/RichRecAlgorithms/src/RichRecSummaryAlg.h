
//---------------------------------------------------------------------------
/** @file RichRecSummaryAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::SummaryAlg
 *
 *  CVS Log :-
 *  $Id: RichRecSummaryAlg.h,v 1.5 2007-02-02 10:05:51 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/ProcStatus.h"
#include "Event/RichSummaryTrack.h"

// tool interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class SummaryAlg RichRecSummaryAlg.h
     *
     *  Algorithm to fill the reconstruction summary data objects for the RICH
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   07/06/2006
     */
    //---------------------------------------------------------------------------

    class SummaryAlg : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      SummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~SummaryAlg();   ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution
      virtual StatusCode finalize  ();    // Algorithm finalization

    private:   // Private data members

      /// Rich Cherenkov angle calculator tool
      const ICherenkovAngle * m_ckAngle;

      /// Cherenkov angle resolution tool
      const ICherenkovResolution * m_ckAngleRes;

      /// Pointer to RichExpectedTrackSignal tool
      const IExpectedTrackSignal * m_tkSignal;

      /// Track selector
      const ITrackSelector * m_trSelector;

      /// Location to store the summary tracks
      std::string m_summaryLoc;

      /// Number of sigmas to select photons, for each radiator
      std::vector<double> m_nSigma;

    };

  }
}

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
