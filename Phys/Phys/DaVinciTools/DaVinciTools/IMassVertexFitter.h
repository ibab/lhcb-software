#ifndef DAVINCITOOLS_IMASSVERTEXFITTER_H 
#define DAVINCITOOLS_IMASSVERTEXFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;
class IParticlePropertySvc;

static const InterfaceID IID_IMassVertexFitter("IMassVertexFitter", 1 , 0); 

/** @class IMassVertexFitter
 *  Perform a mass constrained fit
 *
 *  @author J. de Mello, C. Nunes 
 *          based on Fortran code by J. Saborido
 *  @date   26/02/2002
 */

class IMassVertexFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMassVertexFitter; }
  /// Calculate

  // inputs: string name of the mother particle
  //         vector of particles (2 or 3)
  // output: vertex
  virtual StatusCode
  lagrangeFitter(const std::string&,const ParticleVector&,
                 Vertex&,Particle&) = 0; 
};

#endif // DAVINCITOOLS_IMASSVERTEXFITTER_H
