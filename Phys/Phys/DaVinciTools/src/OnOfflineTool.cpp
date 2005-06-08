// $Id: OnOfflineTool.cpp,v 1.2 2005-06-08 16:41:44 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from Event model
#include "Event/Vertex.h" 
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

  declareProperty( "OfflinePVLocation", m_offlinePVLocation = VertexLocation::Primary);
  declareProperty( "OnlinePVLocation", m_onlinePVLocation = "/Event/Phys/Trg/Vertices" );
  declareProperty( "Online", m_online = false );
  declareProperty( "OfflineGeomTool", m_offlineGeomTool = "GeomDispCalculator" );
  declareProperty( "OnlineGeomTool", m_onlineGeomTool = "TrgDispCalculator" );
  declareProperty( "OfflineVertexFitter", m_offlineVertexFitter = "UnconstVertexFitter" );
  declareProperty( "OnlineVertexFitter", m_onlineVertexFitter = "TrgVertexFitter" );
  
}
//=============================================================================
// Destructor
//=============================================================================
OnOfflineTool::~OnOfflineTool() {}; 

//=============================================================================
// get PV
//=============================================================================
std::string OnOfflineTool::getPVLocation(void) const {
  if ( m_online ) return m_onlinePVLocation ;
  else return m_offlinePVLocation ;
};
//=============================================================================
// set Online
//=============================================================================
StatusCode OnOfflineTool::setOnline(const bool& online){
  m_online = online ;
  if ( m_online ) debug() << "Reset to online" << endmsg ;
  else debug() << "Reset to offline" << endmsg ;
  return StatusCode::SUCCESS ;
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
