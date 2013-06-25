
//===============================================================================
/** @file RichCopySummedDepositsToDigits.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichCopySummedDepositsToDigits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   18/11/2011
 */
//===============================================================================

#include "RichCopySummedDepositsToDigits.h"

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( CopySummedDepositsToDigits )

// Standard constructor, initializes variables
  CopySummedDepositsToDigits::CopySummedDepositsToDigits( const std::string& name,
                                                          ISvcLocator* pSvcLocator )
    : Rich::AlgBase ( name, pSvcLocator )
{
  // job opts
  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = LHCb::MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = LHCb::MCRichDigitLocation::Default );
}

CopySummedDepositsToDigits::~CopySummedDepositsToDigits () { }

StatusCode CopySummedDepositsToDigits::execute()
{
  const LHCb::MCRichSummedDeposits * SummedDeposits =
    get<LHCb::MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Successfully located " << SummedDeposits->size()
            << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endmsg;

  // make new mcrichdigits
  LHCb::MCRichDigits* mcRichDigits = new LHCb::MCRichDigits();
  put ( mcRichDigits, m_mcRichDigitsLocation );

  for ( LHCb::MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep )
  {

    // Make new MCRichDigit
    LHCb::MCRichDigit * newDigit = new LHCb::MCRichDigit();

    double summedEnergy = 0.0;
    LHCb::MCRichDigitHit::Vector hitVect;
    for ( SmartRefVector<LHCb::MCRichDeposit>::const_iterator deposit =
            (*iSumDep)->deposits().begin();
          deposit != (*iSumDep)->deposits().end(); ++deposit )
    {
      summedEnergy += (*deposit)->energy();
      hitVect.push_back( LHCb::MCRichDigitHit( *((*deposit)->parentHit()),
                                               (*deposit)->history() ) );
    }
    newDigit->setHits( hitVect );

    (*iSumDep)->setSummedEnergy( summedEnergy );

    // Store history info
    newDigit->setHistory( (*iSumDep)->history() );

    // save the digit
    mcRichDigits->insert( newDigit, (*iSumDep)->key() );

  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << mcRichDigits->size() << " MCRichDigits at "
            << m_mcRichDigitsLocation << endmsg;

  return StatusCode::SUCCESS;
}
