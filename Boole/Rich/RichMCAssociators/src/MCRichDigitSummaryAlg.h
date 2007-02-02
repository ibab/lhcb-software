
//-----------------------------------------------------------------------------
/** @file MCRichDigitSummaryAlg.h
 *
 *  Header file for algorithm : MCRichDigitSummaryAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitSummaryAlg.h,v 1.5 2007-02-02 10:13:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   04/10/2005
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCASSOCIATORS_MCRichDigitSummaryAlg_H
#define RICHMCASSOCIATORS_MCRichDigitSummaryAlg_H 1

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichDigitSummary.h"
#include "Event/MCParticle.h"

// base class
#include "RichKernel/RichAlgBase.h"

// interfaces
#include "RichKernel/IRichMCTruthTool.h"

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
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @class MCRichDigitSummaryAlg MCRichDigitSummaryAlg.h
     *
     *  Algorithm to fill the MCRichDigitSummary objects.
     *  Used to provide direct navigation from RichSmartIDs to MCParticles on the DST,
     *  and also to provide some history information.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2005-10-04
     */
    //-----------------------------------------------------------------------------

    class MCRichDigitSummaryAlg : public RichAlgBase
    {

    public:

      /// Standard constructor
      MCRichDigitSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~MCRichDigitSummaryAlg( ); ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution

    private: // data members

      /// Flag to turn off storing of spillover summaries
      bool m_storeSpill;

      /// Pointer to RichMCTruth tool
      const Rich::MC::IMCTruthTool * m_truth;

    };

  }
}

#endif // RICHMCASSOCIATORS_MCRichDigitSummaryAlg_H
