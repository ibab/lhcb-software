// $Id: RelatedPV.cpp,v 1.1 2006-09-14 18:03:21 pkoppenb Exp $
// Include files
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IToolSvc.h"
#include "Kernel/DVAlgorithm.h"
// local
#include "RelatedPV.h"
//-----------------------------------------------------------------------------
// Implementation file for class : RelatedPV
//
// 2006-09-12 : Patrick Koppenburg
//-----------------------------------------------------------------------------

typedef LHCb::RecVertex::ConstVector::const_iterator rv_iter;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RelatedPV );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelatedPV::RelatedPV( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_desktop(0)
  , m_geom(0)
{
  declareInterface<IRelatedPV>(this);  
  declareProperty("SelectByClosestZ", m_closestZ = true );
  declareProperty("SelectByClosest", m_closest = false );
  declareProperty("SelectBySmallestIP", m_smallestIP = false );
  declareProperty("SelectBySignificance", m_significance = true );
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

  if ( m_closest+m_closestZ+m_smallestIP != 1 ){
    err() << "You must select only one of SelectByClosestZ: " << m_closestZ
          << ", SelectByClosest: " <<  m_closest
          << ", SelectBySmallestIP: " << m_smallestIP << endmsg ;
    return StatusCode::FAILURE ;
  }
  if ( m_significance ){
    if ( m_closestZ ) debug() << "The PV closest in Z/err will be chosen." << endmsg ;
    else if ( m_closest ) debug() << "The PV closest in 3D/err will be chosen." << endmsg ;
    else if ( m_smallestIP ) debug() << "The PV with smallest IP/err will be chosen." << endmsg ;
  } else {  
    if ( m_closestZ ) debug() << "The PV closest in Z will be chosen." << endmsg ;
    else if ( m_closest ) debug() << "The PV closest in 3D will be chosen." << endmsg ;
    else if ( m_smallestIP ) debug() << "The PV with smallest IP will be chosen." << endmsg ;
  }

  return getTools();
}
//=============================================================================
// get tools
//=============================================================================
StatusCode RelatedPV::getTools(){
  // look for desktop
  const IAlgTool* tool = this ;
  // get last tool in chain
  debug() << "Looking for parents of " << tool->name() << endmsg ;
  while ( NULL!=dynamic_cast<const IAlgTool*>(tool->parent())){
    tool = dynamic_cast<const IAlgTool*>(tool->parent());
    debug() << "... tool is owned by tool " << tool->name() << endmsg ;
  }
  // check it's not the ToolSvc
  const IToolSvc* tsvc = dynamic_cast<const IToolSvc*>( tool->parent() );
  if ( NULL!=tsvc ){
    err() << "Parent of " << tool->name() << " is the ToolSvc. RelatedPV *must* be private" << endmsg ;
    err() << "else it can only deal with association tables stored on the TES" << endmsg ;
    return StatusCode::FAILURE;
  }
  // check if it is an algorithm
  const DVAlgorithm* dvalgo = dynamic_cast<const DVAlgorithm*>( tool->parent() );
  if ( NULL==dvalgo ){
    err() << "Parent of " << tool->name() << " is not a DVAlgorithm" << endmsg ;
    return StatusCode::FAILURE;
  }
  debug() << tool->name() << " is owned by " << dvalgo->name() << endmsg ;
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
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Get all PVs
//=============================================================================
StatusCode RelatedPV::allPVs(const LHCb::Particle* p, LHCb::RecVertex::ConstVector& pvs, 
                             double minweight){
  if ( p==NULL ) {
    err() << "Passing NULL Particle" << endmsg ;
    return StatusCode::FAILURE ;
  }
  debug() << "Getting associated PVs of a " << p->particleID().pid() << " with weight > " 
           << minweight << endmsg ;
  pvs.clear();
  Particle2Vertex::Range r = desktop()->particle2Vertices(p);

  // if no range is found, fill it
  if ( r.empty() ) {
    StatusCode sc = buildRelations(p);
    if (!sc) return sc;
  }
  // try again
  r = desktop()->particle2Vertices(p); 

  // relations table is ordered
  for ( Particle2Vertex::Range::const_reverse_iterator i = r.rbegin() ; i!=r.rend() ; ++i){
    debug() << "Found a PV at " << i->to()->position() << " with weight " << i->weight() ;
    if ( i->weight() >= minweight ){
      debug() << " -> keeping it" <<  endmsg ;
      const LHCb::RecVertex* rv = dynamic_cast<const LHCb::RecVertex*>(i->to());
      if ( NULL==rv ){
        err() << "Vertex Base at " << i->to()->position() << " associated to a " 
              << p->particleID().pid() << " is not a RecVertex. isPrimary: " 
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
const LHCb::RecVertex* RelatedPV::bestPV(const LHCb::Particle* p){
  const LHCb::RecVertex* rv = dynamic_cast<const LHCb::RecVertex*>(desktop()->relatedVertex(p));
  if ( NULL==rv ) {
    debug() << "Found no associated PV to particle " << p->particleID().pid() << endmsg ;
    StatusCode sc = buildRelations(p);
    if (!sc) Exception("Error from buildRelations()");
  } else debug() << "Found an associated vertex to particle " << p->particleID().pid() << endmsg ; 
  // try again
  rv = dynamic_cast<const LHCb::RecVertex*>(desktop()->relatedVertex(p));
  return rv ;
}
//=============================================================================
/// Build relations table and store it in desktop
//=============================================================================
StatusCode RelatedPV::buildRelations(const LHCb::Particle* p){
  if (NULL==p) return Error("Particle is NULL") ;
  debug() << "Building PV relations for " << p->particleID().pid() << endmsg ;
  // sanity check
  const LHCb::Vertex* v = p->endVertex() ;
  if ( (m_closestZ || m_closest) && (NULL==v)) 
    return Error("Cannot measure distances without vertex.");
  
  const LHCb::RecVertex::ConstVector pvs = desktop()->primaryVertices() ;

  double fom = 0;
  double err = 0 ;
  for ( rv_iter i = pvs.begin() ; i!=pvs.end() ; ++i){
    if ( m_closestZ ) {
      fom = fabs(v->position().z()-(*i)->position().z());
      if ( m_significance ) fom = fom/sqrt((*i)->covMatrix()(2,2)*(*i)->covMatrix()(2,2)
                                           + v->covMatrix()(2,2)*v->covMatrix()(2,2));
    } else if ( m_closest ) {
      m_geom->calcVertexDis(*v,*(*i),fom,err);
      if ( m_significance ) fom = fom/err ;
    } else if ( m_closest ) {
      m_geom->calcImpactPar(*p,*(*i),fom,err);
      if ( m_significance ) fom = fom/err ;
    }
    desktop()->relate(p,*i,1./fom); // use 1/fom as weight
    debug() << "... PV at " << (*i)->position() << " gets weight 1/" << fom << endmsg ;
  }

  return StatusCode::SUCCESS ;
}
