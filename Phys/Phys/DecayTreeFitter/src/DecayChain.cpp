#include <algorithm>
#include "FitParams.h"
#include "ParticleBase.h"
#include "InteractionPoint.h"
#include "DecayChain.h"

namespace DecayTreeFitter
{
  
  extern int vtxverbose ;
  
  void
  DecayChain::printConstraints(std::ostream& os) const
  {
    os << "Constraints in decay tree: " << m_constraintlist.size() << std::endl ;
    for( ParticleBase::constraintlist::const_iterator 
	   it = m_constraintlist.begin() ;
	 it != m_constraintlist.end(); ++it) 
      it->print(os) ;
  }

  DecayChain::DecayChain(const LHCb::Particle& bc, const Configuration& config)
    : m_dim(0),m_mother(0),m_isOwner(true)
  {
    m_mother = ParticleBase::createParticle(bc,0,config) ;
    m_mother->updateIndex(m_dim) ;
    m_cand   = locate(bc) ;
  }
  
  DecayChain::DecayChain(const LHCb::Particle& bc, const LHCb::VertexBase& pv, const Configuration& config) 
    : m_dim(0),m_mother(0),m_isOwner(true)
  {
    m_mother = new InteractionPoint(pv,bc,config) ;
    m_mother->updateIndex(m_dim) ;
    m_cand   = locate(bc) ;
  }
  
  DecayChain::~DecayChain()
  { 
    if(m_mother && m_isOwner) delete m_mother ; 
  }

  void
  DecayChain::initConstraintList()
  {
    if( m_dim==0 ) m_mother->updateIndex(m_dim) ;
    m_constraintlist.clear() ;
    m_mother->addToConstraintList(m_constraintlist,0) ;

    // the order of the constraints is a rather delicate thing
    // std::sort(m_constraintlist.begin(),m_constraintlist.end()) ;

    // merge all non-lineair constraints
    m_mergedconstraintlist.clear() ;
    if(false) {
      m_mergedconstraint = MergedConstraint() ;
      for( ParticleBase::constraintlist::iterator it =  m_constraintlist.begin() ;
	   it != m_constraintlist.end(); ++it) {
	if( it->isLineair() ) m_mergedconstraintlist.push_back(&(*it)) ;
	else  m_mergedconstraint.push_back(&(*it)) ;
      }
      if( m_mergedconstraint.dim()>0 )
	m_mergedconstraintlist.push_back(&m_mergedconstraint) ;
    }
    if(vtxverbose>=2) {
      std::cout << "DecayChain::initConstraintList(): " << m_constraintlist.size() << " " 
		<< m_mergedconstraintlist.size()<< std::endl ;
      printConstraints() ;
    }
  }

  ErrCode
  DecayChain::init(FitParams& par) 
  {
    ErrCode status ;

    if( m_dim==0 ) m_mother->updateIndex(m_dim) ;
    if( par.dim() != m_dim ) par = FitParams(m_dim) ;

    // set everything to 0
    par.resetPar() ;
    status |= m_mother->initPar1(&par) ;

    // let the mother do it
    par.resetCov() ;
    status |= m_mother->initCov(&par) ;
   
    if(vtxverbose>=2) std::cout << "status after init: " << status << std::endl ;
 
    return status ;
  }

  ErrCode
  DecayChain::filter(FitParams& par, bool firstpass) 
  {
    ErrCode status ;
    if(m_constraintlist.empty()) initConstraintList() ;
    
    par.resetCov(1000) ;
    if(firstpass || !par.testCov()) status |= m_mother->initCov(&par) ;

    if( vtxverbose>=3 || (vtxverbose>=2&&firstpass) ) {
      std::cout << "DecayChain::filter, after initialization: "  << std::endl ; 
      m_mother->print(&par) ;
    }
    
    FitParams reference = par ;

    if(m_mergedconstraintlist.empty() ) {
      for( ParticleBase::constraintlist::const_iterator it = m_constraintlist.begin() ;
	   it != m_constraintlist.end(); ++it) {
	status |= it->filter(par,reference) ;
	if( vtxverbose>=2 && status.failure() ) {
	  std::cout << "status is failure after parsing constraint: " ;
	  it->print() ;
	}
      }
    } else {
      for( std::vector<Constraint*>::const_iterator it = m_mergedconstraintlist.begin() ;
	   it != m_mergedconstraintlist.end(); ++it) {
	status |= (*it)->filter(par,reference) ;
	if( vtxverbose>=2 && status.failure() ) {
	  std::cout << "status is failure after parsing constraint: " ;
	  (*it)->print() ;
	}
      }
    }
    
    if(vtxverbose>=3) std::cout << "VtkDecayChain::filter: status = " << status << std::endl ;
      
    return status ;
  }

  double 
  DecayChain::chiSquare(const FitParams* par) const
  {
    return m_mother->chiSquare(par) ;
  }

  const ParticleBase* 
  DecayChain::locate(const LHCb::Particle& bc) const {
    const ParticleBase* rc(0) ;
    // return m_mother->locate(bc) ;
    ParticleMap::const_iterator it = m_particleMap.find(&bc) ;
    if( it == m_particleMap.end() ) {
      rc = m_mother->locate(bc) ;
      // used to add every candidate here, but something goes wrong
      // somewhere. now only cache pointers that we internally reference.
      if(rc) m_particleMap[&(rc->particle())] = rc ;
    } else {
      rc = it->second ;
    }
    return rc ;
  }
  
  void
  DecayChain::setMassConstraint( const LHCb::Particle& bc, bool add)
  {
    ParticleBase* part = const_cast<ParticleBase*>(locate(bc)) ;
    if(part && part->setMassConstraint(add)) {
      m_dim = 0 ;
      m_constraintlist.clear() ;
    }
  }

  void
  DecayChain::setMassConstraint( const LHCb::Particle& bc, double mass)
  {
    ParticleBase* part = const_cast<ParticleBase*>(locate(bc)) ;
    if(part) { 
      part->setMassConstraint(mass) ;
      m_dim = 0 ;
      m_constraintlist.clear() ;
    }
  }
  
  void
  DecayChain::setMassConstraint( const LHCb::ParticleID& pid, bool add) 
  {
    ParticleBase::ParticleContainer particles ;
    m_mother->locate( pid, particles ) ;
    bool changed(false) ;
    for(ParticleBase::ParticleContainer::iterator it = particles.begin() ;
        it != particles.end() ; ++it)
    { changed |= (*it)->setMassConstraint(add) ; }
    //
    if (changed) { 
      m_dim = 0 ;
      m_constraintlist.clear() ; 
    }
  }
  
  int 
  DecayChain::index(const LHCb::Particle& bc) const {
    int rc = -1 ;
    const ParticleBase* base = locate(bc) ;
    if(base) rc = base->index() ;
    return rc ;
  }

  int 
  DecayChain::posIndex(const LHCb::Particle& bc) const {
    const ParticleBase* base = locate(bc) ;
    return base ? base->posIndex() : -1 ;
  }

  int 
  DecayChain::momIndex(const LHCb::Particle& bc) const {
    const ParticleBase* base = locate(bc) ;
    return base ? base->momIndex() : -1 ;
  }

  int 
  DecayChain::lenIndex(const LHCb::Particle& bc) const {
    const ParticleBase* base = locate(bc) ;
    return base ? base->lenIndex() : 0 ;
  }

  ChiSquare
  DecayChain::chiSquare( const LHCb::Particle& bc, const FitParams& fitpars ) const {
    const ParticleBase* base = locate(bc) ;
    ChiSquare chisq ;
    if( base ) {
      chisq = base->chiSquare(fitpars) ;
      // uhh: need to subtract the lifetime dof if this particle has a
      // mother with a different vertex
      if( base->mother() && base->posIndex()>=0 &&
	  base->posIndex() != base->mother()->posIndex() ) 
	chisq += ChiSquare(0,1) ;
    }
    return chisq ;
  }
}
