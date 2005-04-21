
#ifndef DAVINCIKERNEL_IKFFITTOOL_H
#define DAVINCIKERNEL_IKFFITTOOL_H 1

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

static const InterfaceID IID_IKFFitTool("IKFFitTool", 1 , 2 ); 

/** @class IKFFitTool IKFFitTool.h Kernel/IKFFitTool.h 
 *  
 *  Kalman Filter type kinematic fit interface
 *
 *  @author Yuehong Xie
 *  @date   01/10/2004
 */

class IKFFitTool : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IKFFitTool; }

  /// Method to refit a particle
  virtual StatusCode reFit( Particle&) = 0;

  /// Method to apply mass constraint to a Particle
  virtual StatusCode massConstrain( Particle&) = 0; 
 
  /// Method to get Particle parameters with energy
  virtual StatusCode getEParameter(Particle&, HepVector&, HepSymMatrix&) =0;

                                                                                
  /// Method to get Particle parameters with mass
  virtual StatusCode getMParameter(Particle&, HepVector&, HepSymMatrix&) =0;
                                                                                

  /// Method to get photon parameters: z, x,y,E  cov(x,y,E)
  virtual StatusCode getPhotonParameter(Particle&, double&,
                                HepVector&, HepSymMatrix&) =0;


  /// Method to determine if a particle is composite
  virtual bool isComposite(Particle&) =0 ;
                                                                                
  /// Method to return the z of the most upstream measurement of a non-composite charged particle
  virtual double zFirstMeasurement(Particle* part) =0 ;

};

#endif // DAVINCIKERNEL_IKFFITTOOL_H
