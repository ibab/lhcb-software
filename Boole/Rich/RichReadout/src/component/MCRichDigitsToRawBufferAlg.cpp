
//-----------------------------------------------------------------------------
/** @file MCRichDigitsToRawBufferAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitsToRawBufferAlg.cpp,v 1.7 2006-03-01 09:53:54 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichDigitsToRawBufferAlg.h"

// namespaces
using namespace LHCb; ///< General LHCb namespace

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichDigitsToRawBufferAlg>          s_factory ;
const        IAlgFactory& MCRichDigitsToRawBufferAlgFactory = s_factory ;

// Standard constructor
MCRichDigitsToRawBufferAlg::MCRichDigitsToRawBufferAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : RichAlgBase   ( name, pSvcLocator ),
    m_rawFormatT  ( 0 )
{

  declareProperty( "MCRichDigitsLocation",
                   m_digitsLoc = MCRichDigitLocation::Default );
  declareProperty( "DataVersion", m_version = RichDAQ::LHCb2 );

}

// Destructor
MCRichDigitsToRawBufferAlg::~MCRichDigitsToRawBufferAlg() {};

// Initialisation.
StatusCode MCRichDigitsToRawBufferAlg::initialize()
{

  // intialise base lcass
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // acquire tools
  acquireTool( "RichRawDataFormatTool", m_rawFormatT, 0, true );

  info() << "Using RICH Level1 buffer format : " << m_version << endreq;

  return sc;
}

// Main execution
StatusCode MCRichDigitsToRawBufferAlg::execute()
{

  // Retrieve MCRichDigits
  const MCRichDigits * digits = get<MCRichDigits>( m_digitsLoc );

  // new vector of smart IDs
  LHCb::RichSmartID::Vector smartIDs;

  // Loop over digits and fill smartIDs into vector
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit )
  {
    smartIDs.push_back( (*iDigit)->key() );
  }

  // Fill raw buffer
  m_rawFormatT->fillRawEvent( smartIDs, (RichDAQ::BankVersion)m_version );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode MCRichDigitsToRawBufferAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
