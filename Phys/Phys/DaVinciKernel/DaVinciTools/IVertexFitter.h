// $Id: IVertexFitter.h,v 1.1.1.1 2004-08-24 06:59:45 pkoppenb Exp $
#ifndef DAVINCITOOLS_IVERTEXFITTER_H
#define DAVINCITOOLS_IVERTEXFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

// Forward declarations

static const InterfaceID IID_IVertexFitter("IVertexFitter", 1 , 1); 

/** @class IVertexFitter IVertexFitter.h DaVinciTools/IVertexFitter.h 
 *  
 *  Unconstrained Vertex Fitter Interface
 *
 *  @author Sandra Amato
 *  @date   05/03/2002
 */

class IVertexFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IVertexFitter; }

  /// Method to fit the vertex given a vector of Particles
  virtual StatusCode fitVertex( const  ParticleVector&,  Vertex& ) = 0; 

  /// Method to fit a vertex between two given Particles
  virtual StatusCode fitVertex( Particle&, Particle&,  Vertex& ) = 0; 

  /// Method to fit a vertex between three given Particles
  virtual StatusCode fitVertex( Particle&, Particle&, Particle&, Vertex&) = 0; 

};

#endif // DAVINCITOOLS_IVERTEXFITTER_H
