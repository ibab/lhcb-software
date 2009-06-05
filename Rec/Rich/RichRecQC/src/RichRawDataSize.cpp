
//-----------------------------------------------------------------------------
/** @file RichRawDataSize.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::RawDataSize
 *
 *  CVS Log :-
 *  $Id: RichRawDataSize.cpp,v 1.4 2009-06-05 16:19:08 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRawDataSize.h"

//-----------------------------------------------------------------------------

using namespace Rich::DAQ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawDataSize );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawDataSize::RawDataSize( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : Rich::HistoAlgBase ( name , pSvcLocator ),
    m_SmartIDDecoder   ( NULL  ),
    m_RichSys          ( NULL  ),
    m_taeEvents        ( 1, "" )
{
  declareProperty( "RawEventLocations", m_taeEvents   );
  declareProperty( "FillHPDPlots", m_hpdPlots = false );
  setProperty( "HistoPrint", false );
}

//=============================================================================
// Destructor
//=============================================================================
RawDataSize::~RawDataSize() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RawDataSize::initialize()
{
  const StatusCode sc = Rich::HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawDataSize::execute()
{
  // get the raw data
  const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(m_taeEvents);

  for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map )
  {
    const Rich::DAQ::Level1HardwareID l1HardID = iL1Map->first;
    const Rich::DAQ::IngressMap & ingressMap  = iL1Map->second;

    unsigned int nL1Words(Rich::DAQ::NumIngressPerL1); // Start with 1 word per HPD ingress header

    for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap )
    {
      const Rich::DAQ::IngressInfo & ingressInfo = iIngressMap->second;
      const Rich::DAQ::HPDMap & hpdMap = ingressInfo.hpdData();

      for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap )
      {
        const Rich::DAQ::HPDInfo & hpdInfo           = iHPDMap->second;
        const LHCb::RichSmartID  & hpdID             = hpdInfo.hpdID();
        const Rich::DAQ::HPDInfo::Header & hpdHeader = hpdInfo.header();
        const Rich::DAQ::HPDInfo::Footer & hpdFooter = hpdInfo.footer();

        // Only use valid data
        if ( hpdHeader.inhibit() || !hpdID.isValid() ) continue;

        // use a try block in case of DB lookup errors
        try
        {

          // Get the HPD hardware ID
          const Rich::DAQ::HPDHardwareID hpdHardID   = m_RichSys->hardwareID(hpdID);

          // number of data words for this HPD
          const unsigned int nHPDwords = ( hpdHeader.nDataWords()   +
                                           hpdHeader.nHeaderWords() +
                                           hpdFooter.nFooterWords() );
          nL1Words += nHPDwords;

          if ( m_hpdPlots )
          {
            std::ostringstream title, ID;
            title << "Data Size (# 32-bit words) : HPD Hardware ID " << hpdHardID;
            ID << "hpds/HPDHardwareID" << hpdHardID;
            plot1D( nHPDwords, ID.str(), title.str(), -0.5, 500.5, 501 );
          }

        }
        catch ( const GaudiException & excpt )
        {
          Error( excpt.message() ).ignore();
        }

      } // loop over HPDs

    } // loop over ingresses

    std::ostringstream title, ID;
    title << "Data Size (# 32-bit words) : L1 HardwareID " << l1HardID;
    ID << "L1s/L1HardwareID" << l1HardID;
    plot1D( nL1Words, ID.str(), title.str(), -0.5, 500.5, 501 );

  } // loop over L1 boards

  // test, by going direct to the RawBanks
  /*
    {
    // full TES location for this event
    const std::string reLoc = LHCb::RawEventLocation::Default;

    // load the raw RICH data at this location
    if ( exist<LHCb::RawEvent>(reLoc) )
    {
    LHCb::RawEvent * rawEvent = get<LHCb::RawEvent>(reLoc);

    // Get the banks for the Rich
    const LHCb::RawBank::Vector & richBanks = rawEvent->banks( LHCb::RawBank::Rich );

    // Loop over data banks
    for ( LHCb::RawBank::Vector::const_iterator iBank = richBanks.begin();
    iBank != richBanks.end(); ++iBank )
    {
    const int L1size     = (*iBank)->size() / 4; // size in # 32 bit words
    const Level1HardwareID L1ID ( (*iBank)->sourceID() );

    std::ostringstream title, ID;
    title << "Main Event Data Size (# 32-bit words) : L1 HardwareID " << L1ID;
    ID << "RawBanks/L1ID" << L1ID;
    plot1D( L1size, ID.str(), title.str(), -0.5, 500.5, 501 );

    } // raw banks

    } // raw event exists
    }
  */

  return StatusCode::SUCCESS;
}
