
#include "RichDetailedMaPMTFrontEndResponse.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDetailedMaPMTFrontEndResponse>  s_factory ;
const         IAlgFactory& RichDetailedMaPMTFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichDetailedMaPMTFrontEndResponse::RichDetailedMaPMTFrontEndResponse( const std::string& name,
                                                                      ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ) {

  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "SimpleCalibration", m_Calibration = 4420 );
  declareProperty( "SimpleBaseline",    m_Baseline = 50 );

}

RichDetailedMaPMTFrontEndResponse::~RichDetailedMaPMTFrontEndResponse (){ };

StatusCode RichDetailedMaPMTFrontEndResponse::initialize() {

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
    if ( !toolSvc()->retrieveTool("RichDetInterface" , detint ) ) {
      log << MSG::FATAL << "Unable to create RichDetInterface" << endreq;
      return StatusCode::FAILURE;
    }
    detint->readoutChannelList(pixels);
    toolSvc()->releaseTool(detint);
  }
  actual_base = theRegistry.GetNewBase( pixels );

  log << MSG::DEBUG
      << " Using detailed MaPMT frontend response algorithm for " << m_detMode << endreq
      << " Acquired information for " << pixels.size() << " pixels" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedMaPMTFrontEndResponse::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "finalize" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedMaPMTFrontEndResponse::execute() {

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

  tscache.clear();

  // Process the hits
  Analog();
  Digital();

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedMaPMTFrontEndResponse::Analog() {

  MsgStream log(msgSvc(), name());

  for ( MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    RichPixelProperties* props =
      actual_base->DecodeUniqueID( (*iSumDep)->key().index()  );
    if ( !props ) continue;

    const RichPixelReadout * readOut = props->Readout();
    if ( !readOut )  continue;

    RichPixel * pid = new RichPixel(props);
    unsigned int tsize = readOut->FrameSize();
    const RichShape* shape = readOut->Shape();
    if ( shape ) {

      RichTimeSample ts(pid,readOut->FrameSize(),readOut->BaseLine());

      // Evolve shape --------------------------------------------------

      // Inject gaussian noise
      if ( readOut->Noisifier() ) {
        ts += readOut->Noisifier()->noisify( ts.MyPixel() );
      } else {
        log << MSG::ERROR << " No Noisifier " << endreq;
        return StatusCode::FAILURE;
      }

      // Retrieve vector of SmartRefs to contributing deposits (non-const)
      SmartRefVector<MCRichDeposit>& deposits = (*iSumDep)->deposits();

      double summedEnergy = 0.0;
      for( SmartRefVector<MCRichDeposit>::const_iterator iDep
             = deposits.begin(); iDep != deposits.end(); ++iDep ) {
        if( (*iDep)->time() > 0.0 &&  (*iDep)->time() < 25.0 ) {
          int binZero = (int)( (*iDep)->time()/0.25 );
          double binTime = 0.;
          double e = ( (*iDep)->energy()*m_Calibration );
          summedEnergy += (*iDep)->energy();
          for ( unsigned int bin = binZero; bin < tsize; ++bin ) {
            binTime += 1.;
            ts[bin] += (*shape)(binTime)*e;
          }
        }
      } // MCrichDeposit loop
      (*iSumDep)->setSummedEnergy(summedEnergy);

      tscache.insert( samplecache_t::value_type( (*iSumDep), ts ) );

    } // if shape

    delete pid;
  }

  return StatusCode::SUCCESS;

}

StatusCode RichDetailedMaPMTFrontEndResponse::Digital() {

  MsgStream log(msgSvc(), name());

  // Register new RichDigit container to Gaudi data store
  MCRichDigits* mcRichDigits = new MCRichDigits();
  if ( !eventSvc()->registerObject(m_mcRichDigitsLocation, mcRichDigits) ) {
    log << MSG::ERROR << "Failed to register RichDigits at "
        << m_mcRichDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }

  for ( samplecache_t::iterator tsc_it = tscache.begin();
        tsc_it != tscache.end(); ++tsc_it ) {

    RichPixelProperties* props = actual_base->DecodeUniqueID( ((*tsc_it).first)->key().index() );
    const RichPixelReadout* readOut = props->Readout();
    if ( readOut ) {

      if ( readOut->ADC()->process((*tsc_it).second) ) {
        MCRichDigit* newDigit = new MCRichDigit();
        mcRichDigits->insert( newDigit, ((*tsc_it).first)->key() );
        // Create MCRichHit links
        SmartRefVector<MCRichDeposit> & deps = ((*tsc_it).first)->deposits();
        for ( SmartRefVector<MCRichDeposit>::iterator iDep = deps.begin();
              iDep != deps.end(); ++iDep ) {
          newDigit->addToHits( (*iDep)->parentHit() );
        }
      }

    }
  }

  log << MSG::DEBUG
      << "Created " << mcRichDigits->size() << " MCRichDigits at "
      << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;
}
