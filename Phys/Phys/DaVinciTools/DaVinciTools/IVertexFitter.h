// $Id: IVertexFitter.h,v 1.1 2002-03-27 20:27:46 gcorti Exp $
#ifndef DAVINCITOOLS_IVERTEXFITTER_H
#define DAVINCITOOLS_IVERTEXFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/Vertex.h"

// Forward declarations

static const InterfaceID IID_IVertexFitter("IVertexFitter", 1 ,
                                                  0); 

/** @class IVertexFitter IVertexFitter.h 
 *  DaVinciTools/IVertexFitter.h
 *  Unconstrained Vertex Fitter Interface
 *
 *  @author Sandra Amato
 *  @date   05/03/2002
 */

class IVertexFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IVertexFitter; }
  /// Method to fit the vertex
  virtual StatusCode fitVertex( const  ParticleVector&,  Vertex& ) = 0; 
};

#endif // DAVINCITOOLS_IVERTEXFITTER_H
