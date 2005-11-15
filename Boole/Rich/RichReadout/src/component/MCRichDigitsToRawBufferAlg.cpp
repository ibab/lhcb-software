
//-----------------------------------------------------------------------------
/** @file MCRichDigitsToRawBufferAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitsToRawBufferAlg.cpp,v 1.4 2005-11-15 13:29:56 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// local
#include "MCRichDigitsToRawBufferAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichDigitsToRawBufferAlg>          s_factory ;
const        IAlgFactory& MCRichDigitsToRawBufferAlgFactory = s_factory ;

// Standard constructor
MCRichDigitsToRawBufferAlg::MCRichDigitsToRawBufferAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : RichAlgBase   ( name, pSvcLocator ),
    m_rawFormatT  ( 0 ),
    m_level1      ( 0 )
{

  declareProperty( "MCRichDigitsLocation",
                   m_digitsLoc = MCRichDigitLocation::Default );
  declareProperty( "DataVersion", m_version = 1 );

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
  acquireTool( "RichDetNumberingTool",  m_level1,     0, true );

  // create a dummy L1data object with an emtpy vector for each L1 board
  m_dummyMap.clear();
  for ( RichDAQ::Level1IDs::const_iterator iID = m_level1->level1IDs().begin();
        iID != m_level1->level1IDs().end(); ++iID )
  {
    m_dummyMap[ *iID ];
  }
  debug() << "Created " << m_dummyMap.size() << " entries in empty L1 map : L1IDs = " 
          << m_level1->level1IDs() << endreq;

  info() << "Using RICH Level1 buffer format : " << m_version << endreq;

  return sc;
}

// Main execution
StatusCode MCRichDigitsToRawBufferAlg::execute()
{

  // Retrieve MCRichDigits
  const MCRichDigits * digits = get<MCRichDigits>( m_digitsLoc );

  // new rich data map
  RichDAQ::L1Map L1Data = m_dummyMap;

  // Loop over digits and sort according to L1 and HPD
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit ) 
  {

    // Get Level 1 ID number
    const RichDAQ::Level1ID L1ID = m_level1->level1ID( (*iDigit)->key() );

    // Get reference to L1 group
    RichDAQ::PDMap & PDs = L1Data[ L1ID ];

    // Finally, insert this pixel into correct place
    PDs[ (*iDigit)->key().pdID() ].push_back( (*iDigit)->key() );

  }

  // Fill raw buffer
  m_rawFormatT->createDataBank( L1Data, (RichDAQ::BankVersion)m_version );

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << L1Data.size() << " RICH L1 bank(s) for "
            << digits->size() << " MCRichDigits" << endreq;
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode MCRichDigitsToRawBufferAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
