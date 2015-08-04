// Include files 

// from Gaudi
#include "GaudiUtils/Aida2ROOT.h"
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
DECLARE_ALGORITHM_FACTORY( RawBankReadoutStatusMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankReadoutStatusMonitor::RawBankReadoutStatusMonitor( const std::string& name,
                                                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_first       ( true )
{

  declareProperty( "bankNames", m_bankTypes );
  declareProperty( "NumberOfSources", m_nSources=-1 );// Number Of Source in the StatusMap
  declareProperty( "axisLabels", m_labels );
  declareProperty( "xLabelOptions", m_lab ="" );

  // default labelling
  m_degree = 0;
  unsigned int word = 1;
  m_labels.push_back( "Counter" );
  while( word <= LHCb::RawBankReadoutStatus::Unknown ){
    LHCb::RawBankReadoutStatus::Status stat = (LHCb::RawBankReadoutStatus::Status) word;
    std::stringstream slabel("");
    slabel << stat ;
    m_labels.push_back( slabel.str() );
    m_degree++;
    word *= 2;
  }
  setHistoDir( name );
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

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_first = true;
  info() << "---- Histo bins label : " << endmsg  ;
  int k = -1;
  for( std::vector<std::string>::iterator i = m_labels.begin() ; i != m_labels.end() ; i++){
    info() <<  " | " << k << " = '" <<  *i  <<"' | " << endmsg;
    k++;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankReadoutStatusMonitor::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  // get RawBankReadoutStatus
  LHCb::RawBankReadoutStatuss* statuss = getIfExists<LHCb::RawBankReadoutStatuss>( LHCb::RawBankReadoutStatusLocation::Default );
  if( NULL == statuss ){
    Warning("No  RawBankReadoutStatus container found at " + LHCb::RawBankReadoutStatusLocation::Default ).ignore();
    for(std::vector<std::string>::iterator inam = m_bankTypes.begin() ; inam != m_bankTypes.end() ; ++inam){
      std::string typeName = *inam;
      std::stringstream base("");
      base << typeName << "/" ;    
      std::stringstream tit1D("");
      tit1D << "Status summary for " << typeName << " bank " ;
      AIDA::IHistogram1D* histo1D =
        plot1D( LHCb::RawBankReadoutStatus::MissingStatus , base.str() + "1",  tit1D.str() ,  -1, (double) m_degree, m_degree+1);
      plot1D( -1 , base.str() + "1",  tit1D.str() ,  -1, (double) m_degree, m_degree+1);
      setLabels1D( histo1D  );
    }
    return StatusCode::SUCCESS;
  }
  
  if( 0 == statuss->size() ){
    Warning("The RawBankReadoutStatus container is empty *" + LHCb::RawBankReadoutStatusLocation::Default + ")").ignore();
    for(std::vector<std::string>::iterator inam = m_bankTypes.begin() ; inam != m_bankTypes.end() ; ++inam){
      std::string typeName = *inam;
      std::stringstream base("");
      base << typeName << "/" ;    
      std::stringstream tit1D("");
      tit1D << "Status summary for " << typeName << " bank " ;
      AIDA::IHistogram1D* histo1D =
        plot1D( LHCb::RawBankReadoutStatus::MissingStatus , base.str() + "1",  tit1D.str() ,  -1, (double) m_degree, m_degree+1);
      plot1D( -1 , base.str() + "1",  tit1D.str() ,  -1, (double) m_degree, m_degree+1);
      setLabels1D( histo1D  );
    }
    return StatusCode::SUCCESS;
  }
  
  
  
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
    AIDA::IHistogram1D* histo1D =
      plot1D( -1., base.str() + "1",  tit1D.str() ,  -1, (double) m_degree , m_degree+1);
    setLabels1D( histo1D  );
    if( msgLevel(MSG::DEBUG) ) debug() << "Status " << status->status() << endmsg;
    for(int i = 0; i < m_degree ; ++i ){
      const int word = status->status() >> i;
      const int isok = (0x1 & word);
      if( isok )plot1D( (double) i , base.str() + "1",  tit1D.str() ,   -1, (double) m_degree , m_degree+1);
      if( msgLevel(MSG::DEBUG) ) debug() << i << " -> " <<  isok << endmsg;
    }


    // 2D summary histogram as a function of the sourceID
    std::stringstream tit2D("");
    tit2D << "Status as a function of sourceID for " << typeName << " bank ";
    double min = 0.;
    double max = m_nSources;
    if( m_nSources <= 0){
      max = -1.;
      for( std::map< int, long >::iterator imap = statusMap.begin() ; imap != statusMap.end() ; ++imap ){
        if ( (double)(*imap).first > max)max= (double)(*imap).first;
      }
    }
    max += 1;
    int bin = (int) max ;


    for( std::map< int, long >::iterator imap = statusMap.begin() ; imap != statusMap.end() ; ++imap ){
      double source = (double)(*imap).first;
      AIDA::IHistogram2D* histo2D =
        plot2D(  source, -1.,  base.str() + "2",  
                 tit2D.str() , min, max, -1., (double) m_degree , bin , m_degree+1 );
      setLabels2D( histo2D );
      for(int i = 0; i < m_degree ; ++i ){
        int word = (*imap).second >> i;
        if( ( 0x1 & word) )plot2D(  source , (double) i , base.str() + "2",  
                                    tit2D.str() , min, max, -1., (double) m_degree , bin , m_degree+1 );
      }
    }
  }
  if(m_first)m_first=false;
  return StatusCode::SUCCESS;
}
  
//=============================================================================
void RawBankReadoutStatusMonitor::setLabels1D( AIDA::IHistogram1D* histo ){  
  TH1D* th1d = Gaudi::Utils::Aida2ROOT::aida2root( histo );
  int k = 1;
  TAxis* xAxis = th1d->GetXaxis();
  for ( std::vector<std::string>::iterator i  = m_labels.begin() ; i != m_labels.end() ; i++) {
    xAxis->SetBinLabel( k , (*i).c_str() );
    k++;
  }
  xAxis->LabelsOption( m_lab.c_str() );
}
void RawBankReadoutStatusMonitor::setLabels2D( AIDA::IHistogram2D* histo ){  
  TH2D* th2d = Gaudi::Utils::Aida2ROOT::aida2root( histo );
  int k = 1;
  TAxis* yAxis = th2d->GetYaxis();
  for ( std::vector<std::string>::iterator i  = m_labels.begin() ; i != m_labels.end() ; i++) {
    yAxis->SetBinLabel( k , (*i).c_str() );
    k++;
  }
}
