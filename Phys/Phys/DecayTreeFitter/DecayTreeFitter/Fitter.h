#ifndef DECAYTREEFITTER_FITTER_HH
#define DECAYTREEFITTER_FITTER_HH

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <vector>
#include "DecayTreeFitter/VtxErrCode.h"
#include "DecayTreeFitter/VtxFitStatus.h"
#include "DecayTreeFitter/Tree.h"

class VtxDoubleErr ;
class VtxLorentzVectorErr ;
class VtxFitParams ;

namespace LHCb
{
  class Particle ;
  class VertexBase ;
  class VtxDoubleErr ;
  class VtxFitParams ;
}

namespace DecayTreeFitter
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
    Fitter(const LHCb::Particle& bc, bool forceFitAll = true) ;
    Fitter(const LHCb::Particle& bc, const LHCb::VertexBase& pv, bool forceFitAll = true) ;
    ~Fitter() ;

    // Add or remove a mass constraint
    void setMassConstraint( const LHCb::Particle& cand, bool add=true ) ;
    
    // Add or remove a mass constraintfor a certain ParticleID
    void setMassConstraint( const LHCb::ParticleID& pid, bool add=true ) ;

    // Fit the decay tree
    void fit(int maxNumberOfIterations=10, double deltaChisquareConverged=0.01) ;
    
    // Fit just one step
    void fitOneStep() ;

    // Print the result of the fit
    void print() const ;

    // The top level particle that is fitted
    const LHCb::Particle* particle() const { return m_particle ; }

    // Currently the only accessor to the actual fitted data
    VtxFitParams fitParams(const LHCb::Particle& cand) const ;

    // Name of a particle in the decay tree
    std::string name(const LHCb::Particle& cand) const ;
    
    // Total chisquare
    double chiSquare() const { return m_chiSquare ; }

    // Total number of DOFs
    int nDof()      const ; 

    // Status of vertex fit
    int status() const { return m_status ; }

    // Number of iterations used by vertex fit
    int nIter() const { return m_niter ; }

    // Compute the decay length sum of two particles in the decay tree (useful for e.g. B->DD)
    VtxDoubleErr decayLengthSum(const LHCb::Particle&, const LHCb::Particle&) const ;

    static void setVerbose(int i) { vtxverbose = i ; }
    
    // methods to retrieve the result in terms of LHCb::Particles
    // (note: mother vertex is not updated, and decay length cannot be
    // stored anywhere. Use fitParams instead)
    LHCb::Particle getFitted() const ;
    LHCb::Particle getFitted(const LHCb::Particle& cand) const ;
    // reurn an updated decay tree. this is not a final solution. will
    // try to move more info to Particle
    Tree getFittedTree() const ;
    
    // update a particlular candidate in the tree
    bool updateCand(LHCb::Particle& cand) const ;
    bool updateTree(LHCb::Particle& cand) const ;
    
  protected:
    
    // expert interface. not yet for real consumption
    VtxFitParams fitParams(const ParticleBase& pb) const ;
    VtxDoubleErr decayLengthSum(const ParticleBase&,const ParticleBase&) const ;

    DecayChain* decaychain() { return m_decaychain ; }
    FitParams* fitparams() { return m_fitparams ; }
    const DecayChain* decaychain() const { return m_decaychain ; }
    const FitParams* fitparams() const { return m_fitparams ; }

    double globalChiSquare() const ;
    const ErrCode& errCode() { return m_errCode ; }
    
    // must be moved to derived class or so ...
    double add(const LHCb::Particle& cand) ; 
    double remove(const LHCb::Particle& cand) ;
    void updateIndex() ;

    //LHCb::Particle getFittedTree() const ;
    LHCb::Particle* fittedCand(const LHCb::Particle& cand, LHCb::Particle* headoftree) const ;
    
  private:
    const LHCb::Particle* m_particle ;
    DecayChain* m_decaychain ;
    FitParams* m_fitparams ;
    int m_status ;
    double m_chiSquare ;
    int m_niter ;
    ErrCode m_errCode ;
  } ;
}

#endif
