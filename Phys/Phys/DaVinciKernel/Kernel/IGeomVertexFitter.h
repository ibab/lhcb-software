// $Id: IGeomVertexFitter.h,v 1.1 2005-01-06 10:30:43 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IGEOMVERTEXFITTER_H 
#define DAVINCIKERNEL_IGEOMVERTEXFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;
class IParticlePropertySvc;

static const InterfaceID IID_IGeomVertexFitter("IGeomVertexFitter", 2 , 0); 

/** @class IGeomVertexFitter
 *  Interface Class for geometrical constrained vertex fit of the mother
 *    particle and for mass constrained fit of the daughter(s) in case it is
 *    a ressonance.
 *
 *  @author J. de Mello, C. Nunes. 
 *          based on Fortran code by J. Saborido.
 *  @date   26/02/2002
 */

class IGeomVertexFitter : virtual public IAlgTool {
  

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGeomVertexFitter; }
  
  /// Perform geometrical contrained vertex fit for 2, 3 or 4 tracks without 
  /// ressonance.
  /// inputs: string name of the mother particle to contrain to
  ///         vector of particles to fit to a vertex
  /// output: resulting Vertex and Particle
  virtual StatusCode fitWithGeom(const std::string&,const ParticleVector&,
                                 Vertex&, Particle&) = 0; 
  
  /// Perform geometrical contrained vertex fit for 2 tracks without 
  /// ressonance.
  /// inputs: string name of the mother particle to contrain to
  ///         two Particles to fit to a vertex
  /// output: resulting Vertex and Particle
  virtual StatusCode fitWithGeom(const std::string&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0;
  
  /// Perform geometrical contrained vertex fit for 3 tracks without 
  /// ressonance.
  /// inputs: string name of the mother particle to contrain to
  ///         three Particles to fit to a vertex
  /// output: resulting Vertex and Particle   
  virtual StatusCode fitWithGeom(const std::string&,
                                 Particle&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0; 
  
  /// Perform geometrical contrained vertex fit for 4 tracks without 
  /// ressonance.
  /// inputs: string name of the mother particle to contrain to
  ///         three Particles to fit to a vertex
  /// output: resulting Vertex and Particle   
  virtual StatusCode fitWithGeom(const std::string&,
                                 Particle&, Particle&, Particle&, Particle&,
                                 Vertex&, Particle&) = 0;
	
	
  /// Perform contrained vertex fit for 2 tracks considering
  /// the cases of one ressonance and two ressonances.
  /// inputs: string name of the mother particle to contrain to.
  ///         two Particles with one or both of them being ressonances. 
  /// output: resulting Vertex and Particle  
	
  virtual StatusCode fitWithGeomAndSubMass(const std::string&, Particle&, 
                                           Particle&,
                                           Vertex&, Particle&) = 0;
	
	
  /// Perform contrained vertex fit for 3 tracks considering
  ///the case of one ressonance.
  /// inputs: string name of the mother particle to contrain to.
  ///         three Particles with one of them being a ressonance.
  /// output: resulting Vertex and Particle 	
	
  virtual StatusCode fitWithGeomAndSubMass(const std::string&,
                                           Particle&, Particle&, Particle&,
                                           Vertex&, Particle&) = 0; 	
	
	
  /// Perform mass contrained vertex fit for 2 or 3 tracks considering
  /// the cases of one ressonance and two ressonances.
  /// inputs: string name of the mother particle to contrain to.
  ///         vector of particles with one or two of them being a ressonance.
  /// output: resulting Vertex and Particle  	
	
  virtual StatusCode fitWithGeomAndSubMass(const std::string&,
                                           const ParticleVector&,
                                           Vertex&, Particle&) = 0; 
	
  
  
};

#endif // DAVINCIKERNEL_IGEOMVERTEXFITTER_H
