
//------------------------------------------------------------------------------------
/** @file RichDigitQC.cpp
 *
 *  Implementation file for RICH Digitisation Quality Control algorithm : RichDigitQC
 *
 *  CVS Log :-
 *  $Id: RichDigitQC.cpp,v 1.8 2005-01-17 14:08:10 cattanem Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2005/01/13 14:10:53  jonrob
 *  fix un-initialized variables
 *
 *  Revision 1.6  2005/01/13 13:04:05  jonrob
 *  Update monitoring information
 *
 *  Revision 1.5  2005/01/07 12:38:09  jonrob
 *  Updates for new RichDAQ package
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
    m_smartIDs      ( 0                 )
{

  // Declare job options
  declareProperty( "InputDigits", m_digitTDS = MCRichDigitLocation::Default );

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

  // Initialise variables
  m_evtC = 0;

  return sc;
};

// Main execution
StatusCode RichDigitQC::execute()
{
  debug() << "Execute" << endreq;

  // Locate MCRichDigits
  MCRichDigits * richDigits = get<MCRichDigits>( m_digitTDS );

  // temporary talliy
  HPDCounter nHPD[Rich::NRiches];

  // Loop over all digits
  for ( MCRichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end(); ++iDigit ) {

    // Get Rich ID
    const Rich::DetectorType rich = (*iDigit)->key().rich();

    // count in each HPD for this event
    ++(nHPD[rich])[(*iDigit)->key().pdID()];

  }

  // count events
  ++m_evtC;

  // Various tallies and plots
  std::vector< unsigned int > totDet(Rich::NRiches,0);
  L1Counter totL1R1, totL1R2;
  {for ( HPDCounter::const_iterator iHPD = nHPD[Rich::Rich1].begin();
         iHPD != nHPD[Rich::Rich1].end(); ++iHPD )
  {
    plot( (*iHPD).second, "RICH1 : HPD occupancy", 0, 150 );
    (m_nHPD[Rich::Rich1])[(*iHPD).first] += (*iHPD).second;
    const RichDAQ::Level1ID l1ID = m_level1->levelL1ID( (*iHPD).first );
    totL1R1[l1ID]       += (*iHPD).second;
    totDet[Rich::Rich1] += (*iHPD).second;
  }}
  {for ( HPDCounter::const_iterator iHPD = nHPD[Rich::Rich2].begin();
         iHPD != nHPD[Rich::Rich2].end(); ++iHPD )
  {
    plot( (*iHPD).second, "RICH2 : HPD occupancy", 0, 150 );
    (m_nHPD[Rich::Rich2])[(*iHPD).first] += (*iHPD).second;
    const RichDAQ::Level1ID l1ID = m_level1->levelL1ID( (*iHPD).first );
    totL1R2[l1ID]       += (*iHPD).second;
    totDet[Rich::Rich2] += (*iHPD).second;
  }}
  plot( totDet[Rich::Rich1], "RICH1 : Detector occupancy", 0, 5000 );
  plot( totDet[Rich::Rich2], "RICH2 : Detector occupancy", 0, 2000 );
  {for ( L1Counter::const_iterator iL1 = totL1R1.begin(); iL1 != totL1R1.end(); ++iL1 ) {
    std::string l1String =  boost::lexical_cast<std::string>((*iL1).first);
    plot( (*iL1).second, "RICH1 : Average L1 board occupancy", 0, 1000 );
    plot( (*iL1).second, "RICH1 : L1 board "+l1String+" occupancy", 0, 1000 );
  }}
  {for ( L1Counter::const_iterator iL1 = totL1R2.begin(); iL1 != totL1R2.end(); ++iL1 ) {
    std::string l1String =  boost::lexical_cast<std::string>((*iL1).first);
    plot( (*iL1).second, "RICH2 : Average L1 board occupancy", 0, 1000 );
    plot( (*iL1).second, "RICH2 : L1 board "+l1String+" occupancy", 0, 1000 );
  }}

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichDigitQC::finalize()
{

  // Statistical calculator
  RichStatDivFunctor occ;

  info() << "================================================================================" << endreq
         << "                  RICH Digitisation and DAQ Simuation Summary" << endreq
         << "--------------------------------------------------------------------------------" << endreq;

  // Form final numbers
  L1Counter totL1R1, totL1R2;
  std::vector< unsigned int > totDet(Rich::NRiches,0);
  debug() << "   RICH1 : Individual HPD info :-" << endreq;
  {for ( HPDCounter::const_iterator iHPD = m_nHPD[Rich::Rich1].begin();
         iHPD != m_nHPD[Rich::Rich1].end(); ++iHPD )
  {
    const RichDAQ::HPDHardwareID hID = m_hpdID->hardwareID( (*iHPD).first );
    const RichDAQ::Level1ID l1ID     = m_level1->levelL1ID( (*iHPD).first );
    const HepPoint3D hpdGlo = m_smartIDs->hpdPosition( (*iHPD).first );
    const HepPoint3D hpdLoc = m_smartIDs->globalToPDPanel( hpdGlo );
    totL1R1[l1ID]       += (*iHPD).second;
    totDet[Rich::Rich1] += (*iHPD).second;
    // comment out "D plots until released GaudiHistoAlg supports them
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH1 : HPD hardware ID layout", -800, 800, -600, 600, 100, 100, hID );
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH1 : Level1 ID layout", -800, 800, -600, 600, 100, 100, l1ID );
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH1 : SmartID Row layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.PDRow() );
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH1 : SmartID Col layout", -800, 800, -600, 600, 100, 100, (*iHPD).first.PDCol() );
    debug() << "      HPD " << (*iHPD).first << " hardID "
            << format("%3i",hID) << " : L1 board" << format("%3i",l1ID) << endreq
            << "        Global position : " << hpdGlo << endreq
            << "        Local position  : " << hpdLoc << endreq
            << "        Hit occupancy   : " << occ((*iHPD).second,m_evtC) << " hits/event" << endreq;
  }}

  info() << "   RICH1 : Av. overall hit occupancy       = " << occ(totDet[Rich::Rich1],m_evtC) << endreq
         << "   RICH1 : Av. HPD hit occupancy           = " << occ(totDet[Rich::Rich1],m_evtC*m_nHPD[Rich::Rich1].size())
         << endreq;

  {int iC = 0;
  for ( L1Counter::const_iterator iL1 = totL1R1.begin(); iL1 != totL1R1.end(); ++iL1, ++iC )
  {
    debug() << "   RICH1 : Av. L1 board" << format("%3i",(*iL1).first)
            << " hit occupancy   = " << occ((*iL1).second,m_evtC) << endreq;
  }}

  debug() << "   RICH2 : Individual HPD info :-" << endreq;
  {for ( HPDCounter::const_iterator iHPD = m_nHPD[Rich::Rich2].begin();
         iHPD != m_nHPD[Rich::Rich2].end(); ++iHPD )
  {
    const RichDAQ::HPDHardwareID hID = m_hpdID->hardwareID( (*iHPD).first );
    const RichDAQ::Level1ID l1ID     = m_level1->levelL1ID( (*iHPD).first );
    const HepPoint3D hpdGlo = m_smartIDs->hpdPosition( (*iHPD).first );
    const HepPoint3D hpdLoc = m_smartIDs->globalToPDPanel( hpdGlo );
    totL1R2[l1ID]       += (*iHPD).second;
    totDet[Rich::Rich2] += (*iHPD).second;
    // comment out "D plots until released GaudiHistoAlg supports them
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH2 : HPD hardware ID layout", -800, 800, -800, 800, 100, 100, hID );
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH2 : Level1 ID layout", -800, 800, -800, 800, 100, 100, l1ID );
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH2 : SmartID Row layout", -800, 800, -800, 800, 100, 100, (*iHPD).first.PDRow() );
    //plot( hpdLoc.x(), hpdLoc.y(), "RICH2 : SmartID Col layout", -800, 800, -800, 800, 100, 100, (*iHPD).first.PDCol() );
    debug() << "      HPD " << (*iHPD).first << " hardID "
            << format("%3i",hID) << " : L1 board" << format("%3i",l1ID) << endreq
            << "        Global position : " << hpdGlo << endreq
            << "        Local position  : " << hpdLoc << endreq
            << "        Hit occupancy   : " << occ((*iHPD).second,m_evtC) << " hits/event" << endreq;
  }}

  info() << "   RICH2 : Av. overall hit occupancy       = " << occ(totDet[Rich::Rich2],m_evtC) << endreq
         << "   RICH2 : Av. HPD hit occupancy           = " << occ(totDet[Rich::Rich2],m_evtC*m_nHPD[Rich::Rich2].size())
         << endreq;

  {int iC = 0;
  for ( L1Counter::const_iterator iL1 = totL1R2.begin(); iL1 != totL1R2.end(); ++iL1, ++iC )
  {
    debug() << "   RICH2 : Av. L1 board" << format("%3i",(*iL1).first)
            << " hit occupancy   = " << occ((*iL1).second,m_evtC) << endreq;
  }}

  info() << "================================================================================" << endreq;

  // finalize base class
  return RichMoniAlgBase::finalize();
}

