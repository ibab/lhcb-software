// $Id: RawBankReadoutStatusMonitor.cpp,v 1.3 2008-01-30 14:39:15 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Event/RawBank.h"
#include "Event/RawBankReadoutStatus.h"
#include "RawBankReadoutStatusMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankReadoutStatusMonitor
//
// 2008-01-23 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawBankReadoutStatusMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankReadoutStatusMonitor::RawBankReadoutStatusMonitor( const std::string& name,
                                                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
    ,m_counter(0)
{

  declareProperty( "bankTypes", m_bankTypes );
  declareProperty( "NumberOfSources", m_nSources=-1 );// Number Of Source in the StatusMap

}
//=============================================================================
// Destructor
//=============================================================================
RawBankReadoutStatusMonitor::~RawBankReadoutStatusMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RawBankReadoutStatusMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;


  setSplitHistoDir(true);
  setHistoDir( name() );

  m_degree = 0;

  int word = LHCb::RawBankReadoutStatus::Unknown;
  int k=1;
  int i=0;
  while( word != 0 ){
    LHCb::RawBankReadoutStatus::Status stat = (LHCb::RawBankReadoutStatus::Status) k;
    m_stat << i << "=" << stat << "/";
    m_degree++;
    word = word >> 1;
    k *= 2;
    i++;
  }
  info()  << "RawBankReadoutStatus meaning : " << m_stat.str() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankReadoutStatusMonitor::execute() {

  debug() << "==> Execute" << endmsg;


  // get RawBankReadoutStatus
  LHCb::RawBankReadoutStatuss* statuss;
  if( exist<LHCb::RawBankReadoutStatuss>( LHCb::RawBankReadoutStatusLocation::Default ) ){
    statuss = get<LHCb::RawBankReadoutStatuss>( LHCb::RawBankReadoutStatusLocation::Default );
  }else{
    Warning("No  RawBankReadoutStatus container found at " + LHCb::RawBankReadoutStatusLocation::Default );
    return StatusCode::SUCCESS;
  }
  
  if( 0 == statuss->size() ){
    Warning("The RawBankReadoutStatus container is empty *" + LHCb::RawBankReadoutStatusLocation::Default + ")");
    return StatusCode::SUCCESS;
  }
  
  m_counter++;
  
  
  for(LHCb::RawBankReadoutStatuss::iterator it = statuss->begin() ; it != statuss->end() ; ++it ){
    
    LHCb::RawBankReadoutStatus* status = *it;    
    std::map< int, long > statusMap = status->statusMap();
    
    LHCb::RawBank::BankType type = status->key();
    std::string typeName = LHCb::RawBank::typeName( type );
    
    bool ok = ( 0 == m_bankTypes.size() ) ? true :  false;
    for(std::vector<std::string>::iterator inam = m_bankTypes.begin() ; inam != m_bankTypes.end() ; ++inam){
      if( *inam == typeName){
        ok = true;
        break;
      }  
    }
    if( !ok )continue;
    
    
    
    std::stringstream base("");
    base << typeName << "/" ;    
    // 1D summary histogram for the whole bank
    std::stringstream tit1D("");
    tit1D << "Status summary for " << typeName << " bank " ;
    plot1D( -1., base.str() + "/1",  tit1D.str() ,  -1, (double) m_degree , m_degree+1);
    debug() << "Status " << status->status() << endreq;
    for(int i = 0; i < m_degree ; ++i ){
      int word = status->status() >> i;
      if( ( 0x1 & word) )plot1D( (double) i , base.str() + "/1",  tit1D.str() ,   -1, (double) m_degree , m_degree+1);
      int isok = (0x1 & word);
      debug() << i << " -> " <<  isok << endreq;
    }


    // 2D summary histogram as a function of the sourceID
    std::stringstream tit2D("");
    tit2D << "Status as a function of sourceID for " << typeName << " bank ";
    double min = -1.;
    double max = m_nSources;
    if( m_nSources <= 0){
      max = -1.;
      for( std::map< int, long >::iterator imap = statusMap.begin() ; imap != statusMap.end() ; ++imap ){
        if ( (double)(*imap).first > max)max= (double)(*imap).first;
      }
    }
    max += 1;
    int bin = (int) max + 1;


    for( std::map< int, long >::iterator imap = statusMap.begin() ; imap != statusMap.end() ; ++imap ){
      double source = (double)(*imap).first;
      plot2D(  source, -1.,  base.str() + "/2",  
               tit2D.str() , min, max, -1., (double) m_degree+1. , bin , m_degree+2 );
      for(int i = 0; i < m_degree ; ++i ){
        int word = (*imap).second >> i;
        if( ( 0x1 & word) )plot2D(  source , (double) i , base.str() + "/2",  
                                    tit2D.str() , min, max, -1., (double) m_degree , bin , m_degree+1 );


      }
    }
  }
  return StatusCode::SUCCESS;
}
  
//=============================================================================
//  Finalize
//=============================================================================
StatusCode RawBankReadoutStatusMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
