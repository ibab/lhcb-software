// Include files 
// local
#include "L0CaloCandidatesFromRaw.h"

// From Event
#include "Event/RawBankReadoutStatus.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0ProcessorData.h"

// Local 
#include "L0Candidate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRaw
//
// 2003-12-15 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0CaloCandidatesFromRaw )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRaw::L0CaloCandidatesFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : L0FromRawBase ( name , pSvcLocator ) 
  , m_convertTool(NULL)
{ 
  
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloCandidatesFromRaw::~L0CaloCandidatesFromRaw() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::initialize() {
  StatusCode sc = L0FromRawBase::initialize(); 
  if ( sc.isFailure() ) return sc;  

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_convertTool = 
    tool< L0CaloCandidatesFromRawBank >( "L0CaloCandidatesFromRawBank" ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::execute() {

  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

  std::string name = dataLocation( LHCb::L0CaloCandidateLocation::Default ) ;
  std::string nameFull = dataLocation( LHCb::L0CaloCandidateLocation::Full ) ;

  int version = -1 ;
  std::vector<std::vector<unsigned int> > data;

  LHCb::RawBankReadoutStatus readoutStatus( LHCb::RawBank::L0Calo ) ;
  readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::OK ) ;
  readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::OK ) ;
  
  // Scan the list of input location and select the first existing one.
  // no need to do this any longer, the base class takes care of it
  //std::string rawEventLocation;
  //if ( selectRawEventLocation(rawEventLocation).isFailure() ) 
  //  return Error("No valid raw event location found",StatusCode::SUCCESS,50);

  LHCb::L0CaloCandidates * outFull = new LHCb::L0CaloCandidates( ) ;
  LHCb::L0CaloCandidates * out = new LHCb::L0CaloCandidates() ;

  if ( writeOnTES() ) {
    put( outFull , nameFull , IgnoreRootInTES ) ;
    put( out, name , IgnoreRootInTES ) ;
    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << "L0CaloCandidatesFromRawBank Registered output in TES" 
              << endmsg ;
  }
  
  LHCb::RawEvent* rawEvt = findFirstRawEvent();
  if ( !rawEvt ) 
    return Error("No valid raw event location found",StatusCode::SUCCESS,50);
  
  const std::vector<LHCb::RawBank*>& banks = 
    rawEvt -> banks( LHCb::RawBank::L0Calo );
  // check presence of error bank
  const std::vector< LHCb::RawBank* > * errBanks = 
    &rawEvt -> banks( LHCb::RawBank::L0CaloError ) ;
  
  if ( ( 0 != errBanks ) && ( 0 != errBanks -> size() ) ) 
  {
    std::vector< LHCb::RawBank * >::const_iterator it ;
    for ( it = errBanks->begin() ; errBanks -> end() != it ; ++it ) 
    {
      readoutStatus.addStatus( (*it) -> sourceID() , 
                               LHCb::RawBankReadoutStatus::ErrorBank ) ;
    }
  }
  
  if ( 0 == banks.size() ) 
  {
    Error( "L0Calo Bank has not been found" ).ignore() ;
    readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Missing ) ;
    readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::Missing ) ;      
  } 
  else 
  {
    int sourceZero( 0 ) , sourceOne( 0 ) ;
    
    // convert the banks to two arrays of ints.
    data.reserve( banks.size() ) ;
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); 
          banks.end() != itBnk; ++itBnk ) 
    {
      if ( LHCb::RawBank::MagicPattern != (*itBnk) -> magic() ) 
      {
        Error( "L0Calo Bank source has bad magic pattern" ).ignore() ;
        readoutStatus.addStatus( (*itBnk) -> sourceID() , 
                                 LHCb::RawBankReadoutStatus::Corrupted ) ;
        continue ;
      }

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
  
  m_convertTool -> convertRawBankToTES( data, outFull , out , version , 
                                        readoutStatus ) ;  


  if ( writeOnTES() && m_statusOnTES ) {
    // Now put the status on TES also
    LHCb::RawBankReadoutStatuss * statuss = 
      getOrCreate< LHCb::RawBankReadoutStatuss , LHCb::RawBankReadoutStatuss > 
      ( LHCb::RawBankReadoutStatusLocation::Default ) ;
    
    LHCb::RawBankReadoutStatus * status = 
      statuss -> object( readoutStatus.key() ) ;
    
    if ( 0 == status ) {
      status = new LHCb::RawBankReadoutStatus( readoutStatus ) ;
      statuss -> insert( status ) ;
    } else {
      // merge both status if already exists
      if ( status -> status() != readoutStatus.status() ) {
        std::map< int const, long >::iterator it ;
        for ( it = readoutStatus.statusMap().begin() ; 
              it != readoutStatus.statusMap().end() ; ++it ) {
          status -> addStatus( (*it).first , (*it).second ) ;
        }
      }
    }    
  }
  
  if ( writeProcData() ) {
    // Write processor data for L0DU if requested
    // Save the candidates in CaloProcessor data location (for L0DU) 
    LHCb::L0ProcessorDatas* L0Calo = new LHCb::L0ProcessorDatas() ;
    put( L0Calo, LHCb::L0ProcessorDataLocation::L0Calo ) ;

    LHCb::L0CaloCandidates::iterator it ;
    for ( it = out -> begin() ; it != out -> end() ; ++it ) {
      L0Candidate cand( (*it) ) ;
      cand.saveCandidate( fiberType( (*it) -> type() ) , L0Calo ) ;
    }    
  }

  if ( !writeOnTES() ) {
    delete outFull;
    delete out;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
