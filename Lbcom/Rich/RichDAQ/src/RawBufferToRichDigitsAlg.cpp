
/** @file RawBufferToRichDigitsAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RawBufferToRichDigitsAlg
 *
 *  CVS Log :-
 *  $Id: RawBufferToRichDigitsAlg.cpp,v 1.11 2004-11-03 09:30:16 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.10  2004/11/02 13:13:49  jonrob
 *  minro update
 *
 *  Revision 1.9  2004/10/30 19:13:05  jonrob
 *  Reworking RawBuffer decoding as a tool, to allow reconstruction to skip RichDigit creation
 *
 *  Revision 1.8  2004/10/13 09:16:27  jonrob
 *  Use Data on Demand Service + various speed improvements
 *
 *  Revision 1.7  2004/07/27 13:46:06  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RawBufferToRichDigitsAlg.h"

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

  return StatusCode::SUCCESS;
};


// Main execution
StatusCode RawBufferToRichDigitsAlg::execute() 
{
  debug() << "Execute" << endreq;

  // Make new container for RichDigits and give to Gaudi
  RichDigits * digits = new RichDigits();
  put( digits, m_richDigitsLoc );

  // Get RichSmartIDs decoded from RawEvent
  const RichSmartID::Collection & smartIDs = m_decoder->allRichSmartIDs();

  // Create a RichDigit for each SmartID
  for ( RichSmartID::Collection::const_iterator iID = smartIDs.begin();
        iID != smartIDs.end(); ++iID ) {
    digits->insert( new RichDigit(), *iID );
  }

  // Final printout
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Successfully registered " << digits->size()
            << " RichDigits at " << m_richDigitsLoc << endreq;
  }

  return StatusCode::SUCCESS;
};


//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
