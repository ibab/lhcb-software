
//-----------------------------------------------------------------------------
/** @file MCRichDigitSummaryAlg.cpp
 *
 * Implementation file for class : MCRichDigitSummaryAlg
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
using namespace Rich::MC;

DECLARE_ALGORITHM_FACTORY( MCRichDigitSummaryAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCRichDigitSummaryAlg::MCRichDigitSummaryAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : Rich::AlgBase ( name , pSvcLocator ),
    m_storeSpill  ( false ),
    m_truth       ( NULL )
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
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // tool
  acquireTool( "RichMCTruthTool", m_truth, NULL, true );

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
  LHCb::MCRichDigits * mcDigits = get<LHCb::MCRichDigits>( LHCb::MCRichDigitLocation::Default );

  // Make new container of MCRichDigitSummaries
  LHCb::MCRichDigitSummarys * summaries = new LHCb::MCRichDigitSummarys();
  put ( summaries, LHCb::MCRichDigitSummaryLocation::Default );

  // loop over mc digits
  for ( LHCb::MCRichDigits::const_iterator iDig = mcDigits->begin();
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
          LHCb::MCRichDigitSummary * summary = new LHCb::MCRichDigitSummary();
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
            << LHCb::MCRichDigitSummaryLocation::Default << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
