// Include files
// from STD
#include <sstream>

// from Detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Event
#include "Event/OTTime.h"

// local
#include "OTTimeCreator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : OTTimeCreator
//
// 2004-06-06 : Jacopo Nardulli
//-----------------------------------------------------------------------------

using namespace LHCb;
 
// Declaration of the tool Factory
DECLARE_ALGORITHM_FACTORY( OTTimeCreator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTTimeCreator::OTTimeCreator(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_tracker(NULL),
    m_decoder("OTRawBankDecoder")
{
  declareProperty( "OutputLocation", m_timeLocation = OTTimeLocation::Default );
  declareProperty( "RawBankDecoder", m_decoder ) ;
}

//=============================================================================
// Destructor
//=============================================================================
OTTimeCreator::~OTTimeCreator() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTTimeCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  // Loading OT Geometry from XML
  m_tracker = getDet<DeOTDetector>( DeOTDetectorLocation::Default );

  m_decoder.retrieve().ignore();

  return sc;
}

StatusCode OTTimeCreator::finalize() {
  m_decoder.release().ignore();
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTTimeCreator::execute() 
{
  // make OTTime container 
  OTTimes* outputTimes = new OTTimes();
  outputTimes->reserve(10000);
  // register output buffer to TES
  put(outputTimes, m_timeLocation);

  // Load all modules.
  for(DeOTDetector::Modules::const_iterator imod = m_tracker->modules().begin() ;
      imod != m_tracker->modules().end(); ++imod) {
    LHCb::OTLiteTimeRange ottimes = m_decoder->decodeModule( (*imod)->elementID() ) ;
    for( LHCb::OTLiteTimeRange::const_iterator it = ottimes.begin() ; it != ottimes.end(); ++it) {
      bool found(false) ;
      for( LHCb::OTLiteTimeRange::const_iterator jt = ottimes.begin() ; it != jt && !found; ++jt)
	found = it->channel()==jt->channel() ;
      if ( found ) {
        std::ostringstream mess;
        mess << "Found duplicate hit!" << it->channel();
        Warning( mess.str(), StatusCode::FAILURE, 0 ).ignore();
      } else outputTimes->insert(new OTTime(it->channel(),it->calibratedTime())) ;
    }
  }
  
  if ( msgLevel( MSG::DEBUG ) ) debug() << " OTTimeCreator created " << outputTimes->size() 
                                        << " OTTime(s) at " << m_timeLocation << endmsg;

  return StatusCode::SUCCESS;
}
