#ifndef __VTKDECAYCHAIN_HH__
#define __VTKDECAYCHAIN_HH__

#include <map>
#include "ParticleBase.h"
#include "MergedConstraint.h"

namespace LHCb {
  class Particle ;
  class VertexBase ;
}

namespace DecayTreeFitter {

  class FitParams ;
  class ParticleBase ;

  class DecayChain
  {
  public:
    DecayChain() : m_mother(0) {}

    DecayChain(const LHCb::Particle& bc, const Configuration& config)  ;
    DecayChain(const LHCb::Particle& bc, const LHCb::VertexBase& pv, const Configuration& config)  ;

    ~DecayChain() ;
    
    int dim() const { return m_dim ; }

    void initConstraintList() ;
    ErrCode init(FitParams& par) ;
    ErrCode filter(FitParams& par, bool firstpass=true) ;
    double chiSquare(const FitParams* par) const ;

    ParticleBase* mother() { return m_mother ; }
    const ParticleBase* cand() { return m_cand ; }
    const ParticleBase* mother() const { return m_mother ; }
    const ParticleBase* locate(const LHCb::Particle& bc) const ;
    void locate( const LHCb::ParticleID& pid, ParticleBase::ParticleContainer& result ) ;
   
    int index(const LHCb::Particle& bc) const ;
    int posIndex(const LHCb::Particle& bc) const ;
    int momIndex(const LHCb::Particle& bc) const ;
    int lenIndex(const LHCb::Particle& bc) const ;
    void setOwner(bool b) { m_isOwner=b ;}
    int momIndex() const ;

    void printConstraints(std::ostream& os=std::cout) const ;
    void setMassConstraint( const LHCb::Particle& bc, bool add=true) ;
    void setMassConstraint( const LHCb::Particle& bc, double mass) ;
    void setMassConstraint( const LHCb::ParticleID& pid, bool add=true) ;
    void setMassConstraint( const LHCb::ParticleID& pid, double mass) ;
    
    ChiSquare chiSquare( const LHCb::Particle& bc, const FitParams& fitpars ) const ;
  private:
    int m_dim ;
    ParticleBase* m_mother ;     // head of decay tree
    const ParticleBase* m_cand ; // fit candidate (not same to mother in case of bs/be constraint)
    ParticleBase::constraintlist m_constraintlist ;
    std::vector<Constraint*> m_mergedconstraintlist ;
    MergedConstraint m_mergedconstraint ;
    typedef std::map<const LHCb::Particle*,const ParticleBase*> ParticleMap ;
    mutable ParticleMap m_particleMap ;
    bool m_isOwner ;
  } ;

}



#endif
