// $Id: OnOfflineTool.cpp,v 1.7 2006-07-26 12:10:16 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IToolSvc.h" 

// from Event model
#include "Event/RecVertex.h" 
// local
#include "OnOfflineTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OnOfflineTool
//
// 2005-06-08 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<OnOfflineTool>          s_factory ;
const        IToolFactory& OnOfflineToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OnOfflineTool::OnOfflineTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ){
  declareInterface<IOnOffline>(this);

  declareProperty( "OfflinePVLocation", m_offlinePVLocation = LHCb::RecVertexLocation::Primary);
  declareProperty( "OnlinePVLocation", m_onlinePVLocation = "/Event/Phys/Trg/Vertices" );
  declareProperty( "Online", m_online = false );

  declareProperty( "OfflineGeomTool", m_offlineGeomTool = "GeomDispCalculator" );
  declareProperty( "OnlineGeomTool", m_onlineGeomTool = "TrgDispCalculator" );

  declareProperty( "OfflineVertexFitter", m_offlineVertexFitter = "OfflineVertexFitter" );
  declareProperty( "OnlineVertexFitter", m_onlineVertexFitter = "TrgVertexFitter" );
  
}
//=============================================================================
// Destructor
//=============================================================================
OnOfflineTool::~OnOfflineTool() {}; 

//=============================================================================
// initialize
//=============================================================================
StatusCode OnOfflineTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc ;
  
  if ( context()=="HLT" ) {
    info() << "Running in HLT context: Online = true" << endmsg ;
    m_online = true ;
  } else if ( context()!="" ){
    warning() << "Running in " << context() << " context" << endmsg ;
  }
  
  // check it is not global
  const IToolSvc* par = dynamic_cast<const IToolSvc*>( this->parent() );
  if ( 0!=par ){
    err() << "Parent of OnOfflineTool is ToolSvc. OnOfflineTool *must* be private" << endmsg ;
    return StatusCode::FAILURE;
  }

  return sc;
   
}

//=============================================================================
// get PV
//=============================================================================
std::string OnOfflineTool::getPVLocation(void) const {
  if ( m_online ) return m_onlinePVLocation ;
  else return m_offlinePVLocation ;
};
//=============================================================================
// 
//=============================================================================
std::string OnOfflineTool::dispCalculator() const {
  if ( m_online ) return m_onlineGeomTool ;
  else return m_offlineGeomTool ;
};
//=============================================================================
// 
//=============================================================================
std::string OnOfflineTool::vertexFitter() const {
  if ( m_online ) return m_onlineVertexFitter ;
  else return m_offlineVertexFitter ;
};
