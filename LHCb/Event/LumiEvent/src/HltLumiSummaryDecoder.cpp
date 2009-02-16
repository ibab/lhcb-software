// $Id: HltLumiSummaryDecoder.cpp,v 1.1.1.1 2009-02-16 16:04:17 panmanj Exp $
// Include files
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// Event
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// local
#include "HltLumiSummaryDecoder.h"
#include "Event/HltLumiSummary.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( HltLumiSummaryDecoder );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltLumiSummaryDecoder::HltLumiSummaryDecoder( const std::string& name,
					      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "RawEventLocation"    , m_rawEventLocation    = LHCb::RawEventLocation::Default );
  declareProperty( "OutputContainerName" , m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
HltLumiSummaryDecoder::~HltLumiSummaryDecoder() {}

//=============================================================================

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode HltLumiSummaryDecoder::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_bankType  = LHCb::RawBank::HltLumiSummary;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltLumiSummaryDecoder::execute() {

  debug() << "==> Execute" << endmsg;
  
  // ------------------------------------------
  // get (existing) container  >>>>>>>>>>>>>>>>>>>> later: if exists: return!!!!!!!!!!
  if ( !exist<LHCb::HltLumiSummary>(m_OutputContainerName) ){
    // create output container on the TES
    m_HltLumiSummary = new LHCb::HltLumiSummary();
    // locate them in the TES
    put(m_HltLumiSummary, m_OutputContainerName); 
    debug() << m_OutputContainerName << " not found, made a new one" << endmsg ;
  }
  else {
    // in this case should just do nothing!!!
    debug() << m_OutputContainerName << " found, do nothing" << endmsg ;
    return StatusCode::SUCCESS;
  }
  
  // Retrieve the RawEvent:
  // get data container
  if( !exist<RawEvent>(m_rawEventLocation) ){
    StatusCode sc = Warning("RawBank cannot be loaded",StatusCode::FAILURE);
    return sc;
  }
  RawEvent* event = get<RawEvent>(m_rawEventLocation);
  // Get the buffers associated with the HltLumiSummary
  const std::vector<RawBank*>& banks = event->banks( RawBank::HltLumiSummary );
  // Now copy the information from all banks (normally there should only be one)
  bool decodingerror(false) ;
  for (std::vector<RawBank*>::const_iterator  ibank = banks.begin();
       ibank != banks.end() ; ++ibank) {
    // get now the raw data
    const unsigned int* idata = (*ibank)->data() ;
    
    // The data part
    const unsigned int* begin = idata ;
    const unsigned int* end   = idata + (*ibank)->size()/sizeof( unsigned int ) ;
    for( const unsigned int* itW = begin; end != itW; itW++ ) {
      // decode the info
      int iKey = (*itW >> 16);
      int iVal = (*itW & 0xFFFF);
      if ( MSG::VERBOSE >= msgLevel() ) {
	verbose() << format ( " %8x %11d %11d %11d ", *itW, *itW, iKey, iVal ) 
		  << endreq;
      }
      // add this counter
      m_HltLumiSummary->addInfo( iKey, iVal);
    }
    
    // keep statistics
    int totDataSize = 0;
    totDataSize += (*ibank)->size()/sizeof( unsigned int ) ;
    m_totDataSize += totDataSize;
    m_nbEvents++;
    
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Bank size: ";
      debug() << format( "%4d ", (*ibank)->size() ) 
	      << "Total Data bank size " << totDataSize << endreq;
    }
  }

  return decodingerror ? StatusCode::FAILURE : StatusCode::SUCCESS ;
};

//=============================================================================
// Finalize
//=============================================================================
StatusCode HltLumiSummaryDecoder::finalize()
{
  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    info() << "Average event size : " << format( "%7.1f words", m_totDataSize ) 
	   << endreq;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
};
