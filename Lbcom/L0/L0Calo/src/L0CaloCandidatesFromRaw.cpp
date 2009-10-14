// $Id: L0CaloCandidatesFromRaw.cpp,v 1.18 2009-10-14 10:21:36 robbep Exp $
// Include files 
// local
#include "L0CaloCandidatesFromRaw.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// From Event
#include "Event/RawBankReadoutStatus.h"
#include "Event/L0CaloCandidate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRaw
//
// 2003-12-15 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0CaloCandidatesFromRaw ) ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRaw::L0CaloCandidatesFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Extension",      m_extension = "" );
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloCandidatesFromRaw::~L0CaloCandidatesFromRaw() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;  

  debug() << "==> Initialize" << endmsg;

  m_convertTool = 
    tool< L0CaloCandidatesFromRawBank >( "L0CaloCandidatesFromRawBank" ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::execute() {

  debug() << "==> Execute" << endmsg;

  std::string name     = rootInTES() + 
    LHCb::L0CaloCandidateLocation::Default + m_extension;
  std::string nameFull = rootInTES() + 
    LHCb::L0CaloCandidateLocation::Full + m_extension;

  LHCb::RawEvent * rawEvt = 0 ;
  int version = -1 ;
  std::vector<std::vector<unsigned int> > data;

  LHCb::RawBankReadoutStatus readoutStatus( LHCb::RawBank::L0Calo ) ;
  readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::OK ) ;
  readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::OK ) ;
  
  if ( exist< LHCb::RawEvent >( LHCb::RawEventLocation::Default ) ) {

    rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default ) ;
    const std::vector<LHCb::RawBank*>& banks = 
      rawEvt->banks( LHCb::RawBank::L0Calo );
    
    // check presence of error bank
    const std::vector< LHCb::RawBank* > * errBanks = 
      &rawEvt -> banks( LHCb::RawBank::L0CaloError ) ;

    if ( ( 0 != errBanks ) && ( 0 != errBanks -> size() ) ) {
      std::vector< LHCb::RawBank * >::const_iterator it ;
      for ( it = errBanks->begin() ; errBanks -> end() != it ; ++it ) {
        readoutStatus.addStatus( (*it) -> sourceID() , 
                                 LHCb::RawBankReadoutStatus::ErrorBank ) ;
      }
    }

    if ( 0 == banks.size() ) {
      Error( "L0Calo Bank has not been found" ).ignore() ;
      readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing ) ;
      readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::Missing ) ;      
    } else {
      int sourceZero( 0 ) , sourceOne( 0 ) ;

      // convert the banks to two arrays of ints.
      data.reserve( banks.size() ) ;
      for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); 
            banks.end() != itBnk; ++itBnk ) {

        data.push_back( std::vector< unsigned int >( (*itBnk) -> begin< unsigned int >() ,
                                                     (*itBnk) -> end< unsigned int >() ) ) ;
        
        if ( 0 == (*itBnk) -> sourceID() ) ++sourceZero ;
        else ++sourceOne ;
      }
      // Version of the bank
      version = banks.front() -> version() ;
      if ( 0 == sourceZero ) 
        readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing ) ;
      if ( 0 == sourceOne ) 
        readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::Missing ) ;
      if ( 1 < sourceZero ) 
        readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::NonUnique ) ;
      if ( 1 < sourceOne ) 
        readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::NonUnique ) ;
    }

  } else {
    Warning( "RawEvent not found" ).ignore() ;
  }

  m_convertTool->convertRawBankToTES( data, nameFull, name , version , 
                                      readoutStatus );  

  // Now put the status on TES also
  LHCb::RawBankReadoutStatuss * statuss = 
    getOrCreate< LHCb::RawBankReadoutStatuss , LHCb::RawBankReadoutStatuss > 
    ( LHCb::RawBankReadoutStatusLocation::Default ) ;
  
  LHCb::RawBankReadoutStatus * status = statuss -> object( readoutStatus.key() ) ;
  
  if ( 0 == status ) {
    status = new LHCb::RawBankReadoutStatus( readoutStatus ) ;
    statuss -> insert( status ) ;
  } else {
    // merge both status if already exists
    if ( status -> status() != readoutStatus.status() ) {
      std::map< int , long >::iterator it ;
      for ( it = readoutStatus.statusMap().begin() ; 
            it != readoutStatus.statusMap().end() ; ++it ) {
        status -> addStatus( (*it).first , (*it).second ) ;
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::finalize() {
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize(); 
}
//=============================================================================
