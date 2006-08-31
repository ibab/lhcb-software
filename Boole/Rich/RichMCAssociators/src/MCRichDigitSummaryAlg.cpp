
//-----------------------------------------------------------------------------
/** @file MCRichDigitSummaryAlg.cpp
 *
 * Implementation file for class : MCRichDigitSummaryAlg
 *
 * CVS Log :-
 * $Id: MCRichDigitSummaryAlg.cpp,v 1.4 2006-08-31 16:58:52 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2004-02-11
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichDigitSummaryAlg.h"

// namespace
using namespace LHCb;

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichDigitSummaryAlg>          s_factory;
const        IAlgFactory& MCRichDigitSummaryAlgFactory = s_factory;

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
  declareProperty( "HitLocation",
                   m_RichHitLoc =  MCRichHitLocation::Default );
  declareProperty( "PrevLocation",
                   m_RichPrevLoc = "Prev/" + MCRichHitLocation::Default );
  declareProperty( "PrevPrevLocation",
                   m_RichPrevPrevLoc = "PrevPrev/" + MCRichHitLocation::Default );
  declareProperty( "NextLocation",
                   m_RichNextLoc = "Next/" + MCRichHitLocation::Default );
  declareProperty( "NextNextLocation",
                   m_RichNextNextLoc = "NextNext/" + MCRichHitLocation::Default );
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

    // Charge share hit ?
    const bool chargeShare = (*iDig)->history().chargeShareHit();

    if ( !(*iDig)->hits().empty() )
    {
      // loop over hits for this digit
      for ( SmartRefVector<MCRichHit>::const_iterator iH = (*iDig)->hits().begin();
            iH != (*iDig)->hits().end(); ++iH )
      {

        // if storing all associations or this is a "main" event hit, add to list
        const bool inMainEvent = hitInMainEvent(*iH);
        if ( m_storeSpill || inMainEvent )
        {

          // Make and insert new summary object
          MCRichDigitSummary * summary = new MCRichDigitSummary();
          summaries->add( summary );

          // Set RichSmartID
          summary->setRichSmartID( (*iDig)->richSmartID() );

          // Set MCParticle
          summary->setMCParticle( (*iH)->mcParticle() );

          // Copy history from MCRichHit
          MCRichDigitHistoryCode hist = (*iH)->mcRichDigitHistoryCode();

          // Which event
          if      ( inMainEvent )                            { hist.setSignalEvent(true);   }
          else if ( hitInSpillEvent(*iH,m_RichPrevLoc) )     { hist.setPrevEvent(true);     }
          else if ( hitInSpillEvent(*iH,m_RichPrevPrevLoc) ) { hist.setPrevPrevEvent(true); }
          else if ( hitInSpillEvent(*iH,m_RichNextLoc) )     { hist.setNextEvent(true);     }
          else if ( hitInSpillEvent(*iH,m_RichNextNextLoc) ) { hist.setNextNextEvent(true); }

          // If digit is charge-share - set this flag also
          if ( chargeShare ) { hist.setChargeShareHit(true); }

          // update history in data object
          summary->setHistory(hist);

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
