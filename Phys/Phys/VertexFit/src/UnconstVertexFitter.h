// $Id: UnconstVertexFitter.h,v 1.4 2005-02-04 10:12:53 pkoppenb Exp $
#ifndef UNCONSTVERTEXFITTER_H
#define UNCONSTVERTEXFITTER_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IVertexFitter.h"

// Forward declarations
class IParticleTransporter;
class IPhotonParams;

/** @class UnconstVertexFitter UnconstVertexFitter.h 
 *  Perform an unconstrained vertex fit.
 *  Based on the fortran axksvrtx.F and on its translation to 
 *  C++ by D. Hutchcroft
 *
 * @author S. Amato
 * @date 16/12/2001 
 *
 * Modified by: Luis Fernandez, 07/12/2004 
 * - fit with neutral(s) with origin
 * 
*/

class UnconstVertexFitter : public GaudiTool,
                            virtual public IVertexFitter {
public:

  /// Standard Constructor
  UnconstVertexFitter( const std::string& type, const std::string& name, 
                       const IInterface* parent);
  
  /// Standard Destructor
  virtual ~UnconstVertexFitter() { }

  /// Retrieve the Particle Tranporter Tool.
  virtual StatusCode initialize();    

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


private:

  /// find "charged" and "neutrals"
  int splitIntoNeutralsAndCharged(const ParticleVector&, ParticleVector&, ParticleVector&);
  /// special case where there is only a resonance in the list
  Vertex* singleResonanceVertex(const ParticleVector& );
  /// Fit the vertex given a vector of Particles as input. 
  StatusCode doFitVertex( const ParticleVector&, Vertex&);  
  /// Add neutrals to vertex
  StatusCode addNeutrals( const ParticleVector&, ParticleVector&, Vertex&);  
  /// do the matrix math
  virtual StatusCode matrixMath( HepSymMatrix&, HepVector&, double&, Particle&);  
  /// get the vertex
  virtual StatusCode makeVertex( Vertex&, double, int, 
                                 HepSymMatrix& , HepVector&, double);  

  /// get the first estimate for the z position  
  double getZEstimate (const ParticleVector& particleList);

  

  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      

  IPhotonParams* m_pPhotonParams; ///< Reference to PhotonParams
  std::string m_PhotonParamsType; ///< Type

  int m_gammaID; ///< gamma particle ID
  double m_scale; // for cov matrix of neutrals with origin
};

#endif // UNCONSTVERTEXFITTER_H
