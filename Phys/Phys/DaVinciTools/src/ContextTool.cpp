// $Id: ContextTool.cpp,v 1.13 2009-11-05 13:50:40 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IToolSvc.h" 

// from Event model
#include "Event/RecVertex.h" 
// from DaVinci
#include "Kernel/DVAlgorithm.h" 
#include "Kernel/IOnOffline.h" 
// local
#include "ContextTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ContextTool
//
// 2005-06-08 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ContextTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ContextTool::ContextTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_desktop(0)
  , m_dist(0)
  , m_fitter(0)
  , m_onOffline(0){
  declareInterface<IContextTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
ContextTool::~ContextTool() {}; 

//=============================================================================
// initialize
//=============================================================================
StatusCode ContextTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc ;
  
  return getTools();
   
}

//=========================================================================
// get parent DVAlgorithm
//=========================================================================
const DVAlgorithm*  ContextTool::getDVAlgorithm ( )const {
  const DVAlgorithm* dvalgo = 0 ;
  const IAlgTool* atool = this ;
  // get last tool in chain
  if (msgLevel(MSG::DEBUG)) debug() << "Looking for parents of " << atool->name() << endmsg ;
  while ( 0!=dynamic_cast<const IAlgTool*>(atool->parent())){
    atool = dynamic_cast<const IAlgTool*>(atool->parent());
    if (msgLevel(MSG::VERBOSE)) verbose() << "... tool is owned by tool " << atool->name() << endmsg ;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "null. Checking for toolsvc with " << atool<< endmsg ;
  // check it's not the ToolSvc
  const IToolSvc* tsvc = dynamic_cast<const IToolSvc*>( atool->parent() );
  if ( 0!=tsvc ){
    warning() << "Parent of " << atool->name() << " is the ToolSvc." << endmsg ;
    warning() << "OnOffline tool will return default tools" << endmsg ;
  } else {
    // check if it is an algorithm
    if (msgLevel(MSG::VERBOSE)) verbose() << "Not toolsvc. Checking for DVA with " << atool << endmsg ;
    dvalgo = dynamic_cast<const DVAlgorithm*>( atool->parent() );
    if (0==dvalgo) warning() << "parent of " << name() << " is not a DVAlgorithm" << endmsg ;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "DVA is " << dvalgo << endmsg ;
  
  return dvalgo ;
}
//=============================================================================
// get tools
//=============================================================================
StatusCode ContextTool::getTools(){

  if (msgLevel(MSG::DEBUG)) debug() << "getTools()" << endmsg ;
  const DVAlgorithm* dvalgo = getDVAlgorithm() ;

  if (0!=dvalgo){
    if (msgLevel(MSG::DEBUG)) debug() << name() << " is owned by " << dvalgo->name() << endmsg ;
    m_desktop = dvalgo->desktop() ;
    if (0==m_desktop) {
      err() << "No desktop found" << endmsg;
      return StatusCode::FAILURE;
    } else if (msgLevel(MSG::DEBUG)) debug() << "Desktop of parent DVAlgorithm found" << endmsg ;
    
    m_dist = dvalgo->distanceCalculator() ;
    if (0==m_dist) {
      err() << "No DistanceCalculator found" << endmsg;
      return StatusCode::FAILURE;
    } else if (msgLevel(MSG::DEBUG)) debug() << "distanceCalculator of parent DVAlgorithm found" << endmsg ;
    
    m_fitter = dvalgo->vertexFitter() ;
    if (0==m_fitter) {
      err() << "No Vertex Fitter found" << endmsg;
      return StatusCode::FAILURE;
    } else if (msgLevel(MSG::DEBUG)) debug() << "Vertex Fitter of parent DVAlgorithm found" << endmsg ;
  }
  
  // now get default tools if needed 
  if ( 0==m_fitter ) m_fitter = tool<IVertexFit>(m_onOffline->vertexFitterType(),this);
  if ( 0==m_onOffline ) m_onOffline = tool<IOnOffline>("OnOfflineTool",this);
  if ( 0==m_dist ) m_dist = tool<IDistanceCalculator>("DistanceCalculator",this);
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// get PVs
//=============================================================================
const LHCb::RecVertex::Container* ContextTool::primaryVertices() const {
  if (0!=m_desktop){
    if (msgLevel(MSG::DEBUG)) debug() << "Getting PVs from desktop" << endmsg;
    return desktop()->primaryVertices();
  } else {
    
    LHCb::RecVertex::ConstVector PV ;
    verbose() << "Will get PVs from OnOffline tool at : " 
              << m_onOffline->primaryVertexLocation() << endmsg ;
    return (exist<LHCb::RecVertices>( m_onOffline->primaryVertexLocation() )) ?
      get<LHCb::RecVertices>( m_onOffline->primaryVertexLocation() ) : 0;
  }
  
}

//=============================================================================
// get the obsolete geom tool - load on demand only.
//=============================================================================
const IGeomDispCalculator* ContextTool::geomTool() const {

  const IGeomDispCalculator* obsoleteGeom = 0 ;
  const DVAlgorithm* dvalgo = getDVAlgorithm() ;
  if ( 0==dvalgo ){
    warning() << "Ancestor of " << name() 
              << " is not a DVAlgorithm. Gettting own instance of obsolete geometry tool." << endmsg ;
    obsoleteGeom = tool<IGeomDispCalculator>("GeomDispCalculator",this);
  } else {
    obsoleteGeom = dvalgo->obsoleteGeomDispCalculator() ;
    if (0==obsoleteGeom) {
      Exception("No geomDispCalculator found");
    } else {
      if (msgLevel(MSG::DEBUG)) debug() << "geomDispCalculator of parent DVAlgorithm found" << endmsg ;
    }
  }
  
  return obsoleteGeom;
}
