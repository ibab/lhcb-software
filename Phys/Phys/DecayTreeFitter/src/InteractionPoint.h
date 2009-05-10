#ifndef VTK_BEAMSPOT_HH
#define VTK_BEAMSPOT_HH

#include "ParticleBase.h"
#include <CLHEP/Matrix/Vector.h>
#include <CLHEP/Matrix/SymMatrix.h>

namespace LHCb
{
  class Vertex ;
}

namespace decaytreefit 
{

  class InteractionPoint : public ParticleBase
  {
  public:
    InteractionPoint(const LHCb::Vertex& ipvertex, const LHCb::Particle& daughter, bool forceFitAll) ;
    virtual ~InteractionPoint() ;

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

  private:
    bool m_constrainXY ;
    bool m_constrainXYZ ;
    HepVector m_ipPos ;       // interaction point position
    HepSymMatrix m_ipCov ;    // cov matrix
    HepSymMatrix m_ipCovInv ; // inverse of cov matrix
    ParticleBase* m_daughter ;
  } ;

}


#endif
