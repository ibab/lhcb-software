// $Id: OnOfflineTool.cpp,v 1.18 2008-10-20 15:12:25 jpalac Exp $
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
  : 
  GaudiTool ( type, name , parent ),
  m_onlinePVLocation("Hlt/Vertex/PV2D"),
  m_offlinePVLocation(LHCb::RecVertexLocation::Primary),
  m_online(false),
  m_offlineDistTool("LoKi::DistanceCalculator"),
  m_onlineDistTool("LoKi::TrgDistanceCalculator"),
  m_offlineVertexFitter("OfflineVertexFitter" ),
  m_onlineVertexFitter("TrgVertexFitter"),
  m_offlinePVRelatorName("GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/P2PVWithIPChi2"),
  m_onlinePVRelatorName("GenericParticle2PVRelator__p2PVWithIPChi2_OnlineDistanceCalculatorName_/OnlineP2PVWithIPChi2")
{

  declareInterface<IOnOffline>(this);

  declareProperty( "OfflinePVLocation",    m_offlinePVLocation    );
  declareProperty( "OnlinePVLocation",     m_onlinePVLocation     );

  declareProperty( "Online",               m_online               );

  declareProperty( "OfflineDistanceTool",  m_offlineDistTool      );
  declareProperty( "OnlineDistanceTool",   m_onlineDistTool       );

  declareProperty( "OfflineVertexFitter",  m_offlineVertexFitter  );
  declareProperty( "OnlineVertexFitter",   m_onlineVertexFitter   );

  declareProperty( "OfflinePVRelatorName", m_offlinePVRelatorName );
  declareProperty( "OnlinePVRelatorName",  m_onlinePVRelatorName  );
  
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

  return sc;
   
}
//=============================================================================
const std::string& OnOfflineTool::primaryVertexLocation() const {
  return online() ? m_onlinePVLocation : m_offlinePVLocation ;
};
//=============================================================================
const std::string& OnOfflineTool::distanceCalculatorType() const {
  return online() ? m_onlineDistTool : m_offlineDistTool ;
};
//=============================================================================
const std::string& OnOfflineTool::relatedPVFinderType() const {
  return online() ? m_onlinePVRelatorName : m_offlinePVRelatorName ;
};

//=============================================================================
const std::string& OnOfflineTool::vertexFitterType() const {
  return online() ? m_onlineVertexFitter : m_offlineVertexFitter ;
};
//=============================================================================
