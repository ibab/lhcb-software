#ifndef VERTEXFIT_INEWDIRECTIONFITTER_H 
#define VERTEXFIT_INEWDIRECTIONFITTER_H 1
// ============================================================================

#include <string>

#include "GaudiKernel/IAlgTool.h"

#include "CLHEP/Matrix/SymMatrix.h"

static const InterfaceID IID_INewDirectionFitter("INewDirectionFitter", 1 , 1);

/** @class INewDirectionFitter
 *  New Interface Class for lifetime constrained vertex fit.
 *
 *  @author Yuehong Xie
 *  @date   20/04/2005
 */


class INewDirectionFitter : virtual public IAlgTool
{
public:  

  virtual StatusCode fit
  ( const Vertex& primary ,
    Particle&     particle,
    double & time, double & mass, 
    HepSymMatrix& cov,
    double& chisq ) const = 0 ;

  virtual StatusCode constrainMassForTime 
  ( double & time, double & mass,
    HepSymMatrix& cov,
    double newMass ) const = 0 ;


public: 
  
  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_INewDirectionFitter; }
  
protected:
  
  // virtual and protected destructor 
  //virtual ~INewDirectionFitter() ; ///< virtual and protected destructor
  
};

// ============================================================================
#endif // VERTEXFIT_INEWDIRECTIONFITTER_H
