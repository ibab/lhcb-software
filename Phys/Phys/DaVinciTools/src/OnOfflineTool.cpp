// $Id: OnOfflineTool.cpp,v 1.14 2008-07-10 15:03:24 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IToolSvc.h" 

// from Event model
#include "Event/RecVertex.h" 

#include "Kernel/IRelatedPVFinder.h"
// local
#include "OnOfflineTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OnOfflineTool
//
// 2005-06-08 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OnOfflineTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OnOfflineTool::OnOfflineTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_pvRelator(0){
  declareInterface<IOnOffline>(this);

  declareProperty( "OfflinePVLocation", m_offlinePVLocation = LHCb::RecVertexLocation::Primary);
  //  declareProperty( "OnlinePVLocation", m_onlinePVLocation = LHCb::RecVertexLocation::Velo3D );
  declareProperty( "OnlinePVLocation", m_onlinePVLocation = "Hlt/Vertex/PV2D" );

  declareProperty( "Online", m_online = false );

  declareProperty( "OfflineGeomTool", m_offlineGeomTool = "GeomDispCalculator" );
  declareProperty( "OnlineGeomTool", m_onlineGeomTool = "TrgDispCalculator" );

  /// @todo There is only one Distance Calculator presently
  declareProperty( "OfflineDistanceTool", m_offlineDistTool = "LoKi::DistanceCalculator" );
  declareProperty( "OnlineDistanceTool", m_onlineDistTool = "LoKi::DistanceCalculator" );

  declareProperty( "OfflineVertexFitter", m_offlineVertexFitter = "OfflineVertexFitter" );
  declareProperty( "OnlineVertexFitter", m_onlineVertexFitter = "TrgVertexFitter" );

  declareProperty( "OfflinePVRelatorName", m_offlinePVRelatorName = "RelatedPVFinder" );
  declareProperty( "OnlinePVRelatorName", m_onlinePVRelatorName = "RelatedPVFinder/OnlinePVFinder" );
  
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
    if (msgLevel(MSG::DEBUG)) debug() << "Running in HLT context: Online = true" << endmsg ;
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

  if ( m_online) {
    m_pvRelator = tool<IRelatedPVFinder>(m_onlinePVRelatorName); // not private
  } else {
    m_pvRelator = tool<IRelatedPVFinder>(m_offlinePVRelatorName); // not private    
  }
  sc = m_pvRelator->setDefaults(getPVLocation(),dispCalculator());

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
std::string OnOfflineTool::distanceCalculator() const {
  if ( m_online ) return m_onlineDistTool ;
  else return m_offlineDistTool ;
};
//=============================================================================
// 
//=============================================================================
std::string OnOfflineTool::vertexFitter() const {
  if ( m_online ) return m_onlineVertexFitter ;
  else return m_offlineVertexFitter ;
};
