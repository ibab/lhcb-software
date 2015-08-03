// Include files
#include <algorithm>

// Event
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// local
#include "HltLumiSummaryDecoder.h"
#include "Event/HltLumiSummary.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( HltLumiSummaryDecoder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltLumiSummaryDecoder::HltLumiSummaryDecoder( const std::string& name,
					      ISvcLocator* pSvcLocator)
  : Decoder::AlgBase ( name , pSvcLocator )
  , m_totDataSize(0), m_nbEvents(0)
{
  m_rawEventLocations={LHCb::RawEventLocation::Trigger,LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
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
  StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

  m_nbEvents    = 0;
  m_totDataSize = 0;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltLumiSummaryDecoder::execute() {

  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;
  
  // ------------------------------------------
  // get (existing) container  >>>>>>>>>>>>>>>>>>>> later: if exists: return!!!!!!!!!!
  LHCb::HltLumiSummary* hltLumiSummary = NULL;
  if ( !exist<LHCb::HltLumiSummary>(m_OutputContainerName) ){
    // create output container on the TES
    hltLumiSummary = new LHCb::HltLumiSummary();
    // locate them in the TES
    put(hltLumiSummary, m_OutputContainerName); 
    if ( msgLevel( MSG::DEBUG ) )
      debug() << m_OutputContainerName << " not found, made a new one" << endmsg ;
  }
  else {
    // in this case should just do nothing!!!
    if ( msgLevel( MSG::DEBUG ) )
      debug() << m_OutputContainerName << " found, do nothing" << endmsg ;
    return StatusCode::SUCCESS;
  }
  
  // Retrieve the RawEvent:
  // get data container
  RawEvent* event = findFirstRawEvent();
  if( NULL == event ){
    return Warning("RawEvent cannot be loaded",StatusCode::FAILURE);
  }
  // Get the buffers associated with the HltLumiSummary
  const std::vector<RawBank*>& banks = event->banks( RawBank::HltLumiSummary );
  // Now copy the information from all banks (normally there should only be one)
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
                  << endmsg;
      }
      // add this counter
      hltLumiSummary->addInfo( iKey, iVal);
    }
    
    // keep statistics
    int totDataSize = 0;
    totDataSize += (*ibank)->size()/sizeof( unsigned int ) ;
    m_totDataSize += totDataSize;
    m_nbEvents++;
    
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << "Bank size: ";
      debug() << format( "%4d ", (*ibank)->size() ) 
	      << "Total Data bank size " << totDataSize << endmsg;
    }
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode HltLumiSummaryDecoder::finalize()
{
  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    info() << "Average event size : " << format( "%7.1f words", m_totDataSize ) 
	   << endmsg;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
