
#include "RichDetailedFrontEndResponse.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDetailedFrontEndResponse>  s_factory ;
const         IAlgFactory& RichDetailedFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichDetailedFrontEndResponse::RichDetailedFrontEndResponse( const std::string& name,
                                                            ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) {

  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  //  declareProperty( "SimpleCalibration", m_Calibration = 12500. );
  declareProperty( "SimpleCalibration", m_Calibration = 8330. );
  declareProperty( "SimpleBaseline",    m_Pedestal = 50 );
  declareProperty( "Noise", m_Noise = 150. );  // in electrons
  declareProperty( "Threshold", m_Threshold = 1400. ); // in electrons
  declareProperty( "ThresholdSigma", m_ThresholdSigma = 140. ); // in electrons
  //  declareProperty( "ThresholdSigma", m_ThresholdSigma = 35?. ); // in electrons - Jolly improvement

  declareProperty( "HistoPath", m_histPth = "RICH/DIGI/Readout/" );

  el_per_adc = 40.;

  Rndm::Numbers m_gaussThreshold;
  Rndm::Numbers m_gaussNoise; // currently hardwired to be 150 electrons (sigma) hack

}

RichDetailedFrontEndResponse::~RichDetailedFrontEndResponse() {}

StatusCode RichDetailedFrontEndResponse::initialize() {

  // Initialize base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // create a collection of all pixels
  IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs );
  const RichSmartID::Collection & pixels = smartIDs->readoutChannelList();
  releaseTool( smartIDs );
  actual_base = theRegistry.GetNewBase( pixels );

  m_gaussNoise.initialize( randSvc(), Rndm::Gauss(0., m_Noise) );
  //m_GaussThreshold.initialize( randSvc(), Rndm::Gauss(m_adc_cut,200./20.)));
  m_gaussThreshold.initialize( randSvc(), Rndm::Gauss(m_Threshold,m_ThresholdSigma));

  debug() << " Using detailed HPD frontend response algorithm" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedFrontEndResponse::finalize()
{
  debug() << "finalize" << endreq;

  // finalise randomn number generators
  m_gaussNoise.finalize();
  m_gaussThreshold.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

StatusCode RichDetailedFrontEndResponse::execute() {

  debug() << "Execute" << endreq;

  SummedDeposits = get<MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );
  debug() << "Successfully located " << SummedDeposits->size()
          << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endreq;

  tscache.clear();

  // Process the hits
  Analog();
  Digital();

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedFrontEndResponse::Analog() {

  for ( MCRichSummedDeposits::const_iterator iSumDep = SummedDeposits->begin();
        iSumDep != SummedDeposits->end(); ++iSumDep ) {

    RichPixelProperties* props =
      actual_base->DecodeUniqueID( (*iSumDep)->key() );
    if ( !props ) continue;

    const RichPixelReadout * readOut = props->Readout();
    if ( !readOut )  continue;

    RichPixel * pid = new RichPixel(props);
    unsigned int tsize = readOut->FrameSize();
    const RichShape* shape = readOut->Shape();

    if ( shape ) {

      RichTimeSample ts(pid,readOut->FrameSize(),readOut->BaseLine());

      // Evolve shape --------------------------------------------------

      // Retrieve vector of SmartRefs to contributing deposits (non-const)
      SmartRefVector<MCRichDeposit>& deposits = (*iSumDep)->deposits();

      double summedEnergy = 0.0;
      for( SmartRefVector<MCRichDeposit>::const_iterator iDep
             = deposits.begin(); iDep != deposits.end(); ++iDep ) {
 
        if( (*iDep)->time() > -100.0 &&  (*iDep)->time() < 100.0 ) {

          int binZero = (int)( (*iDep)->time());
          double binTime = -binZero + 25.; // locked into peakTime - hardwired hack

          double e = ( (*iDep)->energy()*m_Calibration ) + m_gaussNoise()/el_per_adc;
          summedEnergy += (*iDep)->energy();

          for ( unsigned int bin = 0; bin < ts.size(); ++bin ) {
            binTime += 25./tsize;
            if ( binZero < 0 && binZero > -50 ) {
              // make pixel dead for this event
              ts[bin] -= 999999;
            } else {
              ts[bin] += (*shape)[binTime] * e;
            }
          }
        }

      } // MCrichDeposit loop
      (*iSumDep)->setSummedEnergy(summedEnergy);
 
      tscache.insert( samplecache_t::value_type( *iSumDep, ts ) );

    } // if shape

    delete pid;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedFrontEndResponse::Digital() {

  // new RichDigit container to Gaudi data store
  MCRichDigits * mcRichDigits = new MCRichDigits();

  for ( samplecache_t::iterator tsc_it = tscache.begin();
        tsc_it != tscache.end(); ++tsc_it ) {

    RichPixelProperties* props = 
      actual_base->DecodeUniqueID( ((*tsc_it).first)->key() );
    const RichPixelReadout* readOut = props->Readout();
    if ( readOut ) {

      double temp_threshold = m_gaussThreshold()/el_per_adc + readOut->BaseLine();
 
      if ( readOut->ADC()->process((*tsc_it).second,temp_threshold) ) {

        MCRichDigit* newDigit = new MCRichDigit();
        mcRichDigits->insert( newDigit, ((*tsc_it).first)->key().pixelID() );

        // Create MCRichHit links
        SmartRefVector<MCRichDeposit> & deps = ((*tsc_it).first)->deposits();
        for ( SmartRefVector<MCRichDeposit>::iterator iDep = deps.begin();
              iDep != deps.end(); ++iDep ) {
          newDigit->addToHits( (*iDep)->parentHit() );
        }

      }

    } // readout exits
  }

  put( mcRichDigits, m_mcRichDigitsLocation );
  debug() << "Registered " << mcRichDigits->size() 
          << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;

  return StatusCode::SUCCESS;
}
