// $Id: UnconstVertexFitter.h,v 1.1.1.1 2004-08-24 06:28:54 pkoppenb Exp $
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
  /// In case any particle is a resonance, it uses the daughters and call
  /// the doFitVertex to actually perform the fit 
  StatusCode fitVertex( const ParticleVector& particleList,Vertex& myVertex ); 

  /// Fit the vertex given two Particles as input. 
  /// In case any particle is a resonance, it uses the daughters and call
  /// the doFitVertex to actually perform the fit 
  StatusCode fitVertex( Particle& particle1, Particle& particle2, 
                        Vertex& myVertex ); 

  /// Fit the vertex given three Particles as input. 
  /// In case any particle is a resonance, it uses the daughters and call
  /// the doFitVertex to actually perform the fit 
  StatusCode fitVertex( Particle& particle1, Particle& particle2, 
                        Particle& particle3, Vertex& myVertex ); 


  /// get the first estimate for the z position  
  double getZEstimate (const ParticleVector& particleList);
  /// Fit the vertex given a vector of Particles as input. 
  StatusCode doFitVertex( const ParticleVector& particleList,
                          Vertex& myVertex ); 
  

private:
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      
};

#endif // UNCONSTVERTEXFITTER_H
