
/** @file MCRichDigitsToRawBufferAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitsToRawBufferAlg.cpp,v 1.2 2005-01-13 12:59:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2005/01/07 13:21:22  jonrob
 *  Add new algorithm
 *
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

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
  acquireTool( "RichRawDataFormatTool", m_rawFormatT );
  acquireTool( "RichHPDToLevel1Tool",   m_level1     );

  info() << "Using RICH Level1 buffer format : " << m_version << endreq;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode MCRichDigitsToRawBufferAlg::execute()
{

  // debug message
  debug() << "Execute" << endreq;

  // Retrieve MCRichDigits
  const MCRichDigits * digits = get<MCRichDigits>( m_digitsLoc );

  // new rich data map
  RichDAQ::L1Map L1Data;

  // Loop over digits and sort according to L1 and HPD
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit ) {

    // Get Level 1 ID number
    const RichDAQ::Level1ID L1ID = m_level1->levelL1ID( (*iDigit)->key() );

    // Get reference to L1 group
    RichDAQ::PDMap & PDs = L1Data[ L1ID ];

    // Finally, insert this pixel into correct place
    PDs[ (*iDigit)->key().pdID() ].push_back( (*iDigit)->key() );

  }

  // Fill raw buffer
  m_rawFormatT->createDataBank( L1Data, (RichDAQ::BankVersion)m_version );

  debug() << "Created " << L1Data.size() << " RICH L1 bank(s) for "
          << digits->size() << " MCRichDigits" << endreq;

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode MCRichDigitsToRawBufferAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
