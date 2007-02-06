// $Id: RelatedPV.cpp,v 1.6 2007-02-06 10:17:30 pkoppenb Exp $
// Include files
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IToolSvc.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IContextTool.h"            // Interface
// local
#include "RelatedPV.h"
//-----------------------------------------------------------------------------
// Implementation file for class : RelatedPV
//
// 2006-09-12 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RelatedPV );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelatedPV::RelatedPV( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_context(0)
    , m_desktop(0)
{
  declareInterface<IRelatedPV>(this);  
}
//=============================================================================
// Destructor
//=============================================================================
RelatedPV::~RelatedPV() {} 

//=============================================================================
// initialize
//=============================================================================
StatusCode RelatedPV::initialize(){

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;

  m_context = tool<IContextTool>("ContextTool",this);
  m_desktop = m_context->desktop();
  if ( NULL==m_desktop ){
    err() << "Context tool does not provide a desktop. Tool probably not owned by a DVAlgorithm" << endmsg ;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Get all PVs
//=============================================================================
StatusCode RelatedPV::allPVs(const LHCb::Particle* p, LHCb::VertexBase::ConstVector& pvs, 
                             double minweight){
  if ( p==NULL ) {
    err() << "Passing NULL Particle" << endmsg ;
    return StatusCode::FAILURE ;
  }
  debug() << "Getting associated PVs of a " << p->particleID().pid() << " with weight > " 
           << minweight << endmsg ;
  pvs.clear();
  Particle2Vertex::Range r = desktop()->particle2Vertices(p);

  // relations table is ordered
  for ( Particle2Vertex::Range::const_reverse_iterator i = r.rbegin() ; i!=r.rend() ; ++i){
    debug() << "Found a PV at " << i->to()->position() << " with weight " << i->weight() ;
    if ( i->weight() >= minweight ){
      debug() << " -> keeping it" <<  endmsg ;
      const LHCb::VertexBase* rv = i->to();
      if ( NULL==rv ){
        err() << "Vertex Base at " << i->to()->position() << " associated to a " 
              << p->particleID().pid() << " is not a VertexBase. isPrimary: " 
              << i->to()->isPrimary() << endmsg ;
        return StatusCode::FAILURE ;
      }
      pvs.push_back(rv);
    } else {
      debug() << " -> discarding it" <<  endmsg ;
      break ; // no need to loop further
    } 
  }
      
  return StatusCode::SUCCESS ;
}
//=============================================================================
/// Returns the best related PV 
//=============================================================================
const LHCb::VertexBase* RelatedPV::bestPV(const LHCb::Particle* p){
  const LHCb::VertexBase* vb = desktop()->relatedVertex(p);
  return vb ;
}
