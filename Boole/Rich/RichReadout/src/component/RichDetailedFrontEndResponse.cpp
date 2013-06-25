
//===============================================================================
/** @file RichDetailedFrontEndResponse.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichDetailedFrontEndResponse
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichDetailedFrontEndResponse.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( DetailedFrontEndResponse )

// Standard constructor, initializes variables
DetailedFrontEndResponse::DetailedFrontEndResponse( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : Rich::AlgBase ( name, pSvcLocator ),
    actual_base   ( NULL ),
    theRegistry   ( NULL ),
    m_summedDeposits(NULL),
    m_timeShift   ( Rich::NRiches )
{

  // job options
  declareProperty( "MCRichSummedDepositsLocation",
                   m_mcRichSummedDepositsLocation = LHCb::MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = LHCb::MCRichDigitLocation::Default );
  //  declareProperty( "SimpleCalibration", m_Calibration = 12500. );
  declareProperty( "SimpleCalibration", m_Calibration = 8330. );
  declareProperty( "SimpleBaseline",    m_Pedestal = 50 );
  declareProperty( "Noise",             m_Noise = 150. );  // in electrons
  declareProperty( "Threshold",         m_Threshold = 1400. ); // in electrons
  declareProperty( "ThresholdSigma",    m_ThresholdSigma = 140. ); // in electrons

   // Shift time ( value correlated settings in RichSignal )
  m_timeShift[Rich::Rich1] = 10;
  m_timeShift[Rich::Rich2] = 6;
  declareProperty( "TimeCalib", m_timeShift );

  el_per_adc = 40.;

  Rndm::Numbers m_gaussThreshold;
  Rndm::Numbers m_gaussNoise; // currently hardwired to be 150 electrons (sigma) hack

}

DetailedFrontEndResponse::~DetailedFrontEndResponse() {}

StatusCode DetailedFrontEndResponse::initialize()
{
  // Initialize base class
  StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // create a collection of all pixels
  const Rich::ISmartIDTool * smartIDs(NULL);
  acquireTool( "RichSmartIDTool" , smartIDs, 0, true );
  const LHCb::RichSmartID::Vector & pixels = smartIDs->readoutChannelList();
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Retrieved " << pixels.size() << " pixels in active list" << endmsg;
  theRegistry = new RichRegistry();
  actual_base = theRegistry->GetNewBase( pixels );
  if ( !actual_base )
  {
    return Error( "Failed to intialise pixel list" );
  }

  // initialise random number generators
  sc = ( m_gaussNoise.initialize     ( randSvc(), Rndm::Gauss(0., m_Noise)                  ) &&
         m_gaussThreshold.initialize ( randSvc(), Rndm::Gauss(m_Threshold,m_ThresholdSigma) ) );
  if ( sc.isFailure() ) { return Error( "Failed to initialise random number generators", sc ); }

  releaseTool( smartIDs );

  info() << "Time Calibration (Rich1/RICH2) = " << m_timeShift << endmsg;
 
  return sc;
}

StatusCode DetailedFrontEndResponse::finalize()
{
  // clean up
  delete actual_base;
  //delete theRegistry; // CRJ :  Disabled since causes a crash. For Marco A. to fix ;)

  // finalise random number generators
  const StatusCode sc = ( m_gaussNoise.finalize() && m_gaussThreshold.finalize() );
  if ( sc.isFailure() ) { Warning( "Failed to finalise random number generators" ).ignore(); }

  // finalize base class
  return Rich::AlgBase::finalize();
}

StatusCode DetailedFrontEndResponse::execute()
{
    if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endmsg;

  m_summedDeposits = get<LHCb::MCRichSummedDeposits>( m_mcRichSummedDepositsLocation );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully located " << m_summedDeposits->size()
            << " MCRichSummedDeposits at " << m_mcRichSummedDepositsLocation << endmsg;
  }

  // Clear time sample cache
  tscache.clear();
  tscache.reserve( m_summedDeposits->size() );

  // Run analog sim
  const StatusCode sc = Analog();
  if ( sc.isFailure() ) { return sc; }

  // run digital sim and return
  return Digital();
}

StatusCode DetailedFrontEndResponse::Analog()
{
    if ( msgLevel(MSG::DEBUG) ) debug() << "Analogue Simulation" << endmsg;

  for ( LHCb::MCRichSummedDeposits::const_iterator iSumDep = m_summedDeposits->begin();
        iSumDep != m_summedDeposits->end(); ++iSumDep )
  {

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Summed Deposit " << (*iSumDep)->key() << endmsg;
    }

    RichPixelProperties* props = actual_base->DecodeUniqueID( (*iSumDep)->key() );
    if ( !props   )
    {
      //std::ostringstream mess;
      //mess << "ID " << (*iSumDep)->key() << " has no RichPixelProperties";
      //Warning( mess.str(), StatusCode::FAILURE, 0 );
      continue;
    }

    const RichPixelReadout * readOut = props->Readout();
    if ( !readOut )
    {
      std::ostringstream mess;
      mess << "ID " << (*iSumDep)->key() << " has no RichPixelReadout";
      Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      continue;
    }

    const RichShape* shape = readOut->Shape();
    if ( shape )
    {

      // Create time sample for this summed deposit
      tscache.push_back( TimeData( *iSumDep,
                                   RichTimeSample( readOut->FrameSize(),
                                                   readOut->BaseLine() ) ) );
      RichTimeSample & ts = tscache.back().second;

      // Retrieve vector of SmartRefs to contributing deposits (non-const)
      const SmartRefVector<LHCb::MCRichDeposit> & deposits = (*iSumDep)->deposits();

      for ( SmartRefVector<LHCb::MCRichDeposit>::const_iterator iDep
              = deposits.begin(); iDep != deposits.end(); ++iDep )
      {

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " Deposit " << (*iDep)->key()
                    << " from '" << objectLocation( (*iDep)->parentHit()->parent() ) << "'" << endmsg
                    << "  -> TOF     = " << (*iDep)->time() << endmsg
                    << "  -> Energy  = " << (*iDep)->energy() << endmsg;
        }

        const Rich::DetectorType rich = (*iDep)->parentHit()->rich();

        // Course cut on deposit TOF ( -50ns to 50ns )
        if ( fabs((*iDep)->time()) < 50 )
        {

          // Bin zero
          const int binZero = (int)(*iDep)->time();

          // origin time
          double binTime = m_timeShift[rich] - binZero;

          // dead region
          const bool dead = ( binZero < 0 && binZero > -50 );

          // electrons
          const double e  =
            ( dead ? 0 : ((*iDep)->energy()*m_Calibration) + m_gaussNoise()/el_per_adc );

          // Loop over time sample and fill for this deposit
          for ( unsigned int bin = 0; bin < ts.size(); ++bin )
          {
            binTime += 25. / readOut->FrameSize();
            ts[bin] += ( dead ? -999999 : (*shape)[binTime] * e );
          }

        }

      } // MCRichDeposit loop

    } // if shape
    else
    {
      Warning( "Summed deposit has no associated RichShape" ).ignore();
    }

  }

  return StatusCode::SUCCESS;
}

StatusCode DetailedFrontEndResponse::Digital()
{
    if ( msgLevel(MSG::DEBUG) ) debug() << "Digital Simulation" << endmsg;

  // new RichDigit container to Gaudi data store
  LHCb::MCRichDigits * mcRichDigits = new LHCb::MCRichDigits();
  put( mcRichDigits, m_mcRichDigitsLocation );

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

        LHCb::MCRichDigit* newDigit = new LHCb::MCRichDigit();
        mcRichDigits->insert( newDigit, ((*tsc_it).first)->key().pixelID() );

        // Create MCRichHit links
        LHCb::MCRichDigitHit::Vector hitVect;
        const SmartRefVector<LHCb::MCRichDeposit> & deps = ((*tsc_it).first)->deposits();
        for ( SmartRefVector<LHCb::MCRichDeposit>::const_iterator iDep = deps.begin();
              iDep != deps.end(); ++iDep )
        {
          hitVect.push_back( LHCb::MCRichDigitHit( *((*iDep)->parentHit()), (*iDep)->history() ) );
        }
        newDigit->setHits( hitVect );

        // Store overall history info
        newDigit->setHistory( ((*tsc_it).first)->history() );

      }

    } // readout exists

  } // loop over time samples

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Registered " << mcRichDigits->size()
            << " MCRichDigits at " << m_mcRichDigitsLocation << endmsg;
  }

  return StatusCode::SUCCESS;
}
