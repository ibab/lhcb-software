// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/DumpL0CaloBanks.cpp,v 1.1 2008-06-18 09:21:14 robbep Exp $

#include <fstream>

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// Event/L0Event
#include "Event/RawEvent.h"
#include "Event/ODIN.h"

// local
#include "DumpL0CaloBanks.h"

DECLARE_ALGORITHM_FACTORY( DumpL0CaloBanks );

//-----------------------------------------------------------------------------
// Implementation file for class : DumpL0CaloBanks
//
// 27/05/2008 : Patrick Robbe
//-----------------------------------------------------------------------------

// Standard creator
DumpL0CaloBanks::DumpL0CaloBanks( const std::string& name, 
                                  ISvcLocator* pSvcLocator )  
  : GaudiAlgorithm( name , pSvcLocator ) , m_event( 0 ) 
{ 
  declareProperty( "FileName"  , m_fileName  = "l0calobanks.txt" ) ;  
}

//=============================================================================
// Standard destructor
//=============================================================================
DumpL0CaloBanks::~DumpL0CaloBanks() {};

//=============================================================================
// Finalize.
//=============================================================================
StatusCode DumpL0CaloBanks::finalize() {
  m_file -> close() ;
  delete m_file ;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DumpL0CaloBanks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_file = new std::ofstream( m_fileName.c_str() ) ;
  
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpL0CaloBanks::execute() {
  // ODIN first 
  (*m_file) << "---------------------------------------------" << ++m_event << std::endl ;
  
  if ( exist< LHCb::ODIN >( LHCb::ODINLocation::Default ) ) {
    LHCb::ODIN * odin = get< LHCb::ODIN >( LHCb::ODINLocation::Default ) ;
    (*m_file) << "event = " << odin -> eventNumber() << " BCId = " << odin -> bunchId() 
              << std::endl ;
  }
  
  LHCb::RawEvent * rawEvt( 0 ) ;
  if ( exist< LHCb::RawEvent >( LHCb::RawEventLocation::Default ) ) {
    rawEvt = get< LHCb::RawEvent >( LHCb::RawEventLocation::Default ) ;
    const std::vector< LHCb::RawBank * > & banks = rawEvt -> banks( LHCb::RawBank::L0Calo ) ;
  
    if ( 0 == banks.size() ) return StatusCode::SUCCESS ;
    
    for ( std::vector< LHCb::RawBank * >::const_iterator it = banks.begin() ; 
          banks.end() != it ; ++it ) { 
      unsigned int * body = (*it) -> data() ;
      for ( int k = 0 ; (*it) -> size() / 4 > k ; ++k ) {
          (*m_file) << std::hex << (*body++) << std::endl ;
      }
    }
  }

  //  return sc ; 
  return StatusCode::SUCCESS; 
}
//============================================================================
