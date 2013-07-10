
//-----------------------------------------------------------------------------
/** @file MCRichDigitsToRawBufferAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichDigitsToRawBufferAlg.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// namespaces
using namespace Rich::MC::Digi;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCRichDigitsToRawBufferAlg )

// Standard constructor
MCRichDigitsToRawBufferAlg::MCRichDigitsToRawBufferAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name, pSvcLocator ),
    m_rawFormatT  ( NULL )
{
  declareProperty( "MCRichDigitsLocation",
                   m_digitsLoc = LHCb::MCRichDigitLocation::Default );
  declareProperty( "DataVersion", m_version = Rich::DAQ::LHCb2 );
}

// Destructor
MCRichDigitsToRawBufferAlg::~MCRichDigitsToRawBufferAlg() {}

// Initialisation.
StatusCode MCRichDigitsToRawBufferAlg::initialize()
{
  // intialise base class
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichRawDataFormatTool", m_rawFormatT, 0, true );

  info() << "Using RICH Level1 buffer format : " << m_version << endmsg;

  return sc;
}

// Main execution
StatusCode MCRichDigitsToRawBufferAlg::execute()
{
  // Retrieve MCRichDigits
  const LHCb::MCRichDigits * digits = get<LHCb::MCRichDigits>( m_digitsLoc );

  // new vector of smart IDs
  LHCb::RichSmartID::Vector smartIDs;

  // Loop over digits and fill smartIDs into vector
  smartIDs.reserve( digits->size() );
  for ( LHCb::MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit )
  {
    smartIDs.push_back( (*iDigit)->key() );
  }

  // Fill raw buffer
  m_rawFormatT->fillRawEvent( smartIDs, (Rich::DAQ::BankVersion)m_version );

  return StatusCode::SUCCESS;
}
