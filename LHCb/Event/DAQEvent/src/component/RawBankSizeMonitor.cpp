// $Id: RawBankSizeMonitor.cpp,v 1.5 2009-04-14 12:12:35 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "RawBankSizeMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankSizeMonitor
//
// 2008-03-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawBankSizeMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankSizeMonitor::RawBankSizeMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
   declareProperty( "bankNames"   , m_bankNames );
   declareProperty( "MaxSizeMap"  , m_max);
   declareProperty( "MaxSizeDef"  , m_def=500);
   declareProperty( "Bins"        , m_bin=100);
   declareProperty( "Profile"     , m_prof=true);

   // default bank types list == all banks !
   for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++){
     std::string name = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
     m_bankNames.push_back( name );
   }

  setHistoDir( name );

}
//=============================================================================
// Destructor
//=============================================================================
RawBankSizeMonitor::~RawBankSizeMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RawBankSizeMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  // convert bankNames to bankTypes
  for(std::vector<std::string>::iterator it = m_bankNames.begin();it!=m_bankNames.end();it++){
    bool found = false;
    for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++){
      std::string name = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
      if( name == *it){
        found = true;
        m_bankTypes.push_back( (LHCb::RawBank::BankType) i );        
      }
    }
    if( !found) warning() << "Requested bank '" << *it << "' is not a valid name" << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankSizeMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  // get RawEvent
  if( exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default ) ){
    m_rawEvt= get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  }else  {
    Warning( "rawEvent not found at location '" + rootInTES() + LHCb::RawEventLocation::Default ).ignore();
    return StatusCode::SUCCESS;
  }
  // Loop over banks
  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++){
    
    std::string name = LHCb::RawBank::typeName( *it );
    
    std::map<std::string , int>::const_iterator im = m_max.find( name );
    int max = m_def;
    if( im != m_max.end() )max = (*im).second;
    
    
    const std::vector<LHCb::RawBank*>* banks= &m_rawEvt->banks(  *it );
    std::map<int,int> size;
    int s_min = 9999;
    int s_max = -1;
    
    for(std::vector<LHCb::RawBank*>::const_iterator ib = banks->begin();ib!=banks->end();ib++){      
      int source   = (*ib)->sourceID();
      if( s_min > source )s_min=source;
      if( s_max < source )s_max=source;
      size[source] = (*ib)->size();
    }

    for(std::map<int,int>::iterator is = size.begin();is!=size.end();is++){
      if(m_prof){
        profile1D( (double) (*is).first , (double) (*is).second
                   , name + "/1" 
                   , name + " bank size (bytes) as a function of sourceID (profile)"
                   , s_min , s_max+1, s_max-s_min+1);
      }else{
        plot2D( (double) (*is).first , (double) (*is).second
                , name + "/2" 
                , name + " bank size (bytes) as a function of sourceID"
                , s_min , s_max+1
                , 0. , (double) max 
                , s_max-s_min+1 , m_bin);
      } 
    }
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode RawBankSizeMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
