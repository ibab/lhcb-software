// $Id: IMassVertexFitter.h,v 1.1.1.1 2004-08-24 06:59:45 pkoppenb Exp $
#ifndef DAVINCITOOLS_IMASSVERTEXFITTER_H 
#define DAVINCITOOLS_IMASSVERTEXFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;
class IParticlePropertySvc;

static const InterfaceID IID_IMassVertexFitter("IMassVertexFitter", 2 , 1);

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
  
	
  /// Perform mass contrained vertex fit for 2 tracks.
  /// inputs: string name of the mother particle to contrain to.
  ///         two Particles. 
  /// output: resulting Vertex and Particle  
	
  virtual StatusCode fitWithMass(const std::string&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0;
	
	
  /// Perform mass contrained vertex fit for 3 tracks 
  /// inputs: string name of the mother particle to contrain to.
  ///         three Particles 
  /// output: resulting Vertex and Particle 	
	
  virtual StatusCode fitWithMass(const std::string&,
                                 Particle&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0; 	
	
  
  /// Perform mass contrained vertex fit for 4 tracks 
  /// inputs: string name of the mother particle to contrain to.
  ///         three Particles 
  /// output: resulting Vertex and Particle 	
	
  virtual StatusCode fitWithMass(const std::string&,
                                 Particle&, Particle&, Particle&,
                                 Particle&,Vertex&, Particle&) = 0;
  
  /// Perform mass contrained vertex fit for 2, 3 or 4 tracks 
  /// inputs: string name of the mother particle to contrain to.
  ///         vector of particles
  /// output: resulting Vertex and Particle  	
	
  virtual StatusCode fitWithMass(const std::string&,const ParticleVector&,
                                 Vertex&, Particle&) = 0; 
	
	
  
  
};

#endif // DAVINCITOOLS_IMASSVERTEXFITTER_H
