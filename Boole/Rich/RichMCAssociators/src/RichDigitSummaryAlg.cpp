
//-----------------------------------------------------------------------------
/** @file RichDigitSummaryAlg.cpp
 *
 * Implementation file for class : RichDigitSummaryAlg
 *
 * CVS Log :-
 * $Id: RichDigitSummaryAlg.cpp,v 1.1.1.1 2005-10-13 15:13:40 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2004-02-11
 */
//-----------------------------------------------------------------------------

// local
#include "RichDigitSummaryAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigitSummaryAlg>          s_factory;
const        IAlgFactory& RichDigitSummaryAlgFactory = s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichDigitSummaryAlg::RichDigitSummaryAlg( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : RichAlgBase  ( name , pSvcLocator ),
    m_storeSpill ( false )
{
  declareProperty( "StoreSpilloverSummaries", m_storeSpill );
}
//=============================================================================

//=============================================================================
// Destructor
//=============================================================================
RichDigitSummaryAlg::~RichDigitSummaryAlg() { }
//=============================================================================

//=============================================================================
// Initialisation
//=============================================================================
StatusCode RichDigitSummaryAlg::initialize()
{
  // Sets up various tools and services
  return RichAlgBase::initialize();
}
//=============================================================================

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichDigitSummaryAlg::execute()
{

  // Locate MCRichDigits
  MCRichDigits * mcDigits = get<MCRichDigits>( MCRichDigitLocation::Default );

  // Create new container of summary objects
  RichDigitSummarys * summaries = new RichDigitSummarys();
  put ( summaries, RichDigitSummaryLocation::Default );

  // reserve space
  summaries->reserve( mcDigits->size() );

  // loop over mc digits
  for ( MCRichDigits::const_iterator iDig = mcDigits->begin();
        iDig != mcDigits->end(); ++iDig )
  {
    // Make a new summary object
    RichDigitSummary * sum = new RichDigitSummary();
    // insert with same key (RichSmartID)
    summaries->insert( sum, (*iDig)->key() );
    // Set history info
    sum->setHistoryCode( (*iDig)->historyCode() );
    // loop over hits
    for ( SmartRefVector<MCRichHit>::const_iterator iH = (*iDig)->hits().begin();
          iH != (*iDig)->hits().end(); ++iH )
    {
      // if storing all associations or this is a "main" event hit, add to list
      if ( m_storeSpill || hitInMainEvent(*iH) )
      {
        sum->addToMCParticles( (*iH)->mcParticle() );
      }
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Stored " << summaries->size() << " RichDigitSummary objects at "
            << RichDigitSummaryLocation::Default << endreq;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichDigitSummaryAlg::finalize()
{
  return RichAlgBase::finalize();
}
//=============================================================================

