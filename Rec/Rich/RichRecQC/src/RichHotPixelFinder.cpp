
//-----------------------------------------------------------------------------
/** @file RichHotPixelFinder.cpp
 *
 *  Implementation file for monitor : Rich::DAQ::HotPixelFinder
 *
 *  CVS Log :-
 *  $Id: RichHotPixelFinder.cpp,v 1.1 2009-09-02 16:24:36 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichHotPixelFinder.h"

//-----------------------------------------------------------------------------

using namespace Rich::DAQ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HotPixelFinder );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HotPixelFinder::HotPixelFinder( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Rich::AlgBase      ( name , pSvcLocator ),
    m_SmartIDDecoder   ( NULL  ),
    m_RichSys          ( NULL  ),
    m_taeEvents        ( 1, "" ),
    m_nEvts            ( 0     )
{
  declareProperty( "RawEventLocations", m_taeEvents );
  declareProperty( "NEventsForAverage", m_nEventsForAverage = 100 );
  declareProperty( "HotOcc",            m_hotOcc = 0.9 );
  // do our own printing at the end
  setProperty( "StatPrint", false );
}

//=============================================================================
// Destructor
//=============================================================================
HotPixelFinder::~HotPixelFinder() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HotPixelFinder::initialize()
{
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

//=============================================================================
// finalization
//=============================================================================
StatusCode HotPixelFinder::finalize()
{
  
  // print hot pixels
  for ( Statistics::const_iterator iHotPix = counters().begin();
        iHotPix != counters().end(); ++iHotPix )
  {
    warning() << iHotPix->first << " : Instances = " << iHotPix->second << endmsg;
  }

  return Rich::AlgBase::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HotPixelFinder::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  for ( std::vector<std::string>::const_iterator iLoc = m_taeEvents.begin();
        iLoc != m_taeEvents.end(); ++iLoc )
  {
    sc = sc && processTAEEvent( *iLoc );
  }

  return sc;
}

StatusCode HotPixelFinder::processTAEEvent( const std::string & taeEvent )
{
  // get the decoded data for this tae event
  const Rich::DAQ::L1Map & l1Map = m_SmartIDDecoder->allRichSmartIDs(taeEvent);

  // Count events
  ++m_nEvts;

  for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map )
  {
    const Rich::DAQ::Level1HardwareID l1HardID = iL1Map->first;
    const Rich::DAQ::IngressMap & ingressMap  = iL1Map->second;

    for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap )
    {
      const Rich::DAQ::IngressInfo & ingressInfo = iIngressMap->second;
      const Rich::DAQ::HPDMap & hpdMap           = ingressInfo.hpdData();

      for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap )
      {

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = (*iHPDMap).second.smartIDs();

        // Loop over raw RichSmartIDs
        for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
              iR != rawIDs.end(); ++iR )
        {
          // count hits per channel
          ++m_hitCount[*iR];
        }

      }
    }
  }

  // Look for hot pixels
  if ( m_nEvts == m_nEventsForAverage )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Searching for hot pixels after " << m_nEvts << " events" << endmsg;
  
    for ( HitCountMap::const_iterator iHPDC = m_hitCount.begin();
          iHPDC != m_hitCount.end(); ++iHPDC )
    {
      const double occ = (double)(iHPDC->second) / (double)m_nEventsForAverage;
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << iHPDC->first << " fraction on " << occ << endmsg;
      if ( occ >= m_hotOcc )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "  -> pixel is hot ...." << endmsg;
        std::ostringstream mess;
        mess << iHPDC->first << " (" << (unsigned int)(iHPDC->first) << ") " << 100.0*m_hotOcc 
             << "% hot in " << m_nEventsForAverage << " events";
        ++counter( mess.str() );
      }
    }

    // Reset counters
    m_nEvts = 0;
    m_hitCount.clear();

  }

  return StatusCode::SUCCESS;
}
