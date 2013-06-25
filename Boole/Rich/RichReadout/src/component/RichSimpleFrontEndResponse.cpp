
//===============================================================================
/** @file RichSimpleFrontEndResponse.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSimpleFrontEndResponse
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSimpleFrontEndResponse.h"

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( SimpleFrontEndResponse )

// Standard constructor, initializes variables
  SimpleFrontEndResponse::SimpleFrontEndResponse( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
    : Rich::AlgBase ( name, pSvcLocator ),
      actual_base   ( NULL ),
      SummedDeposits( NULL ),
      m_AdcCut      ( 85 )
{

  // job opts
  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = LHCb::MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = LHCb::MCRichDigitLocation::Default );
  declareProperty( "SimpleCalibration", m_Calibration = 4420 );
  declareProperty( "SimpleBaseline", m_Baseline = 50 );
  declareProperty( "SimpleSigma", m_Sigma = 1.0 );

  Rndm::Numbers m_gaussRndm;
}

SimpleFrontEndResponse::~SimpleFrontEndResponse () { }

StatusCode SimpleFrontEndResponse::initialize()
{
  // Initialize base class
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // create a collection of all pixels
  const Rich::ISmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs, 0, true );
  const LHCb::RichSmartID::Vector & pixels = smartIDs->readoutChannelList();
  actual_base = theRegistry.GetNewBase( pixels );
  releaseTool( smartIDs );

  // Gauss randomn dist
  m_gaussRndm.initialize( randSvc(), Rndm::Gauss(0.0,0.9) );

  return sc;
}

StatusCode SimpleFrontEndResponse::finalize()
{
  // finalize randomn number generator
  m_gaussRndm.finalize();

  // finalize base class
  return Rich::AlgBase::finalize();
}

StatusCode SimpleFrontEndResponse::execute()
{
  SummedDeposits = get<LHCb::MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Successfully located " << SummedDeposits->size()
            << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endmsg;

  // Run the Simple treatment
  Simple();

  return StatusCode::SUCCESS;
}

StatusCode SimpleFrontEndResponse::Simple()
{

  // make new mcrichdigits
  LHCb::MCRichDigits* mcRichDigits = new LHCb::MCRichDigits();
  put ( mcRichDigits, m_mcRichDigitsLocation );

  for ( LHCb::MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    RichPixelProperties* props =
      actual_base->DecodeUniqueID( (*iSumDep)->key() );
    if ( props ) {

      // Make new MCRichDigit
      LHCb::MCRichDigit * newDigit = new LHCb::MCRichDigit();

      double summedEnergy = 0.0;
      LHCb::MCRichDigitHit::Vector hitVect;
      for ( SmartRefVector<LHCb::MCRichDeposit>::const_iterator deposit =
              (*iSumDep)->deposits().begin();
            deposit != (*iSumDep)->deposits().end(); ++deposit )
      {
        if ( (*deposit)->time() > 0.0 &&
             (*deposit)->time() < 25.0 ) {
          summedEnergy += (*deposit)->energy();
          hitVect.push_back( LHCb::MCRichDigitHit( *((*deposit)->parentHit()),
                                                   (*deposit)->history() ) );
        }
      }
      newDigit->setHits( hitVect );

      (*iSumDep)->setSummedEnergy( summedEnergy );

      // Store history info
      newDigit->setHistory( (*iSumDep)->history() );

      const int value = int((summedEnergy+m_Sigma*m_gaussRndm()/1000)*m_Calibration) + m_Baseline;
      if ( !newDigit->hits().empty() && value >= m_AdcCut )
      {
        mcRichDigits->insert( newDigit, (*iSumDep)->key() );
      }
      else
      {
        delete newDigit;
      }

    }

  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << mcRichDigits->size() << " MCRichDigits at "
            << m_mcRichDigitsLocation << endmsg;

  return StatusCode::SUCCESS;
}
