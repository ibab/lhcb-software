#ifndef __VTK_FITTER_HH__
#define __VTK_FITTER_HH__

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <vector>
#include "ErrCode.h"

#include "DecayTreeFitter/VtxFitStatus.h"

class VtxDoubleErr ;
class VtxLorentzVectorErr ;
class VtxFitParams ;

namespace LHCb
{
  class Particle ;
  class VtxDoubleErr ;
  class VtxFitParams ;
}

namespace vtxtreefit
{

  class DecayChain ;
  class FitParams ;
  class Upsilon ;
  class ParticleBase ;
  
  extern int vtxverbose ;

  typedef LHCb::VtxFitStatus FitStatus ;
  typedef LHCb::VtxDoubleErr VtxDoubleErr ;
  typedef LHCb::VtxFitParams VtxFitParams ;

  class Fitter
  {
  public:
    Fitter() : m_decaychain(0), m_fitparams(0) {} 
    Fitter(const LHCb::Particle& bc, double prec=0.01) ;
    ~Fitter() ;
    void fit() ;
    void print() const ;
    void printConstraints(std::ostream& os=std::cout) const ;
    const HepSymMatrix& cov() const ;
    const HepVector& par() const ;
    HepSymMatrix cov(const std::vector<int>& indexVec) const ;
    HepVector par(const std::vector<int>& indexVec) const ;
    //const DecayChain* decayChain() const { return m_decaychain; }
    int posIndex(const LHCb::Particle& bc) const ;
    int momIndex(const LHCb::Particle& bc) const ;
    int tauIndex(const LHCb::Particle& bc) const ;
    
    double chiSquare() const { return m_chiSquare ; }
    double globalChiSquare() const ;
    int    nDof()      const ; 
    int status() const { return m_status ; }
    int nIter() const { return m_niter ; }
    const ErrCode& errCode() { return m_errCode ; }
    
    // must be moved to derived class or so ...
    double add(const LHCb::Particle& cand) ; 
    double remove(const LHCb::Particle& cand) ;
    void updateIndex() ;
    void fitOneStep() ;
    void setMassConstraint( const LHCb::Particle& cand,
			    bool add=true ) ;

    // interface to beta
    VtxDoubleErr decayLength(const LHCb::Particle& cand) const ;
    VtxDoubleErr lifeTime(const LHCb::Particle& cand) const ;
    VtxDoubleErr decayLengthSum(const LHCb::Particle&, const LHCb::Particle&) const ;

    LHCb::Particle getFitted() const ;
    LHCb::Particle getFitted(const LHCb::Particle& cand) const ;
    LHCb::Particle getFittedTree() const ;
    LHCb::Particle* fittedCand(const LHCb::Particle& cand, LHCb::Particle* headoftree) const ;

    VtxFitParams fitParams(const LHCb::Particle& cand) const ;

    void updateCand(LHCb::Particle& cand) const ;
    void updateTree(LHCb::Particle& cand) const ;

    static void setVerbose(int i) { vtxverbose = i ; }

  public:
    VtxFitParams fitParams(const ParticleBase& pb) const ;
    VtxDoubleErr decayLength(const ParticleBase& pb) const ;
    VtxDoubleErr decayLengthSum(const ParticleBase&,const ParticleBase&) const ;

    DecayChain* decaychain() { return m_decaychain ; }
    FitParams* fitparams() { return m_fitparams ; }
    const DecayChain* decaychain() const { return m_decaychain ; }
    const FitParams* fitparams() const { return m_fitparams ; }
    const LHCb::Particle* bc() const { return m_bc ; }
    static VtxDoubleErr decayLength(const ParticleBase& pb, const FitParams& ) ;
  private:
    const LHCb::Particle* m_bc ;
    DecayChain* m_decaychain ;
    FitParams* m_fitparams ;
    int m_status ;
    double m_chiSquare ;
    int m_niter ;
    double m_prec ;
    ErrCode m_errCode ;
  } ;
}

#endif
