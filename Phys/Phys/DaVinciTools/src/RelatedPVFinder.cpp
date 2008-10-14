// $Id: RelatedPVFinder.cpp,v 1.8 2008-10-14 16:50:50 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Kernel/Particle2Vertex.h"
#include "Kernel/IDistanceCalculator.h"

// local
#include "RelatedPVFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelatedPVFinder
//
// 2006-10-22 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RelatedPVFinder );

typedef LHCb::RecVertices::const_iterator rv_iter;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelatedPVFinder::RelatedPVFinder( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : 
  GaudiTool ( type, name , parent ), 
  m_closestZ(false),
  m_closest(false),
  m_smallestIP(true),
  m_chi2(true)
{
  declareInterface<IRelatedPVFinder>(this);

  declareProperty("SelectByClosestZ", m_closestZ );
  declareProperty("SelectClosest", m_closest );
  declareProperty("SelectBySmallestIP", m_smallestIP );
  declareProperty("SelectByChi2", m_chi2 );

}
//=============================================================================
// Destructor
//=============================================================================
RelatedPVFinder::~RelatedPVFinder() {} 
//=============================================================================
// initialize
//=============================================================================
StatusCode RelatedPVFinder::initialize(){

  StatusCode sc = GaudiTool::initialize() ;

  if (!sc) return sc;

  if ( m_closest+m_closestZ+m_smallestIP != 1 ){
    err() << "You must select only one of SelectByClosestZ: " << m_closestZ
          << ", SelectClosest: " <<  m_closest
          << ", SelectBySmallestIP: " << m_smallestIP << endmsg ;
    return StatusCode::FAILURE ;
  }
  if ( m_chi2 ){
    if ( m_closestZ ) debug() << "The PV closest in Z/err will be chosen." << endmsg ;
    else if ( m_closest ) debug() << "The PV closest in 3D chi2 will be chosen." << endmsg ;
    else if ( m_smallestIP ) debug() << "The PV with smallest IP chi2 will be chosen." << endmsg ;
  } else {  
    if ( m_closestZ ) debug() << "The PV closest in Z will be chosen." << endmsg ;
    else if ( m_closest ) debug() << "The PV closest in 3D will be chosen." << endmsg ;
    else if ( m_smallestIP ) debug() << "The PV with smallest IP will be chosen." << endmsg ;
  }
  if ( m_closest || m_closestZ ) {
    warning() << "You have chosen spacial separation." << endmsg ;
    warning() << "This only works with particles with an endVertex." << endmsg ;
    warning() << "Use (default) IP distance for final state particles" << endmsg ;
  }  

  return StatusCode::SUCCESS;
}
//============================================================================
const Particle2Vertex::Relations RelatedPVFinder::relatedPVs(const LHCb::Particle* p,
                                                         const LHCb::RecVertex::Container& PVs,
                                                         const IDistanceCalculator* distanceCalculator) const 
{
  return relatedPVs<>(p, PVs.begin(), PVs.end(), distanceCalculator);
}
//============================================================================
const Particle2Vertex::Relations RelatedPVFinder::relatedPVs(const LHCb::Particle* p,
                                                         const LHCb::RecVertex::ConstVector& PVs,
                                                         const IDistanceCalculator* distanceCalculator) const 
{
  return relatedPVs(p, PVs.begin(), PVs.end(), distanceCalculator);
}
//============================================================================
const Particle2Vertex::Relations RelatedPVFinder::relatedPVs(const LHCb::Particle* p,
                                                         const std::string& PVLocation,
                                                         const IDistanceCalculator* distanceCalculator) const 
{
 
  LHCb::RecVertex::Container* PVs = get<LHCb::RecVertices>( PVLocation );
 
  if (0!=PVs) {
    return relatedPVs(p, PVs->begin(), PVs->end(), distanceCalculator);
  } else {
    Error("No LHcb::RecVertex::Container found at "+PVLocation).ignore();
  }
  return Particle2Vertex::Relations();
}
//============================================================================
template <typename Iter> 
const Particle2Vertex::Relations RelatedPVFinder::relatedPVs(const LHCb::Particle* p,
                                                         Iter begin,
                                                         Iter end,
                                                         const IDistanceCalculator* distanceCalculator ) const
{

  Particle2Vertex::Relations relations;

  if (0==p) {
    Error("Particle is 0").ignore() ;
    return Particle2Vertex::Relations();
  }

  StatusCode sc(StatusCode::SUCCESS);

  const LHCb::Vertex* v = p->endVertex() ;

  if ( (m_closestZ || m_closest) && (0==v)) {    
    Error("Cannot measure distances without vertex. You have been warned at initialisation!").ignore();
    return Particle2Vertex::Relations();
  }

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Building PV relations for " 
            << p->particleID().pid() << endmsg ;
  }

  double fom(0);
  double chi2(0);

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Looping over " << end-begin << " PVs" << endmsg ;
  }



  for ( Iter i = begin ; i != end ; ++i){
    if ( m_closestZ ) {
      fom = fabs(v->position().z()-(*i)->position().z());
      if ( m_chi2 ) fom = fom/sqrt((*i)->covMatrix()(2,2)*(*i)->covMatrix()(2,2)
                                   + v->covMatrix()(2,2)*v->covMatrix()(2,2));
      if (msgLevel(MSG::VERBOSE)) verbose() << "Closest Z PV at " << (*i)->position() << " fom " << fom << endmsg ;
    } else if ( m_closest ) {
      sc = distanceCalculator->distance(v,(*i),fom,chi2);
      if ( m_chi2 ) fom = chi2 ;
      if (msgLevel(MSG::VERBOSE)) verbose() << "Closest PV at " << (*i)->position() << " fom " << fom << endmsg ;
    } else if ( m_smallestIP ) {
      sc = distanceCalculator->distance(p,(*i),fom,chi2);
      if ( m_chi2 ) fom = chi2 ;
      if (msgLevel(MSG::VERBOSE)) verbose() << "Smallest IP PV at " << (*i)->position() << " fom " << fom << endmsg ;
    } else {
      Exception("None of all options") ;
    }
    if (sc) {
      relations.push_back(Particle2Vertex::Relation(p,*i, 1./fom ));
    }
    
    //    if (sc) sc = table->relate(p,*i,1./fom) ;

    if (msgLevel(MSG::DEBUG)) debug() << "... PV at " << (*i)->position() << " gets weight 1/" << fom << endmsg ;
  }

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Done relations for " << end-begin << " PVs" << endmsg ;
  }

  return relations ;

}
//============================================================================
