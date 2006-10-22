// $Id: ContextTool.cpp,v 1.2 2006-10-22 13:19:49 pkoppenb Exp $
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
static const  ToolFactory<ContextTool>          s_factory ;
const        IToolFactory& ContextToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ContextTool::ContextTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_desktop()
  , m_geom()
  , m_fitter(){
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
//=============================================================================
// get tools
//=============================================================================
StatusCode ContextTool::getTools(){
  // look for desktop
  const IAlgTool* atool = this ;
  // get last tool in chain
  debug() << "Looking for parents of " << atool->name() << endmsg ;
  while ( NULL!=dynamic_cast<const IAlgTool*>(atool->parent())){
    atool = dynamic_cast<const IAlgTool*>(atool->parent());
    debug() << "... tool is owned by tool " << atool->name() << endmsg ;
  }
  // check it's not the ToolSvc
  const IToolSvc* tsvc = dynamic_cast<const IToolSvc*>( atool->parent() );
  if ( NULL!=tsvc ){
    warning() << "Parent of " << atool->name() << " is the ToolSvc." << endmsg ;
    warning() << "OnOffline tool will return default tools" << endmsg ;
  } else {
    // check if it is an algorithm
    const DVAlgorithm* dvalgo = dynamic_cast<const DVAlgorithm*>( atool->parent() );
    if ( NULL==dvalgo ){
      warning() << "Parent of " << atool->name() << " is not a DVAlgorithm." << endmsg ;
      warning() << "OnOffline tool will get tools according to context ``" << context() 
                << "''" << endmsg ;
    } else {
      debug() << atool->name() << " is owned by " << dvalgo->name() << endmsg ;
      m_desktop = dvalgo->desktop() ;
      if (NULL==m_desktop) {
        err() << "No desktop found" << endmsg;
        return StatusCode::FAILURE;
      } else info() << "Desktop of parent DVAlgorithm found" << endmsg ;
      
      m_geom = dvalgo->geomDispCalculator() ;
      if (NULL==m_geom) {
        err() << "No geomDispCalculator found" << endmsg;
        return StatusCode::FAILURE;
      } else info() << "geomDispCalculator of parent DVAlgorithm found" << endmsg ;
      
      m_fitter = dvalgo->vertexFitter() ;
      if (NULL==m_geom) {
        err() << "No geomDispCalculator found" << endmsg;
        return StatusCode::FAILURE;
      } else info() << "geomDispCalculator of parent DVAlgorithm found" << endmsg ;
    }
  }
  m_onOffline = tool<IOnOffline>("OnOfflineTool",this);
  // now get default tools if needed 
  if ( NULL==m_geom )   m_geom = tool<IGeomDispCalculator>(m_onOffline->dispCalculator(),this);
  if ( NULL==m_fitter ) m_fitter = tool<IVertexFit>(m_onOffline->vertexFitter(),this);

  return StatusCode::SUCCESS;
}
//=============================================================================
// get PVs
//=============================================================================
LHCb::RecVertex::ConstVector ContextTool::primaryVertices() const {
  if (NULL!=m_desktop){
    debug() << "Getting PVs from desktop" << endmsg;
    return desktop()->primaryVertices();
  }
  LHCb::RecVertex::ConstVector PV ;
  verbose() << "Will get PVs from OnOffline tool at : " << m_onOffline->getPVLocation() << endmsg ;
  if ( !exist<LHCb::RecVertices>( m_onOffline->getPVLocation() ) ) return PV ; // no PV
  LHCb::RecVertices* verts = get<LHCb::RecVertices>( m_onOffline->getPVLocation() );
  for( LHCb::RecVertices::const_iterator ivert = verts->begin();
       ivert != verts->end(); ivert++ ) {
    verbose() << "    Vertex coordinates =  "
              << (*ivert)->position()  << ", Chi^2 = " << (*ivert)->chi2()
              << endmsg;
    PV.push_back(*ivert);
  }
  return PV ;
}

