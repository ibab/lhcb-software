// $Id: L0DUMultiConfigProvider.cpp,v 1.2 2008-02-07 17:24:52 odescham Exp $
// Include files 

#include<iostream>
#include<string>
#include<sstream>
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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
DECLARE_TOOL_FACTORY( L0DUMultiConfigProvider );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUMultiConfigProvider::L0DUMultiConfigProvider( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    ,m_configs()
{
  declareInterface<IL0DUConfigProvider>(this);
  //
  // preload Config in the initialization phase or load on-Demand
  declareProperty("Preload" , m_preload = false ); 
  declareProperty("registerTCK" , m_list );
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
  debug() << "Initialize L0DUMultiConfigProvider" << endreq;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;


  // check TCK is uniquely defined
  for( std::vector<std::string>::iterator itck = m_list.begin() ; itck != m_list.end() ; ++itck){

    if( "0x" != (*itck).substr( 0, 2 ) ){
      error() << "TCK value " << *itck << " MUST be registered in hexadecimal format '0x" << *itck << "'" << endreq;
      return StatusCode::FAILURE;
    }

    for( std::vector<std::string>::iterator jtck = itck+1 ; jtck != m_list.end() ; ++jtck){
      if( *itck == *jtck){
        error() << "The Trigger Configuration Key " << *itck << " is not unique " << endreq;
        return StatusCode::FAILURE;
      }
    }
  }
  


  // pre-load configurations if requested
  if( m_preload ){    
    for( std::vector<std::string>::iterator itck = m_list.begin() ; itck != m_list.end() ; ++itck){
      loadConfig( *itck );
    }  
  }
  return StatusCode::SUCCESS;
}
//============
// finalize 
//============
StatusCode L0DUMultiConfigProvider::finalize(){
  m_configs.release();

  return GaudiTool::finalize();
}

//=============================================================================
LHCb::L0DUConfig*  L0DUMultiConfigProvider::config( long tck ){

  if(tck < 0 || tck > 0xFFFF){
    warning() << "requested TCK " << tck << "is not a 16 bit word" << endreq;
    return NULL;
  } 


  if( NULL != m_configs.object(tck) ) return m_configs.object(tck);
  

  std::stringstream s("");
  s << "0x" << format("%04X", tck) ;
  
  return loadConfig( s.str() );

  
}
//=============================================================================
LHCb::L0DUConfig*  L0DUMultiConfigProvider::loadConfig( std::string tck ){
  bool ok = false;
  for( std::vector<std::string>::iterator itck = m_list.begin() ; itck != m_list.end() ; ++itck){
    if( tck == *itck){ ok = true ; break;  }
  
  }  
  if(!ok)warning() << "The requested TCK = " << tck << " is not registered " 
                   << " ... will try to load it with no garantee " << endreq;



  std::stringstream s("");
  s <<  "TCK_" << tck ;
  debug() << "Loading L0DUConfigProvider : " << s.str() << endreq;
  m_provider = tool<IL0DUConfigProvider>("L0DUConfigProvider" , s.str(),this );


  int itck;
  int index = tck.rfind("0x") +2 ;
  std::string stck = tck.substr( index, tck.length() );
  std::istringstream is( stck.c_str() );
  is >> std::hex >> itck;

  debug() << "TCK = '"<<tck <<"' -> decimal value = " << itck << endreq ;

  LHCb::L0DUConfig* config = m_provider->config( itck );

  if( NULL == config ){
    error() << "Failed to load configuration for TCK = " << tck 
            << " ... return empty LODUConfig" << endreq;
    return config;
  }

  m_configs.insert( config);
  
  //m_provider->release(); 
  //debug() << "L0DUConfigProvider " << tck << " released " << endreq;

  return  config;
}
