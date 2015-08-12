// Include files 

// event model
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/FileId.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

// local
#include "LumiFileReader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiFileReader
//
// 2009-10-06 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiFileReader )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiFileReader::LumiFileReader( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_totDataSize(0.),
    m_nbEvents(0),
    m_bankType(LHCb::RawBank::FileID),
    m_HltLumiSummary(NULL),
    m_odin(NULL),
    m_fileId()
{
  declareProperty( "RawEventLocation"    , m_rawEventLocation    = LHCb::RawEventLocation::Default );
  declareProperty( "OutputFileName" , m_OutputFileName = "lumi.dat");
 
}
//=============================================================================
// Destructor
//=============================================================================
LumiFileReader::~LumiFileReader() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiFileReader::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_nbEvents    = 0;
  m_totDataSize = 0;
  m_bankType  = LHCb::RawBank::FileID; 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiFileReader::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get lumi data container
  if( !exist<LHCb::HltLumiSummary>(LHCb::HltLumiSummaryLocation::Default) ){
    // not sure that this is an error - physics events do not have the lumi bank
    return Warning("LumiSummary cannot be loaded", StatusCode::SUCCESS);
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "LumiSummary loaded!!" << endmsg;
  m_HltLumiSummary = get<LHCb::HltLumiSummary>(LHCb::HltLumiSummaryLocation::Default);

  // get ODIN
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    m_odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    // should remain flagged as an error
    error() << "ODIN cannot be loaded" << endmsg;
    return StatusCode::SUCCESS;
  }
  // obtain the run number from ODIN
  unsigned int run = m_odin->runNumber();  
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN RunNumber: " << run << endmsg;
  
  // Retrieve the RawEvent:
  if( !exist<LHCb::RawEvent>(m_rawEventLocation) ){
    StatusCode sc = Warning("RawBank cannot be loaded",StatusCode::FAILURE);
    return sc;
  }
  std::vector<unsigned int> id_bank;
  id_bank.reserve(4);
  LHCb::RawEvent* event = get<LHCb::RawEvent>(m_rawEventLocation);
  // Get the buffers associated with the HltLumiSummary
  const std::vector<LHCb::RawBank*>& banks = event->banks( m_bankType );
  if ( msgLevel( MSG::DEBUG ) ) debug() << "Bank size: " << banks.size() << endmsg;
  // Now copy the information from all banks (normally there should only be one)
  for (std::vector<LHCb::RawBank*>::const_iterator  ibank = banks.begin();
       ibank != banks.end() ; ++ibank) {
    // get now the raw data
    const unsigned int* idata = (*ibank)->data() ;
    
    // The data part
    const unsigned int* begin = idata ;
    const unsigned int* end   = idata + (*ibank)->size()/sizeof( unsigned int ) ;
    for( const unsigned int* itW = begin; end != itW; itW++ ) {
      // decode the info
      id_bank.push_back( *itW );
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
    // decode ID
    std::string s = m_fileId.fileIDint2str(id_bank);
    if ( msgLevel(MSG::INFO) ) info() << "RunInfo record: " << s << endmsg;
    break;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiFileReader::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 < m_nbEvents ) {
    m_totDataSize /= m_nbEvents;
    info() << "Average event size : " << format( "%7.1f words", m_totDataSize ) 
	   << endmsg;
  }

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


//=============================================================================
