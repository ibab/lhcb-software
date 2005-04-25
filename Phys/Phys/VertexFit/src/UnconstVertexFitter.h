// $Id: UnconstVertexFitter.h,v 1.5 2005-04-25 13:52:54 pkoppenb Exp $
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
 * Modified by: Luis Fernandez, 07/12/2004, reviewed 20/04/2005
 * - looks recursively for all final states (with origin) or long-lived daughters 
 *   in the resonances' decay trees
 * - all gammas(s) found in the descendants are re-evaluated at the vertex
 * - does not refit vertex if only one composite and gamma(s)
 * - for composites in the original list only decaying to gammas: 
 *   Point on track, decay vertex, momentum are *not* changed
 * - TEMPORARY fix: allow to fit only gammas, e.g. pi0 -> gamma gamma. This should not be allowed
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

  /// Get recursively all the final states (with origin) or long-lived 
  /// daughters from the resonances in the original list
  StatusCode getProductsForFit(Particle*&, ParticleVector&);
  /// Get the reference vertex for the case of one composite and gammas
  Vertex* singleCompositeVertex(const ParticleVector& );
  /// Fit the vertex given a vector of Particles as input. 
  StatusCode doFitVertex( const ParticleVector&, Vertex&);  
  /// Moves gammas to existing vertex
  StatusCode moveGammas( const ParticleVector&, Vertex&);  
  /// do the matrix math
  virtual StatusCode matrixMath( HepSymMatrix&, HepVector&, double&, Particle&);  
  /// get the vertex
  virtual StatusCode makeVertex( Vertex&, double, int, HepSymMatrix& , HepVector&, double);

  /// get the first estimate for the z position  
  double getZEstimate (const ParticleVector& particleList);

  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      

  IPhotonParams* m_pPhotonParams; ///< Reference to PhotonParams
  std::string m_PhotonParamsType; ///< Type

  int m_gammaID; ///< gamma particle ID
  double m_scale; // for cov matrix of online neutrals with origin
  bool m_useDaughters; ////< use daughers' tracks if composite is found, true by default

};

#endif // UNCONSTVERTEXFITTER_H
