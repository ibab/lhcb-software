
#include "RichSimpleMaPMTFrontEndResponse.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSimpleMaPMTFrontEndResponse>  s_factory ;
const         IAlgFactory& RichSimpleMaPMTFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichSimpleMaPMTFrontEndResponse::RichSimpleMaPMTFrontEndResponse( const std::string& name,
                                                                  ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "SimpleCalibration", m_Calibration = 4420 );
  declareProperty( "SimpleBaseline", m_Baseline = 50 );
  declareProperty( "SimpleSigma", m_Sigma = 1.0 );
  declareProperty( "DetectorMode", m_detMode = "GAUSS" );

  Rndm::Numbers m_gaussRndm;
}

RichSimpleMaPMTFrontEndResponse::~RichSimpleMaPMTFrontEndResponse (){ };

StatusCode RichSimpleMaPMTFrontEndResponse::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  // create a collection of all pixels
  std::vector<RichSmartID> pixels;
  if ( "SICB" == m_detMode ) {
    IPixelFinder* finder;
    if ( !toolSvc()->retrieveTool("PixelFinder", finder ) ) {
      log << MSG::FATAL << "Unable to create PixelFinder tool" << endreq;
      return StatusCode::FAILURE;
    }
    finder->PixelList(pixels);
    toolSvc()->releaseTool(finder);
  } else if ( "GAUSS" == m_detMode ) {
    IRichDetInterface * detint;
    if ( !toolSvc()->retrieveTool("RichDetInterface", detint ) ) {
      log << MSG::FATAL << "Unable to create RichDetInterface" << endreq;
      return StatusCode::FAILURE;
    }
    detint->readoutChannelList(pixels);
    toolSvc()->releaseTool(detint);
  }
  actual_base = theRegistry.GetNewBase( pixels );

  m_AdcCut = 85;

  // Gauss randomn dist
  m_gaussRndm.initialize( randSvc(), Rndm::Gauss(0.0,0.9) );

  log << MSG::DEBUG
      << " Using simple MaPMT frontend response algorithm for " << m_detMode << endreq
      << " Acquired information for " << pixels.size() << " pixels" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleMaPMTFrontEndResponse::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize" << endreq;

  // finalize randomn number generator
  m_gaussRndm.finalize();

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleMaPMTFrontEndResponse::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Execute" << endreq;

  SmartDataPtr<MCRichSummedDeposits> DepPtr( eventSvc(),
                                             m_mcRichSummedDepositsLocation );
  SummedDeposits = DepPtr;
  if ( !SummedDeposits ) {
    log << MSG::WARNING << "Cannot locate MCRichSummedDeposits at "
        << m_mcRichSummedDepositsLocation << endreq;
    return StatusCode::FAILURE;
  } else {
    log << MSG::DEBUG << "Successfully located " << SummedDeposits->size()
        << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endreq;
  }

  // Run the Simple treatment
  Simple();

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleMaPMTFrontEndResponse::Simple() {

  MsgStream log(msgSvc(), name());

  MCRichDigits* mcRichDigits = new MCRichDigits();
  if ( !eventSvc()->registerObject(m_mcRichDigitsLocation, mcRichDigits) ) {
    log << MSG::ERROR << "Failed to register MCRichDigits at " << m_mcRichDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }

  for ( MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    RichPixelProperties* props = actual_base->DecodeUniqueID( (*iSumDep)->key().index()  );
    if ( props ) {

      // Make new MCRichDigit
      MCRichDigit * newDigit = new MCRichDigit();

      double summedEnergy = 0.0;
      for (SmartRefVector<MCRichDeposit>::const_iterator deposit =
             (*iSumDep)->deposits().begin();
           deposit != (*iSumDep)->deposits().end(); ++deposit ) {
        if ( (*deposit)->time() > 0.0 &&
             (*deposit)->time() < 25.0 ) {
          summedEnergy += (*deposit)->energy();
          newDigit->addToHits( (*deposit)->parentHit() );
        }
      }
      (*iSumDep)->setSummedEnergy( summedEnergy );

      int value = int((summedEnergy+m_Sigma*m_gaussRndm()/1000)*m_Calibration ) + m_Baseline;
      if ( value >= m_AdcCut) {
        mcRichDigits->insert( newDigit, ((*iSumDep)->key()) );
      } else {
        delete newDigit;
      }

    }
  }

  log << MSG::DEBUG << "Created " << mcRichDigits->size() << " MCRichDigits at "
      << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;

}
