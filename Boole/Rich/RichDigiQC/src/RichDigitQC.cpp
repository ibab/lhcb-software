
//------------------------------------------------------------------------------------
/** @file RichDigitQC.cpp
 *
 *  Implementation file for RICH Digitisation Quality Control algorithm : RichDigitQC
 *
 *  CVS Log :-
 *  $Id: RichDigitQC.cpp,v 1.14 2005-03-05 12:26:29 jonrob Exp $
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
  : RichMoniAlgBase ( name, pSvcLocator ),
    m_level1        ( 0                 ),
    m_hpdID         ( 0                 ),
    m_smartIDs      ( 0                 ),
    m_mcTool        ( 0                 ),
    m_spillDigits   ( Rich::NRiches     ),
    m_totalSpills   ( Rich::NRiches     ),
    m_bkgHits       ( Rich::NRiches, 0  )
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
  const StatusCode sc = RichMoniAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichHPDToLevel1Tool", m_level1   );
  acquireTool( "RichHPDIDTool",       m_hpdID    );
  acquireTool( "RichSmartIDTool" ,    m_smartIDs );
  acquireTool( "RichMCTruthTool",     m_mcTool   );

  // Initialise variables
  m_evtC = 0;

  //m_evtLocs.push_back( "/Event/MC/Rich/Hits" );
  //m_evtLocs.push_back( "/Event/PrevPrev/MC/Rich/Hits" );
  //m_evtLocs.push_back( "/Event/Prev/MC/Rich/Hits" );
  //m_evtLocs.push_back( "/Event/Next/MC/Rich/Hits" );
  //m_evtLocs.push_back( "/Event/NextNext/MC/Rich/Hits" );

  // Warn if extra histos are enabled
  if ( m_extraHists ) Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  return sc;
}

// Main execution
StatusCode RichDigitQC::execute()
{
  debug() << "Execute" << endreq;

  // Locate MCRichDigits
  MCRichDigits * richDigits = get<MCRichDigits>( m_digitTDS );

  // temporary talliy
  HPDCounter nHPD[Rich::NRiches];

  // Loop over all digits
  std::vector<unsigned int> backs(Rich::NRiches);
  SpillDetCount spills(Rich::NRiches);
  for ( MCRichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end(); ++iDigit ) {

    // Get Rich ID
    const Rich::DetectorType rich = (*iDigit)->key().rich();

    // count in each HPD for this event
    ++(nHPD[rich])[(*iDigit)->key().pdID()];

    // Location of parent MCHit
    const std::string location = mchitLocation( *iDigit );
    m_evtLocs[location] = true;

    // Count hits
    ++(m_spillDigits[rich])[location];
    ++(spills[rich])[location];

    // Check if digit is background
    if ( m_mcTool->isBackground(*iDigit) ) { ++m_bkgHits[rich]; ++backs[rich]; }

  }

  // Get total number of hits in each event
  //------------------------------------------------------------------------------
  for ( RichHashMap<std::string,bool>::const_iterator iC = m_evtLocs.begin(); 
        iC != m_evtLocs.end(); ++iC )
  {
    if ( exist<MCRichHits>(iC->first) )
    {
      MCRichHits * hits = get<MCRichHits>( iC->first );
      for ( MCRichHits::const_iterator iH = hits->begin(); iH != hits->end(); ++iH )
      {
        ++(m_totalSpills[(*iH)->rich()])[iC->first];
      }
      debug() << "Found " << hits->size() << " MCRichHits at " << iC->first << endreq;
    }
  }

  // count events
  ++m_evtC;

  // Various tallies and plots
  //------------------------------------------------------------------------------
  std::vector< unsigned int > totDet(Rich::NRiches,0);
  L1Counter totL1R1, totL1R2;
  {for ( HPDCounter::const_iterator iHPD = nHPD[Rich::Rich1].begin();
         iHPD != nHPD[Rich::Rich1].end(); ++iHPD )
  {
    plot1D( (*iHPD).second, "RICH1 : HPD occupancy", 0, 150, 75 );
    (m_nHPD[Rich::Rich1])[(*iHPD).first] += (*iHPD).second;
    const RichDAQ::Level1ID l1ID = m_level1->levelL1ID( (*iHPD).first );
    totL1R1[l1ID]       += (*iHPD).second;
    totDet[Rich::Rich1] += (*iHPD).second;
  }}
  {for ( HPDCounter::const_iterator iHPD = nHPD[Rich::Rich2].begin();
         iHPD != nHPD[Rich::Rich2].end(); ++iHPD )
  {
    plot1D( (*iHPD).second, "RICH2 : HPD occupancy", 0, 150, 75 );
    (m_nHPD[Rich::Rich2])[(*iHPD).first] += (*iHPD).second;
    const RichDAQ::Level1ID l1ID = m_level1->levelL1ID( (*iHPD).first );
    totL1R2[l1ID]       += (*iHPD).second;
    totDet[Rich::Rich2] += (*iHPD).second;
  }}

  plot1D( totDet[Rich::Rich1], "RICH1 : Detector occupancy", 0, 5000, 50 );
  plot1D( totDet[Rich::Rich2], "RICH2 : Detector occupancy", 0, 2000, 50 );
  if ( m_extraHists )
  {
    plot1D( backs[Rich::Rich1], "RICH1 : # Background hits", 0, 5000, 50 );
    plot1D( backs[Rich::Rich2], "RICH2 : # Background hits", 0, 2000, 50 );
    for ( SpillCount::iterator iC = spills[Rich::Rich1].begin(); iC != spills[Rich::Rich1].end(); ++iC )
    {
      plot1D( iC->second,  "RICH1 : # Spillover hits "+iC->first,  0, 5000, 50 );
    }

    for ( SpillCount::iterator iC = spills[Rich::Rich2].begin(); iC != spills[Rich::Rich2].end(); ++iC )
    {
      plot1D( iC->second,  "RICH2 : # Spillover hits "+iC->first,  0, 2000, 50 );
    }
  }

  {for ( L1Counter::const_iterator iL1 = totL1R1.begin(); iL1 != totL1R1.end(); ++iL1 ) {
    std::string l1String =  boost::lexical_cast<std::string>((*iL1).first);
    plot1D( (*iL1).second, "RICH1 : Average L1 board occupancy", 0, 1000 );
    if ( m_extraHists )
    {
      plot1D( (*iL1).second, "RICH1 : L1 board "+l1String+" occupancy", 0, 1000 );
    }
  }}
  {for ( L1Counter::const_iterator iL1 = totL1R2.begin(); iL1 != totL1R2.end(); ++iL1 ) {
    std::string l1String =  boost::lexical_cast<std::string>((*iL1).first);
    plot1D( (*iL1).second, "RICH2 : Average L1 board occupancy", 0, 1000 );
    if ( m_extraHists )
    {
      plot1D( (*iL1).second, "RICH2 : L1 board "+l1String+" occupancy", 0, 1000 );
    }
  }}

  // Event printout
  //------------------------------------------------------------------------------
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RICH1 : Total # digits = " << totDet[Rich::Rich1] << endreq;
    for ( SpillCount::const_iterator iC = spills[Rich::Rich1].begin();
          iC != spills[Rich::Rich1].end(); ++iC )
    {
      debug() <<  "      : " << iC->first << " " << iC->second << endreq;
    }
    debug() << "      : # background " << backs[Rich::Rich1] << endreq;
    debug() << "RICH2 : Total # digits = " << totDet[Rich::Rich2] << endreq;
    for ( SpillCount::const_iterator iC = spills[Rich::Rich2].begin();
          iC != spills[Rich::Rich2].end(); ++iC )
    {
      debug() <<  "      : " << iC->first << " " << iC->second << endreq;
    }
    debug() << "      : # background " << backs[Rich::Rich2] << endreq;
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichDigitQC::finalize()
{

  // Statistical calculators
  RichStatDivFunctor occ("%8.2f +-%5.2f");
  RichPoissonEffFunctor eff("%5.2f +-%5.2f");

  info() << "=============================================================================================" << endreq
         << "                           RICH Digitisation Simuation Summary" << endreq
         << "---------------------------------------------------------------------------------------------" << endreq;

  // Form final numbers
  L1Counter totL1R1, totL1R2;
  std::vector< unsigned int > totDet(Rich::NRiches,0);

  // RICH1 ----------------------------------------------------------------------------------
  {
    verbose() << " RICH1 : Individual HPD info :-" << endreq;
    for ( HPDCounter::const_iterator iHPD = m_nHPD[Rich::Rich1].begin();
          iHPD != m_nHPD[Rich::Rich1].end(); ++iHPD )
    {
      const RichDAQ::HPDHardwareID hID = m_hpdID->hardwareID( (*iHPD).first );
      const RichDAQ::Level1ID l1ID     = m_level1->levelL1ID( (*iHPD).first );
      const HepPoint3D hpdGlo = m_smartIDs->hpdPosition( (*iHPD).first );
      const HepPoint3D hpdLoc = m_smartIDs->globalToPDPanel( hpdGlo );
      totL1R1[l1ID]       += (*iHPD).second;
      totDet[Rich::Rich1] += (*iHPD).second;
      if ( m_extraHists )
      {
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH1 : HPD hardware ID layout", -800, 800, -600, 600, 100, 100, hID );
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH1 : Level1 ID layout", -800, 800, -600, 600, 100, 100, l1ID );
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH1 : SmartID Row layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.pdRow() );
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH1 : SmartID Col layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.pdCol() );
      }
      verbose() << "    HPD " << (*iHPD).first << " hardID "
                << format("%3i",hID) << " : L1 board" << format("%3i",l1ID) << endreq
                << "      Global position : " << hpdGlo << endreq
                << "      Local position  : " << hpdLoc << endreq
                << "      Hit occupancy   : " << occ((*iHPD).second,m_evtC) << " hits/event" << endreq;
    }

    info() << " RICH1 : Av. overall hit occupancy   " << occ(totDet[Rich::Rich1],m_evtC) << " hits/event" << endreq
           << "       : Av. HPD hit occupancy       "
           << occ(totDet[Rich::Rich1],m_evtC*m_nHPD[Rich::Rich1].size()) << " hits/event" << endreq;
    for ( SpillCount::iterator iC = m_spillDigits[Rich::Rich1].begin(); iC != m_spillDigits[Rich::Rich1].end(); ++iC )
    {
      std::string loc = iC->first;
      loc.resize(28,' ');
      info() << "       :   " << loc << " " << eff(iC->second,totDet[Rich::Rich1]) << " % of total, "
             << eff(iC->second,(m_totalSpills[Rich::Rich1])[iC->first]) << " % event eff." << endreq;
    }
    info() << "       : % background hits              "
           << eff(m_bkgHits[Rich::Rich1],totDet[Rich::Rich1]) << " % " << endreq;

    int iC = 0;
    for ( L1Counter::const_iterator iL1 = totL1R1.begin(); iL1 != totL1R1.end(); ++iL1, ++iC )
    {
      debug() << "       : Av. L1 board" << format("%3i",(*iL1).first)
              << " hit occupancy   = " << occ((*iL1).second,m_evtC) << endreq;
    }

  }

  info() << "---------------------------------------------------------------------------------------------" << endreq;

  // RICH2 ----------------------------------------------------------------------------------
  {
    verbose() << " RICH2 : Individual HPD info :-" << endreq;
    for ( HPDCounter::const_iterator iHPD = m_nHPD[Rich::Rich2].begin();
          iHPD != m_nHPD[Rich::Rich2].end(); ++iHPD )
    {
      const RichDAQ::HPDHardwareID hID = m_hpdID->hardwareID( (*iHPD).first );
      const RichDAQ::Level1ID l1ID     = m_level1->levelL1ID( (*iHPD).first );
      const HepPoint3D hpdGlo = m_smartIDs->hpdPosition( (*iHPD).first );
      const HepPoint3D hpdLoc = m_smartIDs->globalToPDPanel( hpdGlo );
      totL1R2[l1ID]       += (*iHPD).second;
      totDet[Rich::Rich2] += (*iHPD).second;
      if ( m_extraHists )
      {
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH2 : HPD hardware ID layout", -800, 800, -800, 800, 100, 100, hID );
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH2 : Level1 ID layout", -800, 800, -800, 800, 100, 100, l1ID );
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH2 : SmartID Row layout", -800, 800, -800, 800, 100, 100, (*iHPD).first.pdRow() );
        plot2D( hpdLoc.x(), hpdLoc.y(), "RICH2 : SmartID Col layout", -800, 800, -800, 800, 100, 100, (*iHPD).first.pdCol() );
      }
      verbose() << "    HPD " << (*iHPD).first << " hardID "
                << format("%3i",hID) << " : L1 board" << format("%3i",l1ID) << endreq
                << "      Global position : " << hpdGlo << endreq
                << "      Local position  : " << hpdLoc << endreq
                << "      Hit occupancy   : " << occ((*iHPD).second,m_evtC) << " hits/event" << endreq;
    }

    info() << " RICH2 : Av. overall hit occupancy   " << occ(totDet[Rich::Rich2],m_evtC) << " hits/event" << endreq
           << "       : Av. HPD hit occupancy       "
           << occ(totDet[Rich::Rich2],m_evtC*m_nHPD[Rich::Rich2].size()) << " hits/event" << endreq;
    for ( SpillCount::iterator iC = m_spillDigits[Rich::Rich2].begin(); iC != m_spillDigits[Rich::Rich2].end(); ++iC )
    {
      std::string loc = iC->first;
      loc.resize(28,' ');
      info() << "       :   " << loc << " " << eff(iC->second,totDet[Rich::Rich2]) << " % of total, "
             << eff(iC->second,(m_totalSpills[Rich::Rich2])[iC->first]) << " % event eff." << endreq;
    }
    info() << "       : % background hits              "
           << eff(m_bkgHits[Rich::Rich2],totDet[Rich::Rich2]) << " % " << endreq;

    int iC = 0;
    for ( L1Counter::const_iterator iL1 = totL1R2.begin(); iL1 != totL1R2.end(); ++iL1, ++iC )
    {
      debug() << "       : Av. L1 board" << format("%3i",(*iL1).first)
              << " hit occupancy   = " << occ((*iL1).second,m_evtC) << endreq;
    }

  }

  info() << "=============================================================================================" << endreq;

  // finalize base class
  return RichMoniAlgBase::finalize();
}

