// $Id: UnconstVertexFitter.h,v 1.3 2002-05-15 23:23:33 gcorti Exp $
#ifndef UNCONSTVERTEXFITTER_H
#define UNCONSTVERTEXFITTER_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IVertexFitter.h"

// Forward declarations
class IParticleTransporter;

/** @class UnconstVertexFitter UnconstVertexFitter.h 
 *  Perform an unconstrained vertex fit.
 *  Based on the fortran axksvrtx.F and on its translation to 
 *  C++ by D. Hutchcroft
 *
 * @author S. Amato
 * @date 16/12/2001 
*/

class UnconstVertexFitter : public AlgTool,
                            virtual public IVertexFitter {
public:

  /// Standard Constructor
  UnconstVertexFitter( const std::string& type, const std::string& name, 
                       const IInterface* parent);
  
  /// Standard Destructor
  virtual ~UnconstVertexFitter() { }

  /// Retrieve the Particle Tranporter Tool.
  StatusCode initialize();    

  /// Fit the vertex given a vector of Particles as input. 
  StatusCode fitVertex( const ParticleVector& particleList,Vertex& myVertex ); 

  /// Fit the vertex given two Particles as input. 
  StatusCode fitVertex( Particle& particle1, Particle& particle2, 
                        Vertex& myVertex ); 

  /// Fit the vertex given three Particles as input. 
  StatusCode fitVertex( Particle& particle1, Particle& particle2, 
                        Particle& particle3, Vertex& myVertex ); 

  /// get the first estimate for the z position  
  double getZEstimate (const ParticleVector& particleList);
  

private:
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  
};

#endif // UNCONSTVERTEXFITTER_H
