
//-----------------------------------------------------------------------------
/** @file MCRichHitsToRawBufferAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : MCRichHitsToRawBufferAlg
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichHitsToRawBufferAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespaces
using namespace Rich::MC::Digi;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCRichHitsToRawBufferAlg )

// Standard constructor
MCRichHitsToRawBufferAlg::MCRichHitsToRawBufferAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name, pSvcLocator ),
    m_rawFormatT  ( NULL )
{
  declareProperty( "MCRichHitsLocation",
                   m_hitsLoc = LHCb::MCRichHitLocation::Default );
  declareProperty( "DataVersion", m_version = Rich::DAQ::LHCb2 );
}

// Destructor
MCRichHitsToRawBufferAlg::~MCRichHitsToRawBufferAlg() { }

// Initialisation.
StatusCode MCRichHitsToRawBufferAlg::initialize()
{

  // intialise base lcass
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichRawDataFormatTool", m_rawFormatT, 0, true );

  info() << "Using RICH Level1 buffer format : " << m_version << endmsg;

  return sc;
}

// Main execution
StatusCode MCRichHitsToRawBufferAlg::execute()
{

  // Retrieve MCRichHits
  const LHCb::MCRichHits * hits = get<LHCb::MCRichHits>( m_hitsLoc );

  // new vector of smart IDs
  LHCb::RichSmartID::Vector smartIDs;

  // Loop over hits and fill smartIDs into vector
  for ( LHCb::MCRichHits::const_iterator iHit = hits->begin();
        iHit != hits->end(); ++iHit )
  {
    smartIDs.push_back( (*iHit)->sensDetID() );
  }

  // Fill raw buffer
  m_rawFormatT->fillRawEvent( smartIDs, (Rich::DAQ::BankVersion)m_version );

  return StatusCode::SUCCESS;
}
