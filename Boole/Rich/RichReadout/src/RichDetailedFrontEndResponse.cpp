
#include "RichDetailedFrontEndResponse.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDetailedFrontEndResponse>  s_factory ;
const         IAlgFactory& RichDetailedFrontEndResponseFactory = s_factory ;

// Standard constructor, initializes variables
RichDetailedFrontEndResponse::RichDetailedFrontEndResponse( const std::string& name,
                                                            ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator )
{

  // job options
  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  //  declareProperty( "SimpleCalibration", m_Calibration = 12500. );
  declareProperty( "SimpleCalibration", m_Calibration = 8330. );
  declareProperty( "SimpleBaseline",    m_Pedestal = 50 );
  declareProperty( "Noise",             m_Noise = 150. );  // in electrons
  declareProperty( "Threshold",         m_Threshold = 1400. ); // in electrons
  declareProperty( "ThresholdSigma",    m_ThresholdSigma = 140. ); // in electrons

  el_per_adc = 40.;

  Rndm::Numbers m_gaussThreshold;
  Rndm::Numbers m_gaussNoise; // currently hardwired to be 150 electrons (sigma) hack

}

RichDetailedFrontEndResponse::~RichDetailedFrontEndResponse() {}

StatusCode RichDetailedFrontEndResponse::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // create a collection of all pixels
  IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool" , smartIDs );
  const RichSmartID::Collection & pixels = smartIDs->readoutChannelList();
  actual_base = theRegistry.GetNewBase( pixels );
  releaseTool( smartIDs );

  // initialise random number generators
  m_gaussNoise.initialize     ( randSvc(), Rndm::Gauss(0., m_Noise)                  );
  m_gaussThreshold.initialize ( randSvc(), Rndm::Gauss(m_Threshold,m_ThresholdSigma) );

  return sc;
}

StatusCode RichDetailedFrontEndResponse::finalize()
{
  // finalise random number generators
  m_gaussNoise.finalize();
  m_gaussThreshold.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

StatusCode RichDetailedFrontEndResponse::execute()
{
  debug() << "Execute" << endreq;

  m_summedDeposits = get<MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully located " << m_summedDeposits->size()
            << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endreq;
  }

  // Clear time sample cache
  tscache.clear();
  //tscache.reserve( m_summedDeposits->size() );

  // Run analog sim
  const StatusCode sc = Analog();
  if ( sc.isFailure() ) { return sc; }

  // run digital sim and return
  return Digital();
}

StatusCode RichDetailedFrontEndResponse::Analog()
{
  debug() << "Analogue Simulation" << endreq;

  for ( MCRichSummedDeposits::const_iterator iSumDep = m_summedDeposits->begin();
        iSumDep != m_summedDeposits->end(); ++iSumDep ) 
  {

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Summed Deposit " << (*iSumDep)->key() << endreq;
    }

    RichPixelProperties* props = actual_base->DecodeUniqueID( (*iSumDep)->key() );
    if ( !props ) continue;

    const RichPixelReadout * readOut = props->Readout();
    if ( !readOut )  continue;

    const RichShape* shape = readOut->Shape();
    if ( shape )
    {

      // Create time sample for this summed deposit
      RichTimeSample ts(readOut->FrameSize(),readOut->BaseLine());
      //tscache.push_back( TimeData( *iSumDep, 
      //                             RichTimeSample( readOut->FrameSize(),
      //                                             readOut->BaseLine() ) ) );
      //RichTimeSample & ts = tscache.back().second;

      // Retrieve vector of SmartRefs to contributing deposits (non-const)
      SmartRefVector<MCRichDeposit>& deposits = (*iSumDep)->deposits();

      for ( SmartRefVector<MCRichDeposit>::const_iterator iDep
             = deposits.begin(); iDep != deposits.end(); ++iDep ) 
      {

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " Deposit " << (*iDep)->key()
                    << " from '" << objectLocation( (*iDep)->parentHit()->parent() ) << "'" << endreq
                    << "  -> TOF     = " << (*iDep)->time() << endreq
                    << "  -> Energy  = " << (*iDep)->energy() << endreq;
        }

        // Course cut on deposit TOF ( -50ns to 50ns )
        if ( fabs((*iDep)->time()) < 50 ) 
        {

          // Bin zero
          const int binZero = (int)(*iDep)->time();

          // Shift time ( Rich2 value correlated to -40 in RichSignal algorithm.... )
          const double shiftTime = ( Rich::Rich1 == (*iDep)->parentHit()->rich() ? 18 : 7 );

          // origin time
          double binTime = shiftTime - binZero; 

          // dead region
          const bool dead = ( binZero < 0 && binZero > -50 );

          // electrons
          const double e  = ( dead ? 0 : ((*iDep)->energy()*m_Calibration) + m_gaussNoise()/el_per_adc );

          // Loop over time sample and fill for this deposit
          for ( unsigned int bin = 0; bin < ts.size(); ++bin )
          {
            binTime += 25. / readOut->FrameSize();
            ts[bin] += ( dead ? -999999 : (*shape)[binTime] * e );
          }

        }

      } // MCRichDeposit loop

      tscache.insert( samplecache_t::value_type( *iSumDep, ts ) );

    } // if shape

  }

  return StatusCode::SUCCESS;
}

StatusCode RichDetailedFrontEndResponse::Digital()
{
  debug() << "Digital Simulation" << endreq;

  // new RichDigit container to Gaudi data store
  MCRichDigits * mcRichDigits = new MCRichDigits();

  for ( samplecache_t::iterator tsc_it = tscache.begin();
        tsc_it != tscache.end(); ++tsc_it ) 
  {

    RichPixelProperties* props = actual_base->DecodeUniqueID( ((*tsc_it).first)->key() );
    const RichPixelReadout* readOut = props->Readout();
    if ( readOut )
    {

      const double temp_threshold = m_gaussThreshold()/el_per_adc + readOut->BaseLine();
      if ( readOut->ADC()->process((*tsc_it).second,temp_threshold) ) 
      {      
        
        MCRichDigit* newDigit = new MCRichDigit();
        mcRichDigits->insert( newDigit, ((*tsc_it).first)->key().pixelID() );

        // Create MCRichHit links
        SmartRefVector<MCRichDeposit> & deps = ((*tsc_it).first)->deposits();
        for ( SmartRefVector<MCRichDeposit>::iterator iDep = deps.begin();
              iDep != deps.end(); ++iDep )
        {
          newDigit->addToHits( (*iDep)->parentHit() );
        }

        // Store history info
        newDigit->setHistoryCode( ((*tsc_it).first)->historyCode() );

      }

    } // readout exists

  } // loop over time samples

  put( mcRichDigits, m_mcRichDigitsLocation );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Registered " << mcRichDigits->size()
            << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;
  }

  return StatusCode::SUCCESS;
}
