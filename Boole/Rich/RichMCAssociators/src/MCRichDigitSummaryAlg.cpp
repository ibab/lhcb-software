
//-----------------------------------------------------------------------------
/** @file MCRichDigitSummaryAlg.cpp
 *
 * Implementation file for class : MCRichDigitSummaryAlg
 *
 * CVS Log :-
 * $Id: MCRichDigitSummaryAlg.cpp,v 1.6 2006-12-18 15:44:48 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2004-02-11
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichDigitSummaryAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespace
using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCRichDigitSummaryAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCRichDigitSummaryAlg::MCRichDigitSummaryAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : RichAlgBase  ( name , pSvcLocator ),
    m_storeSpill ( false ),
    m_truth      ( 0 )
{
  // job options
  declareProperty( "StoreSpillover", m_storeSpill );
}
//=============================================================================

//=============================================================================
// Destructor
//=============================================================================
MCRichDigitSummaryAlg::~MCRichDigitSummaryAlg() { }
//=============================================================================

//=============================================================================
// initialisation
//=============================================================================
StatusCode MCRichDigitSummaryAlg::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tool
  acquireTool( "RichMCTruthTool", m_truth, 0, true       );

  if ( !m_storeSpill )
    info() << "Will only store MCParticle references for main event" << endreq;

  return sc;
}
//=============================================================================

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCRichDigitSummaryAlg::execute()
{

  // Locate MCRichDigits
  MCRichDigits * mcDigits = get<MCRichDigits>( MCRichDigitLocation::Default );

  // Make new container of MCRichDigitSummaries
  MCRichDigitSummarys * summaries = new MCRichDigitSummarys();
  put ( summaries, MCRichDigitSummaryLocation::Default );

  // loop over mc digits
  for ( MCRichDigits::const_iterator iDig = mcDigits->begin();
        iDig != mcDigits->end(); ++iDig )
  {

    if ( !(*iDig)->hits().empty() )
    {
      // loop over hits for this digit
      for ( LHCb::MCRichDigitHit::Vector::const_iterator iH = (*iDig)->hits().begin();
            iH != (*iDig)->hits().end(); ++iH )
      {
        
        // if storing all associations or this is a "main" event hit, add to list
        const bool inMainEvent = (*iH).history().signalEvent();
        if ( m_storeSpill || inMainEvent )
        {

          // Make and insert new summary object
          MCRichDigitSummary * summary = new MCRichDigitSummary();
          summaries->add( summary );

          // Set RichSmartID
          summary->setRichSmartID( (*iDig)->richSmartID() );

          // Set MCParticle
          summary->setMCParticle( (*iH).mcRichHit()->mcParticle() );

          // Copy history from MCRichDigitHit ( contains simulation and digitisation history )
          summary->setHistory( (*iH).history() );

        }

      } // loop over hits for current digit

    }
    else
    {
      Warning( "MCRichDigit has no MCRichHits associated" );
    }

  } // end loop over digits

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << summaries->size() << " MCRichDigitSummary objects at "
            << MCRichDigitSummaryLocation::Default << endreq;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================

//=============================================================================
// initialisation
//=============================================================================
StatusCode MCRichDigitSummaryAlg::finalize()
{
  // finalize base class
  return RichAlgBase::finalize();
}
//=============================================================================
