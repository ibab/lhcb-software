// $Id: IMassVertexFitter.h,v 1.3 2002-05-15 23:10:17 gcorti Exp $
#ifndef DAVINCITOOLS_IMASSVERTEXFITTER_H 
#define DAVINCITOOLS_IMASSVERTEXFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;
class IParticlePropertySvc;

static const InterfaceID IID_IMassVertexFitter("IMassVertexFitter", 2 , 0); 

/** @class IMassVertexFitter
 *  Interface Class for mass constrained vertex fit.
 *
 *  @author J. de Mello, C. Nunes. 
 *          based on Fortran code by J. Saborido.
 *  @date   26/02/2002
 */

class IMassVertexFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMassVertexFitter; }

  /// Perform mass contrains vertex fit.
  /// inputs: string name of the mother particle to contrain to
  ///         vector of particles to fit to a vertex
  /// output: resulting Vertex and Particle
  virtual StatusCode fitWithMass(const std::string&,const ParticleVector&,
                                 Vertex&, Particle&) = 0; 

  /// Perform mass contrains vertex fit.
  /// inputs: string name of the mother particle to contrain to
  ///         two Particles to fit to a vertex
  /// output: resulting Vertex and Particle
  virtual StatusCode fitWithMass(const std::string&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0;

  /// Perform mass contrains vertex fit.
  /// inputs: string name of the mother particle to contrain to
  ///         three Particles to fit to a vertex
  /// output: resulting Vertex and Particle   
  virtual StatusCode fitWithMass(const std::string&,
                                 Particle&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0; 
};

#endif // DAVINCITOOLS_IMASSVERTEXFITTER_H
