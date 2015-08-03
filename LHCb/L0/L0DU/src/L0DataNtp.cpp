// Include files 

// from Event
#include "Event/L0DUReport.h"

// local
#include "L0DataNtp.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DataNtp
//
// 2012-04-10 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DataNtp )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DataNtp::L0DataNtp( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_l0(NULL)
  , m_l0conf(NULL)
  , m_l0emu(NULL)
{
  declareProperty("DataList"   , m_list );
  declareProperty( "ScaledData",m_scale=false);
  declareProperty( "TCKList" , m_tcks );
  // Accepted/rejected data
  m_list.push_back("ALL");      // add all data but ...
  m_list.push_back("!*Status*"); // no status
  m_list.push_back("!*More*");   // no extra L0PU info
  m_list.push_back("!*CU*");     // no extra L0Muon
  m_list.push_back("!*M0*");     // ...
  m_list.push_back("!*M1*");
  m_list.push_back("!*M2*");
  m_list.push_back("!*M3*");
  m_list.push_back("!*M4*");
  m_list.push_back("!*M5*");
  m_list.push_back("!*M6*");
  m_list.push_back("!*M7*");
  m_list.push_back("!*Add*"); // no address

}
//=============================================================================
// Destructor
//=============================================================================
L0DataNtp::~L0DataNtp() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DataNtp::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_l0    = tool<IL0DUFromRawTool>("L0DUFromRawTool","L0DUFromRaw" ,this );
  m_l0emu = tool<IL0DUEmulatorTool>("L0DUEmulatorTool","L0DUEmulator",this);
  m_l0conf= tool<IL0DUConfigProvider>( "L0DUMultiConfigProvider","L0DUConfig");

  return StatusCode::SUCCESS;
} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DataNtp::execute() {

  Tuple ntp = nTuple(500,"L0DataNtp" ,CLID_ColumnWiseTuple);
  

  StatusCode sc;
  //===== Processing
  if(!m_l0->decodeBank())Warning("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();
  const std::map<std::string, std::pair<unsigned int,double> >&  datamap = m_l0->datas();
  for(std::map<std::string, std::pair<unsigned int,double> >::const_iterator it = datamap.begin();
      datamap.end() != it ; ++it){
    std::string name = it->first;
    int val = (it->second).first;
    double scale=(it->second).second;
    if( LHCb::CaloAlgUtils::StringMatcher( m_list , name )){
      if(m_scale)sc=ntp->column("L0Data_"+rename(name), double(val)*scale );
      else sc=ntp->column("L0Data_"+rename(name), val );
    }
  }


  // L0DU emulated Decision (incl. downscaling !!) Useful on MC data
  if( !m_tcks.empty() ){
    
    LHCb::L0ProcessorDatas* proDatas= m_l0->L0ProcessorDatas();
    for( std::vector<std::string>::iterator it=m_tcks.begin();m_tcks.end()!=it;++it ){
      std::string stck =  *it;
      std::istringstream istck( stck.c_str() );
      int itck;
      istck >> std::hex >> itck  ;
      int dec = 0;
      LHCb::L0DUConfig* config = m_l0conf->config( itck );
      if( NULL != config){
        StatusCode sc = m_l0emu->process(config, proDatas );
        if( sc.isSuccess() && config->emulatedDecision() )dec=1;
      }
      sc=ntp->column( "L0Emu_TCK"+stck, dec);
    }
  }
  

  sc=ntp->write();

  return sc; 
}


std::string L0DataNtp::rename(std::string name){
  std::string rname=name;
  if( std::string::npos != rname.find("(") ){
    int index = rname.find("(");
    rname.erase(index,1);
    rname.insert(index,"_");
  }
  if( std::string::npos != rname.find(")") ){
    int  index = rname.find(")");
    rname.erase(index,1);
  }
  return rname;
}

//=============================================================================



