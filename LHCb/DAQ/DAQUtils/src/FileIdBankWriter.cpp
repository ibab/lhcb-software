// Include files 

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

// event model
#include "Event/RawEvent.h"
#include "Event/FileId.h"

// local
#include "FileIdBankWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FileIdBankWriter
//
// 2009-10-01 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FileIdBankWriter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FileIdBankWriter::FileIdBankWriter( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_count_files(0)
{
  declareProperty( "RawEventLocation"  ,  m_rawEventLocation = LHCb::RawEventLocation::Default );
 
}
//=============================================================================
// Destructor
//=============================================================================
FileIdBankWriter::~FileIdBankWriter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FileIdBankWriter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // file counting 
  m_current_fname = "";
  m_count_files = 0;

  // for output
  m_bank.reserve(10);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FileIdBankWriter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // registry from raw data - only correct if file catalogue used 
  std::string event_fname;
  LHCb::RawEvent* event = getIfExists<LHCb::RawEvent>(m_rawEventLocation);
  if( event == NULL ){
    return Warning("RawBank cannot be loaded", StatusCode::SUCCESS);
  } else {
    IOpaqueAddress* eAddr = event->registry()->address();
    // obtain the fileID
    if ( eAddr ) {
      event_fname = eAddr->par()[0];
      if ( msgLevel(MSG::DEBUG) ) debug() << "RunInfo record: " << event_fname << endmsg;
    } else {
      return Error("Registry cannot be loaded from Event", StatusCode::SUCCESS);
    }
  }

  // check if the file ID is new
  if ( event_fname != m_current_fname ) {
    m_count_files++; // this is a new file
    m_current_fname = event_fname;
    if ( msgLevel(MSG::INFO) ) info() << "RunInfo record: " << event_fname << endmsg;
    // calculate identifier
    m_bank = m_fileId.fileIDstr2int(event_fname);
  }
  // add now to the raw data: set source, type, version 
  event->addBank( 0, LHCb::RawBank::FileID, 0, m_bank );   

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FileIdBankWriter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( msgLevel(MSG::INFO) ) {
    info() << "number of files seen: " << m_count_files << endmsg;
  }
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
