
#ifndef KFFITTOOL_H
#define KFFITTOOL_H 1

// Include files from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiKernel/AlgTool.h"

// Include files from DaVinciTools
#include "Kernel/IKFFitTool.h"
#include "DaVinciTools/IParticleTransporter.h"

// Forward declarations

class HepVector;
class HepSymMatrix;
class HepMatrix;


/** @class KFFitTool KFFitTool.h 
 * Kalman Filter type kinematic fit and refit tool for particle decay. 
 * It does:
 *    1) recompute covariance matrix of charged track Particles
 *    2) make a veretx with two flying particles (trajectories )
 *    3) merge two resonance vertices
 *    4) add a trajectory to a vertex
 *    5) add a photon to a vertex  
 *    6) add a photon pair to a vertex with or w/o mass constraint
 *    7) apply mass constraint to a compoite Particle
 *    8) use above methods to refit a particle 
 *
 * @author  @author Yuehong Xie
 *
 * @date 01/10/2004
 *
*/

class KFFitTool : public AlgTool,
                       virtual public IKFFitTool {

public:

  /// Standard Constructor
  KFFitTool( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent);

  /// Standard Destructor
  virtual ~KFFitTool() { }

  /// initialization
  StatusCode initialize();    

  /// Method to refit a particle
  StatusCode reFit( Particle&);
  
   /// Method to fit a decay
  StatusCode fitDecay( Particle&, ParticleVector&);                 
                                                                
  /// Method to apply mass constraint to a Particle
  StatusCode massConstrain( Particle&, double &) ;

  /// transformation matrix from energy parameters to mass parameters
  HepMatrix MatrixE2M(Particle&);
                                                                                
  /// transformation matrix from mass parameters to energy parameters
  HepMatrix MatrixM2E(Particle&);

  /// Method to set Particle parameters with energy
  StatusCode setEParameter(Particle&, HepVector&, HepSymMatrix&);

  /// Method to get Particle parameters with energy
  StatusCode getEParameter(Particle&, HepVector&, HepSymMatrix&);

  /// Method to set Particle parameters with mass
  StatusCode setMParameter(Particle&, HepVector&, HepSymMatrix&);

  /// Method to get Particle parameters with mass
  StatusCode getMParameter(Particle&, HepVector&, HepSymMatrix&);

  /// Method to get photon parameters: z, x,y,E  cov(x,y,E) 
  StatusCode getPhotonParameter(Particle&, double&,
                                HepVector&, HepSymMatrix&);

  /// Method to fit two a non-resonance Particles
  StatusCode fitWithTwoTrajectories(Particle&, Particle&,
                                    HepVector&, HepSymMatrix& );
                                                                                
  /// Method to merge vertices of two resonance Particles
  StatusCode mergeTwoVertices(Particle&, Particle&,
                              HepVector&, HepSymMatrix& );
                                                                                
  /// Method to add a non-photon Particle to a decay vertex
  StatusCode addParticle(Particle&, Particle&,
                         HepVector&, HepSymMatrix& );
                                                                                
  /// Method to add a photon to a decay vertex
  StatusCode addPhoton(Particle&, Particle&,
                       HepVector&, HepSymMatrix& ) ;

  /// Method to add a pair of photon to a decay vertex
  StatusCode addPhotonPair(Particle&, Particle&,
                           HepVector&, HepSymMatrix& ) ;

  /// Method to determine  if a particle has non-photon decay products
  bool isPurePhotonDecay(Particle&);

  /// convert energy parameter vector to mass parameter vector
  void Ve2Vm(HepVector&,HepVector&);
 
  /// convert mass parameter vector to mass parameter vector
  void Vm2Ve(HepVector&,HepVector&);

  /// get particle width
  double ParticleWidth(Particle&); 

  /// method to determine if a particle is composite
  bool isComposite(Particle&);

  /// method to reset charged track parameters
  StatusCode resetTrackParameters(Particle&);

  /// method to set state at first measurement
  StatusCode setStateAtFirstM(Particle&);

  /// Method to return the z of the most upstream measurement
  double zFirstMeasurement(Particle* part);

  /// get the first estimate for the z position
  double getZEstimate (Particle & part1, Particle & part2);

  /// Method to determine if a particle is made from a Downstream track
  bool isDownstreamTrack(Particle* part);

private:

  /// Private methods used in KFFitTool.

  unsigned   m_maxIter;          
  double     m_maxDeltaChiSq;    
  double     m_widthCut;
  bool       m_PhotonPairMassConstraint;

  IParticlePropertySvc* m_ppSvc;        
  IParticleTransporter* m_pTransporter;
  std::string m_transporterType;    

};

#endif // KFFITTOOL_H

