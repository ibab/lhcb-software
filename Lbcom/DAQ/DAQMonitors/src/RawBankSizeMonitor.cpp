// Include files

// local
#include "RawBankSizeMonitor.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TProfile.h"
//-----------------------------------------------------------------------------
// Implementation file for class : RawBankSizeMonitor
//
// 2008-03-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawBankSizeMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  RawBankSizeMonitor::RawBankSizeMonitor( const std::string& name,
                                          ISvcLocator* pSvcLocator)
    : GaudiHistoAlg ( name , pSvcLocator )
    , m_rawEvt(NULL)
{
  declareProperty( "bankNames"   , m_bankNames );
  declareProperty( "MaxSizeMap"  , m_max);
  declareProperty( "MaxSizeDef"  , m_def=12000);
  declareProperty( "Bins"        , m_bin=200);
  declareProperty( "Profile"     , m_prof=true);

  // default bank types list == all banks !
  for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++){
    const std::string _name = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
    m_bankNames.push_back( _name );
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
      const std::string _name = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
      if( _name == *it){
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
  m_rawEvt= getIfExists<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  if( NULL == m_rawEvt ){
    return Warning( "rawEvent not found at location '" + rootInTES() + LHCb::RawEventLocation::Default, StatusCode::SUCCESS );
  }
  // Loop over banks
  int index=0;
  int total=0;
  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++){

    std::string name = LHCb::RawBank::typeName( *it );

    std::map<std::string , int>::const_iterator im = m_max.find( name );
    int max = m_def;
    if( im != m_max.end() )max = (*im).second;


    const std::vector<LHCb::RawBank*>* banks= &m_rawEvt->banks(  *it );
    std::map<int,int> size;
    int s_min = 9999;
    int s_max = -1;
    int s_sum = 0;
    
    for(std::vector<LHCb::RawBank*>::const_iterator ib = banks->begin();ib!=banks->end();ib++){
      int source   = (*ib)->sourceID();
      if( s_min > source )s_min=source;
      if( s_max < source )s_max=source;      
      s_sum+=(*ib)->size();  // size in Bytes
      size[source] = (*ib)->size(); // size in Bytes 
    }
    counter(name + " total bank size") += s_sum;
    plot1D(s_sum,name+"/0","Overal "+name+" bank size",0,(double) max,m_bin);
    AIDA::IProfile1D* prof= profile1D((double)index,(double)s_sum,"global","bank syze (bytes) as a fonction of bankType",
              0.,(double)m_bankTypes.size(),m_bankTypes.size(),"s");
    Gaudi::Utils::Aida2ROOT::aida2root( prof )->GetXaxis()->SetBinLabel( index+1 , name.c_str() );
    total+=s_sum;

    for(std::map<int,int>::iterator is = size.begin();is!=size.end();is++){
      if(m_prof){
        profile1D( (double) (*is).first , (double) (*is).second
                   , name + "/1"
                   , name + " bank size (bytes) as a function of sourceID (profile)"
                   ,s_min,s_max+1,s_max-s_min+1,"s");
      }else{
        plot2D( (double) (*is).first , (double) (*is).second
                , name + "/2"
                , name + " bank size (bytes) as a function of sourceID"
                , s_min , s_max+1
                , 0. , (double) max
                , s_max-s_min+1 , m_bin);
      }
    }  
    index++;
  }
  counter("Total banks size") += total;
  return StatusCode::SUCCESS;
}

//=============================================================================
