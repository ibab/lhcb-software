
//-------------------------------------------------------------------------------
/** @file RichDigitQC.cpp
 *
 *  Implementation file for RICH Digitisation Quality Control algorithm : RichDigitQC
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//-------------------------------------------------------------------------------

// local
#include "RichDigitQC.h"

//-------------------------------------------------------------------------------

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( DigitQC )

// Standard constructor, initializes variables
DigitQC::DigitQC( const std::string& name,
                  ISvcLocator* pSvcLocator )
  : Rich::HistoAlgBase ( name, pSvcLocator ),
    m_richSys        ( 0                 ),
    m_smartIDs       ( 0                 ),
    m_mcTool         ( 0                 ),
    m_evtC           ( 0                 )
{
  setProperty("HistoDir","DIGI/DIGITQC");
  setProperty("HistoOffSet",10000);
  // Declare job options
  declareProperty( "InputDigits", m_digitTDS = LHCb::MCRichDigitLocation::Default );
  declareProperty( "ExtraHistos", m_extraHists = false );
}

// Destructor
DigitQC::~DigitQC() {}

// Initialisation
StatusCode DigitQC::initialize()
{
  // Initialize base class
  const StatusCode sc = Rich::HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichSmartIDTool" ,     m_smartIDs,   NULL, true );
  acquireTool( "RichMCTruthTool",      m_mcTool,     NULL, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // Warn if extra histos are enabled
  if ( m_extraHists ) Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  return sc;
}

// Main execution
StatusCode DigitQC::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endreq;

  // Locate MCRichDigits
  LHCb::MCRichDigits * richDigits = get<LHCb::MCRichDigits>( m_digitTDS );

  // temporary tally
  HPDCounter nHPDSignal[Rich::NRiches];

  // Loop over all digits
  Rich::Map<Rich::DetectorType,unsigned int> backs;
  SpillDetCount spills;
  for ( LHCb::MCRichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end(); ++iDigit )
  {
    const LHCb::MCRichDigit * mcDig = *iDigit;

    // Get Rich ID
    const Rich::DetectorType rich = mcDig->key().rich();

    // Location of parent MCHit
    const std::string location = mchitLocation( mcDig );
    m_evtLocs[location] = true;

    // count all hits
    ++m_allDigits[rich];
    ++(m_spillDigits[rich])[location];

    // Check if digit is background
    if ( mcDig->history().isBackground() )
    {
      //  count background hits
      ++backs[rich];
      ++m_bkgHits[rich];
      // tally up the different background sources
      if ( mcDig->history().scatteredHit()     ) { ++m_scattHits   [rich]; }
      if ( mcDig->history().chargedTrack()     ) { ++m_chrgTkHits  [rich]; }
      if ( mcDig->history().gasQuartzCK()      ) { ++m_gasQCK      [rich]; }
      if ( mcDig->history().hpdQuartzCK()      ) { ++m_hpdQCK      [rich]; }
      if ( mcDig->history().nitrogenCK()       ) { ++m_nitroQCK    [rich]; }
      if ( mcDig->history().aeroFilterCK()     ) { ++m_aeroFiltQCK [rich]; }
      if ( mcDig->history().chargeShareHit()   ) { ++m_chrgShrHits [rich]; }
      if ( mcDig->history().hpdReflection()    ) { ++m_hpdReflHits [rich]; }
      if ( mcDig->history().hpdSiBackscatter() ) { ++m_siBackScatt [rich]; }
    }
    else
    {
      // Count signal hits
      ++(nHPDSignal[rich])[mcDig->key().pdID()];
      ++(m_spillDigitsSignal[rich])[location];
      ++(spills[rich])[location];
    }

  }

  // Get total number of hits in each event
  //------------------------------------------------------------------------------
  for ( EventLocations::const_iterator iC = m_evtLocs.begin();
        iC != m_evtLocs.end(); ++iC )
  {
    if ( exist<LHCb::MCRichHits>(iC->first) )
    {
      LHCb::MCRichHits * hits = get<LHCb::MCRichHits>( iC->first );
      for ( LHCb::MCRichHits::const_iterator iH = hits->begin(); iH != hits->end(); ++iH )
      {
        if ( !(*iH)->isBackground() )
          ++(m_totalSpills[(*iH)->rich()])[iC->first];
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << hits->size() << " MCRichHits at " << iC->first << endreq;
      }
    }
  }

  // count events
  ++m_evtC;

  // Various tallies and plots
  //------------------------------------------------------------------------------
  for ( unsigned int iRich = 0; iRich<Rich::NRiches; ++iRich )
  {
    const Rich::DetectorType rich = (Rich::DetectorType)iRich;
    const std::string RICH = Rich::text(rich);

    unsigned int totDet(0);
    L1Counter totL1;
    for ( HPDCounter::const_iterator iHPD = nHPDSignal[rich].begin();
          iHPD != nHPDSignal[rich].end(); ++iHPD )
    {
      plot1D( (*iHPD).second, RICH+" : Average HPD occupancy (nHits>0)", 0, 150, 75 );
      if ( m_extraHists )
      {
        const Rich::DAQ::HPDHardwareID hID ( m_richSys->hardwareID( (*iHPD).first ) );
        std::ostringstream title;
        title << RICH << " : HPD " << (*iHPD).first << " " << hID << " occupancy (nHits>0)";
        plot1D( (*iHPD).second, hID.data(), title.str(), 0, 150, 75 );
      }
      (m_nHPDSignal[rich])[(*iHPD).first] += (*iHPD).second;
      const Rich::DAQ::Level1HardwareID l1ID = m_richSys->level1HardwareID( (*iHPD).first );
      totL1[l1ID] += (*iHPD).second;
      totDet      += (*iHPD).second;
    }
    plot1D( totDet, RICH+" : Detector occupancy", 0, 5000, 50 );
    if ( m_extraHists )
    {
      plot1D( backs[rich], RICH+" : # Background hits", 0, 5000, 50 );
      for ( SpillCount::iterator iC = spills[rich].begin(); iC != spills[rich].end(); ++iC )
      {
        plot1D( iC->second, RICH+" : # Spillover hits "+iC->first,  0, 5000, 100 );
      }
    }
    for ( L1Counter::const_iterator iL1 = totL1.begin(); iL1 != totL1.end(); ++iL1 )
    {
      std::string l1String = boost::lexical_cast<std::string>((*iL1).first);
      plot1D( (*iL1).second, RICH+" : Average L1 board occupancy (nHits>0)", 0, 1000 );
      if ( m_extraHists )
      {
        plot1D( (*iL1).second, RICH+" : L1 board "+l1String+" occupancy (nHits>0)", 0, 1000 );
      }
    }

    // Event printout
    //------------------------------------------------------------------------------
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << RICH+" : Total # digits = " << totDet << endreq;
      for ( SpillCount::const_iterator iC = spills[rich].begin();
            iC != spills[rich].end(); ++iC )
      {
        debug() <<  "      : " << iC->first << " " << iC->second << endreq;
      }
      debug() << "      : # background " << backs[rich] << endreq;
    }

  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode DigitQC::finalize()
{

  // Statistical calculators
  const Rich::StatDivFunctor    occ("%8.2f +-%5.2f");
  const Rich::PoissonEffFunctor eff("%6.2f +-%5.2f");

  info() << "===============================================================================================" << endreq
         << "                            RICH Digitisation Simulation Summary" << endreq;

  for ( unsigned int iRich = 0; iRich<Rich::NRiches; ++iRich )
  {
    const Rich::DetectorType rich = (Rich::DetectorType)iRich;
    const std::string RICH = Rich::text(rich);
    info() << "-----------------------------------------------------------------------------------------------" << endreq;

    // Form final numbers
    L1Counter totL1;
    unsigned int totDetSignal(0);

    debug() << " " << RICH << " : Individual HPD info :-" << endreq;
    unsigned int maxOcc(0), minOcc(999999);
    Rich::DAQ::HPDHardwareID maxHPD(0), minHPD(0);
    for ( HPDCounter::const_iterator iHPD = m_nHPDSignal[rich].begin();
          iHPD != m_nHPDSignal[rich].end(); ++iHPD )
    {
      const Rich::DAQ::HPDHardwareID    hID ( m_richSys->hardwareID( (*iHPD).first ) );
      const Rich::DAQ::Level1HardwareID l1ID( m_richSys->level1HardwareID( (*iHPD).first ) );
      Gaudi::XYZPoint hpdGlo;
      const StatusCode sc = m_smartIDs->pdPosition( (*iHPD).first, hpdGlo );
      if (sc.isFailure()) continue;
      const Gaudi::XYZPoint hpdLoc       ( m_smartIDs->globalToPDPanel( hpdGlo ) );
      totL1[l1ID]  += (*iHPD).second;
      totDetSignal += (*iHPD).second;
      // make sure that the choice of maxHPD and minHPD is independent of the iteration order
      if ( iHPD->second >= maxOcc ) { if ( iHPD->second>maxOcc || hID > maxHPD ) { maxHPD = hID; } maxOcc = iHPD->second; }
      if ( iHPD->second <= minOcc ) { if ( iHPD->second<minOcc || hID < minHPD ) { minHPD = hID; } minOcc = iHPD->second; }
      if ( m_extraHists )
      {
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : HPD hardware ID layout", -800, 800, -600, 600, 100, 100, hID.data() );
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : Level1 ID layout", -800, 800, -600, 600, 100, 100, l1ID.data() );
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : SmartID Row layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.pdNumInCol() );
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : SmartID Col layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.pdCol() );
      }
      debug() << "    HPD " << (*iHPD).first << " hardID "
              << boost::format("%3i") % hID.data()
              << " : L1 board";
      debug() << boost::format("%3i") % l1ID.data() << endreq;
      debug() << "      Global position : " << hpdGlo << endreq
              << "      Local position  : " << hpdLoc << endreq
              << "      Hit occupancy   : " << occ((*iHPD).second,m_evtC) << " hits/event" << endreq;
    }

    info() << " " << RICH
           << " : Av. total  hit occupancy     " << occ(m_allDigits[rich],m_evtC) << " hits/event" << endreq;
    info() << "       : Av. signal hit occupancy     " << occ(totDetSignal,m_evtC) << " hits/event" << endreq;

    {for ( SpillCount::iterator iC = m_spillDigits[rich].begin(); iC != m_spillDigits[rich].end(); ++iC )
    {
      std::string loc = iC->first;
      loc.resize(28,' ');
      info() << "       :   " << loc << " " << eff(iC->second,m_allDigits[rich]) << " % of total, "
             << eff((m_spillDigitsSignal[rich])[iC->first],totDetSignal) << " % signal eff." << endreq;
    }}

    info() << "       : Av. HPD hit occupancy        "
           << occ(m_allDigits[rich],m_evtC*m_nHPDSignal[rich].size()) << " hits/event" << endreq;
    info() << "       :   Min Av. HPD occ. hID=" << boost::format("%6i") % minHPD.data()
           << occ(minOcc,m_evtC) << " hits/event" << endreq;
    info() << "       :   Max Av. HPD occ. hID=" << boost::format("%6i") % maxHPD.data()
           << occ(maxOcc,m_evtC) << " hits/event" << endreq;

    info() << "       : % overall background hits      "
           << eff(m_bkgHits[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % rayleigh scattered hits    "
           << eff(m_scattHits[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % charged track on HPD hits  "
           << eff(m_chrgTkHits[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % gas quartz window CK hits  "
           << eff(m_gasQCK[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % hpd quartz window CK hits  "
           << eff(m_hpdQCK[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % nitrogen CK hits           "
           << eff(m_nitroQCK[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % aerogel filter CK hits     "
           << eff(m_aeroFiltQCK[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % silicon back-scatter hits  "
           << eff(m_siBackScatt[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % internal reflection hits   "
           << eff(m_hpdReflHits[rich],m_allDigits[rich]) << " % " << endreq
           << "       :   % silicon charge share hits  "
           << eff(m_chrgShrHits[rich],m_allDigits[rich]) << " % " << endreq;

    int iC = 0;
    for ( L1Counter::const_iterator iL1 = totL1.begin(); iL1 != totL1.end(); ++iL1, ++iC )
    {
      debug() << "       : Av. L1 board" << boost::format("%3i") % (*iL1).first.data()
              << " hit occupancy   = " << occ((*iL1).second,m_evtC) << endreq;
    }

  }

  info() << "===============================================================================================" << endreq;

  // finalize base class
  return Rich::HistoAlgBase::finalize();
}

