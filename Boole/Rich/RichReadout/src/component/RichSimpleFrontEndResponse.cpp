
//===============================================================================
/** @file RichSimpleFrontEndResponse.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSimpleFrontEndResponse
 *
 *  CVS Log :-
 *  $Id: RichSimpleFrontEndResponse.cpp,v 1.5 2006-09-01 10:33:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSimpleFrontEndResponse.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSimpleFrontEndResponse>  s_factory ;
const         IAlgFactory& RichSimpleFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichSimpleFrontEndResponse::RichSimpleFrontEndResponse( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ) 
{

  // job opts
  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "SimpleCalibration", m_Calibration = 4420 );
  declareProperty( "SimpleBaseline", m_Baseline = 50 );
  declareProperty( "SimpleSigma", m_Sigma = 1.0 );

  Rndm::Numbers m_gaussRndm;
}

RichSimpleFrontEndResponse::~RichSimpleFrontEndResponse () { };

StatusCode RichSimpleFrontEndResponse::initialize() 
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // create a collection of all pixels
  const IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs, 0, true );
  const RichSmartID::Vector & pixels = smartIDs->readoutChannelList();
  actual_base = theRegistry.GetNewBase( pixels );
  releaseTool( smartIDs );

  m_AdcCut = 85;

  // Gauss randomn dist
  m_gaussRndm.initialize( randSvc(), Rndm::Gauss(0.0,0.9) );

  return sc;
}

StatusCode RichSimpleFrontEndResponse::finalize() 
{
  // finalize randomn number generator
  m_gaussRndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

StatusCode RichSimpleFrontEndResponse::execute() {

  debug() << "Execute" << endreq;

  SummedDeposits = get<MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );
  debug() << "Successfully located " << SummedDeposits->size()
          << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endreq;

  // Run the Simple treatment
  Simple();

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleFrontEndResponse::Simple() {

  // make new mcrichdigits
  MCRichDigits* mcRichDigits = new MCRichDigits();
  put ( mcRichDigits, m_mcRichDigitsLocation );

  for ( MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    RichPixelProperties* props = 
      actual_base->DecodeUniqueID( (*iSumDep)->key() );
    if ( props ) {

      // Make new MCRichDigit
      MCRichDigit * newDigit = new MCRichDigit();

      double summedEnergy = 0.0;
      LHCb::MCRichDigitHit::Vector hitVect;
      for ( SmartRefVector<MCRichDeposit>::const_iterator deposit =
              (*iSumDep)->deposits().begin();
            deposit != (*iSumDep)->deposits().end(); ++deposit ) {
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

      int value = int((summedEnergy+m_Sigma*m_gaussRndm()/1000)*m_Calibration) + m_Baseline;
      if ( !newDigit->hits().empty() && value >= m_AdcCut ) {
        mcRichDigits->insert( newDigit, (*iSumDep)->key() );
      } else {
        delete newDigit;
      }

    }

  }

  debug() << "Created " << mcRichDigits->size() << " MCRichDigits at "
          << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;
}
