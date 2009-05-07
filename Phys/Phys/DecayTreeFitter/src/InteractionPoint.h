#ifndef VTK_BEAMSPOT_HH
#define VTK_BEAMSPOT_HH

#include "DecayTreeFitter/VtkInternalParticle.h"
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>

namespace vtxtreefit 
{

  class InteractionPoint : public InternalParticle
  {
  public:
    InteractionPoint(const LHCb::Particle& bc, bool forceFitAll) ;
    InteractionPoint(const LHCb::Particle& bc, bool forceFitAll, bool addupsilon) ; // the value of addupsilon is actually not used ;)

    virtual ~InteractionPoint() ;

    ErrCode initBeamSpot(const LHCb::Particle* bc) ;

    virtual int dim() const { return 3 ; } // (x,y,z)

    virtual ErrCode initPar1(FitParams*) ; 
    virtual ErrCode initPar2(FitParams*) ; 
    virtual ErrCode initCov(FitParams*) const ; 

    virtual int type() const { return kInteractionPoint ; }

    virtual double chiSquare(const FitParams* par) const ;
    
    ErrCode projectIPConstraint(const FitParams& fitpar, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams&, Projection&) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const ;
    
    virtual int posIndex() const { return index() ; }
    virtual int momIndex() const { return -1; }//daughters().front()->momIndex() ; } // terrible hack
    virtual int tauIndex() const { return -1; }
    virtual bool hasEnergy() const { return false ; }
    virtual std::string name() const { return "InteractionPoint" ; }

  private:
    bool m_constrainXY ;
    bool m_constrainXYZ ;
    HepVector m_ipPos ;       // interaction point position
    HepSymMatrix m_ipCov ;    // cov matrix
    HepSymMatrix m_ipCovInv ; // inverse of cov matrix
  } ;

}


#endif
