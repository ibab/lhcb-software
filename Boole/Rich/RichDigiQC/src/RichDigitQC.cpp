
//------------------------------------------------------------------------------------
/** @file RichDigitQC.cpp
 *
 *  Implementation file for RICH Digitisation Quality Control algorithm : RichDigitQC
 *
 *  CVS Log :-
 *  $Id: RichDigitQC.cpp,v 1.26 2006-01-23 15:50:56 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

// local
#include "RichDigitQC.h"

//------------------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigitQC>          s_factory ;
const        IAlgFactory& RichDigitQCFactory = s_factory ;

// Standard constructor, initializes variables
RichDigitQC::RichDigitQC( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : RichHistoAlgBase ( name, pSvcLocator ),
    m_detNumTool     ( 0                 ),
    m_smartIDs       ( 0                 ),
    m_mcTool         ( 0                 ),
    m_evtC           ( 0                 ),
    m_spillDigits    ( Rich::NRiches     ),
    m_totalSpills    ( Rich::NRiches     ),
    m_bkgHits        ( Rich::NRiches, 0  )
{

  // Declare job options
  declareProperty( "InputDigits", m_digitTDS = MCRichDigitLocation::Default );
  declareProperty( "ExtraHistos", m_extraHists = false );

}

// Destructor
RichDigitQC::~RichDigitQC() {}

// Initialisation
StatusCode RichDigitQC::initialize()
{
  // Initialize base class
  const StatusCode sc = RichHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichDetNumberingTool", m_detNumTool, 0, true );
  acquireTool( "RichSmartIDTool" ,     m_smartIDs,   0, true );
  acquireTool( "RichMCTruthTool",      m_mcTool,     0, true );

  // Initialise variables
  m_evtC = 0;

  // Warn if extra histos are enabled
  if ( m_extraHists ) Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  return sc;
}

// Main execution
StatusCode RichDigitQC::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endreq;

  // Locate MCRichDigits
  MCRichDigits * richDigits = get<MCRichDigits>( m_digitTDS );

  // temporary tally
  HPDCounter nHPD[Rich::NRiches];

  // Loop over all digits
  std::vector<unsigned int> backs(Rich::NRiches);
  SpillDetCount spills(Rich::NRiches);
  for ( MCRichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end(); ++iDigit )
  {    
    const MCRichDigit * mcDig = *iDigit;

    // Get Rich ID
    const Rich::DetectorType rich = mcDig->key().rich();

    // count in each HPD for this event
    ++(nHPD[rich])[mcDig->key().hpdID()];

    // Location of parent MCHit
    const std::string location = mchitLocation( mcDig );
    m_evtLocs[location] = true;

    // Count hits
    ++(m_spillDigits[rich])[location];
    ++(spills[rich])[location];

    // Check if digit is background
    if ( m_mcTool->isBackground(mcDig) ) { ++m_bkgHits[rich]; ++backs[rich]; }

  }

  // Get total number of hits in each event
  //------------------------------------------------------------------------------
  for ( EventLocations::const_iterator iC = m_evtLocs.begin();
        iC != m_evtLocs.end(); ++iC )
  {
    if ( exist<MCRichHits>(iC->first) )
    {
      MCRichHits * hits = get<MCRichHits>( iC->first );
      for ( MCRichHits::const_iterator iH = hits->begin(); iH != hits->end(); ++iH )
      {
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
  for ( int iRich = 0; iRich<Rich::NRiches; ++iRich )
  {
    const Rich::DetectorType rich = (Rich::DetectorType)iRich;
    const std::string RICH = Rich::text(rich);

    unsigned int totDet(0);
    L1Counter totL1;
    for ( HPDCounter::const_iterator iHPD = nHPD[rich].begin();
          iHPD != nHPD[rich].end(); ++iHPD )
    {
      plot1D( (*iHPD).second, RICH+" : Average HPD occupancy (nHits>0)", 0, 150, 75 );
      if ( m_extraHists )
      {
        const RichDAQ::HPDHardwareID hID ( m_detNumTool->hardwareID( (*iHPD).first ) );
        std::ostringstream title;
        title << RICH << " : HPD " << (*iHPD).first << " " << hID << " occupancy (nHits>0)";
        plot1D( (*iHPD).second, hID.data(), title.str(), 0, 150, 75 );
      }
      (m_nHPD[rich])[(*iHPD).first] += (*iHPD).second;
      const RichDAQ::Level1ID l1ID = m_detNumTool->level1ID( (*iHPD).first );
      totL1[l1ID] += (*iHPD).second;
      totDet      += (*iHPD).second;
    }
    plot1D( totDet, RICH+" : Detector occupancy", 0, 5000, 50 );
    if ( m_extraHists )
    {
      plot1D( backs[rich], RICH+" : # Background hits", 0, 5000, 50 );
      for ( SpillCount::iterator iC = spills[rich].begin(); iC != spills[rich].end(); ++iC )
      {
        plot1D( iC->second, RICH+" : # Spillover hits "+iC->first,  0, 5000, 50 );
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
StatusCode RichDigitQC::finalize()
{

  // Statistical calculators
  RichStatDivFunctor    occ("%8.2f +-%5.2f");
  RichPoissonEffFunctor eff("%6.2f +-%5.2f");

  info() << "===============================================================================================" << endreq
         << "                            RICH Digitisation Simulation Summary" << endreq;

  for ( int iRich = 0; iRich<Rich::NRiches; ++iRich )
  {
    const Rich::DetectorType rich = (Rich::DetectorType)iRich;
    const std::string RICH = Rich::text(rich);
    info() << "-----------------------------------------------------------------------------------------------" << endreq;

    // Form final numbers
    L1Counter totL1;
    unsigned int totDet(0);

    debug() << " " << RICH << " : Individual HPD info :-" << endreq;
    unsigned int maxOcc(0), minOcc(999999);
    RichDAQ::HPDHardwareID maxHPD(0), minHPD(0);
    for ( HPDCounter::const_iterator iHPD = m_nHPD[rich].begin();
          iHPD != m_nHPD[rich].end(); ++iHPD )
    {
      const RichDAQ::HPDHardwareID hID ( m_detNumTool->hardwareID( (*iHPD).first ) );
      const RichDAQ::Level1ID l1ID     ( m_detNumTool->level1ID( (*iHPD).first ) );
      const Gaudi::XYZPoint hpdGlo          ( m_smartIDs->hpdPosition( (*iHPD).first ) );
      const Gaudi::XYZPoint hpdLoc          ( m_smartIDs->globalToPDPanel( hpdGlo ) );
      totL1[l1ID] += (*iHPD).second;
      totDet      += (*iHPD).second;
      if ( (*iHPD).second > maxOcc ) { maxOcc = (*iHPD).second; maxHPD = hID; }
      if ( (*iHPD).second < minOcc ) { minOcc = (*iHPD).second; minHPD = hID; }
      if ( m_extraHists )
      {
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : HPD hardware ID layout", -800, 800, -600, 600, 100, 100, hID.data() );
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : Level1 ID layout", -800, 800, -600, 600, 100, 100, l1ID.data() );
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : SmartID Row layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.hpdNumInCol() );
        plot2D( hpdLoc.x(), hpdLoc.y(), RICH+" : SmartID Col layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.hpdCol() );
      }
      debug() << "    HPD " << (*iHPD).first << " hardID "
              << format("%3i",hID.data()) << " : L1 board" << format("%3i",l1ID.data()) << endreq
              << "      Global position : " << hpdGlo << endreq
              << "      Local position  : " << hpdLoc << endreq
              << "      Hit occupancy   : " << occ((*iHPD).second,m_evtC) << " hits/event" << endreq;
    }

    info() << " " << RICH << " : Av. overall hit occupancy    " << occ(totDet,m_evtC) << " hits/event" << endreq
           << "       : Av. HPD hit occupancy        "
           << occ(totDet,m_evtC*m_nHPD[rich].size()) << " hits/event" << endreq;
    {for ( SpillCount::iterator iC = m_spillDigits[rich].begin(); iC != m_spillDigits[rich].end(); ++iC )
    {
      std::string loc = iC->first;
      loc.resize(28,' ');
      info() << "       :   " << loc << " " << eff(iC->second,totDet) << " % of total, "
             << eff(iC->second,(m_totalSpills[rich])[iC->first]) << " % event eff." << endreq;
    }}
    info() << "       : % background hits              "
           << eff(m_bkgHits[rich],totDet) << " % " << endreq;

    int iC = 0;
    for ( L1Counter::const_iterator iL1 = totL1.begin(); iL1 != totL1.end(); ++iL1, ++iC )
    {
      debug() << "       : Av. L1 board" << format("%3i",(*iL1).first.data())
              << " hit occupancy   = " << occ((*iL1).second,m_evtC) << endreq;
    }

    info() << "       : Min Av. HPD occupancy hID=" << format("%3i",minHPD.data()) 
           << occ(minOcc,m_evtC) << " hits/event" << endreq
           << "       : Max Av. HPD occupancy hID=" << format("%3i",maxHPD.data()) 
           << occ(maxOcc,m_evtC) << " hits/event" << endreq;

  }

  info() << "===============================================================================================" << endreq;

  // finalize base class
  return RichHistoAlgBase::finalize();
}

