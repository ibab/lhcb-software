#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"

#include "RichKernel/RichSmartID.h"

#include "RichReadout/RichFrontEndResponse.h"
#include "RichReadout/RichFrontEndDigitiser.h"
#include "RichReadout/RichRegistry.h"
#include "RichReadout/PixelReadout.h"
#include "RichReadout/RichShape.h"
#include "RichReadout/RichNoisifier.h"
#include "RichReadout/RichBase.h"
#include "RichReadout/Pixel.h"
#include "RichReadout/RichCoder.h"

#include "Event/MCRichDigit.h"

#include "CLHEP/Random/Random.h"

#include "AIDA/IHistogram1D.h"

//#include "RichReadout/RichTimeSample.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichFrontEndResponse>  s_factory ;
const         IAlgFactory& RichFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichFrontEndResponse::RichFrontEndResponse( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator )
{

  declareProperty( "Mode", m_mode = "Simple" );

  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );

  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );

  declareProperty( "SimpleCalibration", m_Calibration = 4.42 );

  declareProperty( "SimpleThreshold", m_Threshold = 8.8 );

  declareProperty( "SimpleBaseline", m_Baseline = 50 );

  declareProperty( "SimpleBaseline", m_Sigma = 0.9 );

  declareProperty( "HistoPath", m_histoPth = "RICH/Readout/" );

}

RichFrontEndResponse::~RichFrontEndResponse (){ };

StatusCode RichFrontEndResponse::initialize()
{

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Initialize" << endreq;

  if ( !bookHistograms() ) return StatusCode::FAILURE;

  StatusCode scReader =  toolSvc()->retrieveTool("CdfReader" , m_reader );
  if( scReader.isFailure() ) {
    throw GaudiException( "ToolSvc not found",
                          "RichReadout Exception", scReader );
  }

  StatusCode scFinder = toolSvc()->retrieveTool("PixelFinder" , m_finder );
  if( scFinder.isFailure()) {
    log << MSG::FATAL << "    Unable to create PixelFinder tool" << endreq;
    return StatusCode::FAILURE;
  }

  // create a collection of all pixels
  actual_base = theRegistry->GetNewBase( m_finder->PixelList(m_reader) );

  if(m_mode=="Simple"){
    m_AdcCut = int(m_Threshold*m_Calibration+m_Baseline);
  }


  return StatusCode::SUCCESS;

}


StatusCode RichFrontEndResponse::execute()
{

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

  datacache.clear(); tscache.clear();

  if ( m_mode == "Simple"   ) { this->Simple();}
  if ( m_mode == "Detailed" ) { this->Analog(); this->Digital();}

  return StatusCode::SUCCESS;

}

StatusCode RichFrontEndResponse::Simple() {


  MsgStream log(msgSvc(), name());

  MCRichDigits* mcRichDigits = new MCRichDigits();
  if ( !eventSvc()->registerObject(m_mcRichDigitsLocation, mcRichDigits) ) {
    log << MSG::ERROR << "Failed to register RichDigits at " << m_mcRichDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }

  for (MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin(); 
       iSumDep != SummedDeposits->end(); ++iSumDep){

    PixelProperties* props = actual_base->DecodeUniqueID( (*iSumDep)->key().index()  );

    if( props != 0 ) {

      StatusCode statusP;
      statusP = (0 == mcRichDigits) ? StatusCode::FAILURE : StatusCode::SUCCESS;
      if ( statusP.isFailure() ) return statusP;

      double summedEnergy = 0.0;
      for (SmartRefVector<MCRichDeposit>::const_iterator deposit = (*iSumDep)->deposits().begin();
           deposit != (*iSumDep)->deposits().end(); ++deposit) {
        if( (*deposit)->time() > 0.0 &&  (*deposit)->time() < 25.0 ) { summedEnergy += (*deposit)->energy(); }
      }

      Rndm::Numbers gauss(randSvc(), Rndm::Gauss(0.0,0.9));
      double rand = gauss();

      int value = int((summedEnergy+m_Sigma*rand)*m_Calibration ) + m_Baseline;

      m_pulseHeight->fill( value, 1.);

      if( value >= m_AdcCut) {

        MCRichDigit* newDigit = new MCRichDigit();
        if( !mcRichDigits->object( (*iSumDep)->key() ) ){
          mcRichDigits->insert( newDigit, ((*iSumDep)->key()) );
        } else {
          delete newDigit;
          log << MSG::ERROR << "Attempted duplicate digit" << endreq;
          return StatusCode::FAILURE;
        }
      }
    }
  }

  log << MSG::DEBUG << "Created " << mcRichDigits->size() << " MCRichDigits at " 
      << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;

}

StatusCode RichFrontEndResponse::Analog() {

  MsgStream log(msgSvc(), name());

  //samplecache_t::iterator tsc_it;
  //double bunchspace = 25.0;

  for ( MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    if ( tscache.count( (*iSumDep)->key().index() > 1 ) ) {
      log << MSG::ERROR << "multiple summed deposits with same key" << endreq;
      return StatusCode::FAILURE;
    }

    PixelProperties* props = actual_base->DecodeUniqueID( (*iSumDep)->key().index()  );

    if ( props != 0 ) {

      Pixel* pid = new Pixel( props );
      const PixelReadout *readOut = props->Readout();
      unsigned int tsize = readOut->FrameSize();
      //double peaktime = readOut->PeakTime();
      const RichShape* shape = readOut->Shape();
      //double sigma = readOut->SigmaElecNoise();
      //double offset = readOut->BaseLine();

      RichTimeSample ts(pid,readOut->FrameSize(),readOut->BaseLine());

      // Evolve shape --------------------------------------------------
      if ( readOut ) {

        // Inject gaussian noise
        if(readOut && readOut->Noisifier()) {
          ts += readOut->Noisifier()->noisify( ts.MyPixel() );
        } else {
          log << MSG::ERROR << " No Noisifier " << endreq;
          return StatusCode::FAILURE;
        }

        // Retrieve vector of SmartRefs to contributing deposits (non-const)
        SmartRefVector<MCRichDeposit>& deposits = (*iSumDep)->deposits();

        for( SmartRefVector<MCRichDeposit>::const_iterator iDep
               = deposits.begin(); iDep != deposits.end(); ++iDep ) {

          if ( shape ) {

            if( (*iDep)->time() > 0.0 &&  (*iDep)->time() < 25.0 ) {
              int binZero = (*iDep)->time()/0.25;
              double binTime = 0.;
              double e = (*iDep)->energy();
              for ( unsigned int bin = binZero; bin < tsize; bin++) {
                binTime += 0.25;
                ts[bin] += (*shape)(binTime)*e;
              }
            }
          }
        }

        tscache.insert(samplecache_t::value_type((*iSumDep)->key().index(),ts));

      }
      delete pid;
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode RichFrontEndResponse::Digital(){

  MsgStream log(msgSvc(), name());

  samplecache_t::iterator tsc_it;

  MCRichDigits* mcRichDigits = new MCRichDigits();

  // Register new RichDigit container to Gaudi data store
  if ( !eventSvc()->registerObject(m_mcRichDigitsLocation, mcRichDigits) ) {
    log << MSG::ERROR << "Failed to register RichDigits at "
        << m_mcRichDigitsLocation << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode statusP;

  statusP = (0 == mcRichDigits) ? StatusCode::FAILURE : StatusCode::SUCCESS;

  if ( statusP.isFailure() ) return statusP;

  for (tsc_it = tscache.begin(); tsc_it != tscache.end(); tsc_it++) {

    PixelProperties* props = actual_base->DecodeUniqueID( (*tsc_it).first  );
    const PixelReadout* readOut = new PixelReadout;
    readOut = props->Readout();
    if(readOut) {

      const RichFrontEndDigitiser* ADC = readOut->ADC();
      StatusCode scDigit = ADC->process((*tsc_it).second);

      if (scDigit.isSuccess()){
        RichSmartID id((*tsc_it).first);
        MCRichDigit* newDigit = new MCRichDigit();
        if( !mcRichDigits->object(id) ){
          mcRichDigits->insert( newDigit, id );
        } else {
          delete newDigit;
          log << MSG::ERROR << "Attempt to duplicate digit" << endreq;
          return StatusCode::FAILURE;
        }
      }

    }
  }

  log << MSG::DEBUG << "Created " << mcRichDigits->size() << " MCRichDigits at " 
      << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;

}

StatusCode RichFrontEndResponse::bookHistograms() {

  MsgStream log(msgSvc(), name());

  std::string title;

  StatusCode sc = StatusCode::SUCCESS;

  title = "Pulse Height Spectrum";
  if ( !(m_pulseHeight =
         histoSvc()->book( m_histoPth, 1, title, 300, 0, 300 )) ) {
    log << MSG::ERROR << "Failed booking '" << title << "'" << endreq;
    sc = StatusCode::FAILURE;
  }

  return sc;

}
