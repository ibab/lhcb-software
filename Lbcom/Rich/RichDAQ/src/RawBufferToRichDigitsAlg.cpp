
//-----------------------------------------------------------------------------
/** @file RawBufferToRichDigitsAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RawBufferToRichDigitsAlg
 *
 *  CVS Log :-
 *  $Id: RawBufferToRichDigitsAlg.cpp,v 1.15 2006-03-01 09:56:12 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBufferToRichDigitsAlg.h"

// namespaces
using namespace LHCb; ///< General LHCb namespace

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RawBufferToRichDigitsAlg>          s_factory ;
const        IAlgFactory& RawBufferToRichDigitsAlgFactory = s_factory ;

// Standard constructor, initializes variables
RawBufferToRichDigitsAlg::RawBufferToRichDigitsAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : RichAlgBase ( name , pSvcLocator )
{
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLoc = RichDigitLocation::Default );
  declareProperty( "DecodeBufferOnly", m_decodeOnly = false );
}

// Destructor
RawBufferToRichDigitsAlg::~RawBufferToRichDigitsAlg() {};

// Initialisation.
StatusCode RawBufferToRichDigitsAlg::initialize()
{
  // intialise base
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichSmartIDDecoder", m_decoder );

  return sc;
}

// Main execution
StatusCode RawBufferToRichDigitsAlg::execute()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Execute" << endreq;

  // Get RichSmartIDs decoded from RawEvent
  const RichDAQ::PDMap & smartIDs = m_decoder->allRichSmartIDs();

  if ( !m_decodeOnly )
  {
    // Make new container for RichDigits and give to Gaudi
    RichDigits * digits = new RichDigits();
    put( digits, m_richDigitsLoc );
    
    // Create a RichDigit for each SmartID
    for ( RichDAQ::PDMap::const_iterator iPD = smartIDs.begin();
          iPD != smartIDs.end(); ++iPD )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iPD).second.begin();
            iID != (*iPD).second.end(); ++iID )
      {
        digits->insert( new RichDigit(), *iID );
      }
    }
    
    // Final printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Successfully registered " << digits->size()
              << " RichDigits at " << m_richDigitsLoc << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
