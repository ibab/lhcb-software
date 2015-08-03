// Include files 

#include<iostream>
#include<string>
#include<sstream>

// local
#include "L0DUMultiConfigProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUMultiConfigProvider
//
//  Provide  **ANY** predefined L0DU configuration according to
//  the Trigger Configuration Key
//  Configuration described in options file
//
// 2008-01-26 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUMultiConfigProvider )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUMultiConfigProvider::L0DUMultiConfigProvider( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_configs()
  , m_provider(NULL)
  , m_template()
{
  declareInterface<IL0DUConfigProvider>(this);
  //
  // preload Config in the initialization phase or load on-Demand
  declareProperty("Preload" , m_preload = false ); 
  declareProperty("registerTCK" , m_list );


  if( context().find("Check") != std::string::npos ){
    m_preload = true;
  }

}
//=============================================================================
// Destructor
//=============================================================================
L0DUMultiConfigProvider::~L0DUMultiConfigProvider() {} 

//=============================================================================


//============================================================================= 
// Initialize is the main method
//=============================================================================
StatusCode L0DUMultiConfigProvider::initialize(){
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Initialize L0DUMultiConfigProvider" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;


  // check TCK is uniquely defined
  for( std::vector<std::string>::iterator itck = m_list.begin() ; itck != m_list.end() ; ++itck){

    if( "0x" != (*itck).substr( 0, 2 ) ){
      error() << "TCK value " << *itck << " MUST be registered in hexadecimal format '0x" << *itck << "'" << endmsg;
      return StatusCode::FAILURE;
    }

    for( std::vector<std::string>::iterator jtck = itck+1 ; jtck != m_list.end() ; ++jtck){
      if( *itck == *jtck){
        error() << "The Trigger Configuration Key " << *itck << " is not unique " << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  

  std::string  slot = (rootInTES() == "") ? "T0" : rootInTES() ;

  // pre-load 'template' configuration
  m_template[slot] = loadConfig( LHCb::L0DUTemplateConfig::Name ,slot );
  //  info() << "preloading template for slot " << slot << " " << m_template[slot] << endmsg;

  // pre-load TCK configurations if requested (pre-loading for rootInTES slot only) 
  if( m_preload ){    
    for( std::vector<std::string>::iterator itck = m_list.begin() ; itck != m_list.end() ; ++itck){ 
      loadConfig( *itck , slot ); 
    }   
  } 

  return StatusCode::SUCCESS;
}
//============
// finalize 
//============
StatusCode L0DUMultiConfigProvider::finalize(){
  for(std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.begin();it!=m_configs.end();++it){
    LHCb::L0DUConfigs* configs = (*it).second;
    if( configs == NULL )continue;
    configs->release();
    delete configs;
 }
  m_configs.clear();
  return GaudiTool::finalize();
}

//=============================================================================
LHCb::L0DUConfig*  L0DUMultiConfigProvider::config( long tck ,std::string slot){

  if( slot == "") slot = "T0";

  // first : handle the 'template' configuration
  if( tck == LHCb::L0DUTemplateConfig::TCKValue ){
    if( NULL != m_template[slot])return m_template[slot];
    LHCb::L0DUConfig* temp = loadConfig( LHCb::L0DUTemplateConfig::Name ,slot );
    m_template[slot] = temp;
    return temp;
  }
  
  else if(tck < 0 || tck > 0xFFFF){
    warning() << "requested TCK " << tck << "is not a 16 bit word" << endmsg;
    return NULL;
  } 


  std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.find( slot );
  if( it != m_configs.end())
    if( NULL != m_configs[slot]->object(tck) ) return m_configs[slot]->object(tck);
  

  std::stringstream s("");
  s << "0x" << format("%04X", tck) ;
  
  return loadConfig( s.str() ,slot);

  
}
//=============================================================================
LHCb::L0DUConfig*  L0DUMultiConfigProvider::loadConfig( std::string tck ,std::string slot){
  
  if( slot == "") slot = "T0";

  // first : handle the 'template' configuration
  if( tck == LHCb::L0DUTemplateConfig::Name ){
    info() << "Creating the TEMPLATE configuration tools : " <<name() << "." << LHCb::L0DUTemplateConfig::Name 
           << " for time slot :  '" << slot << "'" << endmsg;
    m_provider = tool<IL0DUConfigProvider>("L0DUConfigProvider" , LHCb::L0DUTemplateConfig::Name ,this );
    LHCb::L0DUConfig* temp = m_provider->config( LHCb::L0DUTemplateConfig::TCKValue, slot ); // force template
    if(NULL == temp)error() << "Failed to load the TEMPLATE configuration"    << endmsg;
    return temp;
  }
  


  bool ok = false;
  for( std::vector<std::string>::iterator it = m_list.begin() ; it != m_list.end() ; ++it){
    if( tck == *it){ ok = true ; break;  }  
  }  
  if(!ok){
    Error("The requested TCK = " + tck + " is not registered ", StatusCode::SUCCESS ).ignore();
    return NULL;
  }

  std::stringstream s("");
  s <<  "TCK_" << tck ;
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Loading L0DUConfigProvider : " << s.str() << endmsg;
  m_provider = tool<IL0DUConfigProvider>("L0DUConfigProvider" , s.str(),this );


  int itck;
  int index = tck.rfind("0x") +2 ;
  std::string stck = tck.substr( index, tck.length() );
  std::istringstream is( stck.c_str() );
  is >> std::hex >> itck;

  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "TCK = '"<<tck <<"' -> decimal value = " << itck << endmsg ;

  LHCb::L0DUConfig* config = m_provider->config( itck , slot);

  if( NULL == config ){
    error() << "Failed to load configuration for TCK = " << tck 
            << " ... return empty LODUConfig" << endmsg;
    return config;
  }



  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "looking for configs for slot " << slot << " size = " << m_configs.size() << endmsg;
  std::map<std::string,LHCb::L0DUConfigs*>::iterator ic = m_configs.find( slot );
  if(ic != m_configs.end())debug() << "configs for slot " << slot << " found : " << (*ic).second << endmsg;
  if(ic == m_configs.end()){
    LHCb::L0DUConfigs* confs = new LHCb::L0DUConfigs();
    m_configs[slot]= confs;
  }
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "inserting config " << config << " in container " << m_configs[slot] << " for slot " << slot << endmsg;
  m_configs[slot]->insert( config );

  return  config;
}
