// $Id: KinBestParticles.h,v 1.2 2004-08-23 13:55:49 pkoppenb Exp $
#ifndef KINBESTPARTICLES_H 
#define KINBESTPARTICLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "DaVinciTools/IBestParticles.h"            // Interface


/** @class KinBestParticles KinBestParticles.h
 *  
 * This tool returns a vector of n particles ordered by 
 * P, Pt or E from a given vector of particles.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-15
 */
class KinBestParticles : public GaudiTool, virtual public IBestParticles {
public: 
  /// Standard constructor
  KinBestParticles( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);

  virtual ~KinBestParticles( ); ///< Destructor

  /// Get m_number candidates with highest <var>
  StatusCode Candidates( const ParticleVector & input,ParticleVector& output);
  // Get m_number candidates with highest <var>
  //  StatusCode Candidates( const Particles & input,Particles& output);
  /// Get the candidate with highest <var>
  StatusCode BestCandidate( ParticleVector& input,Particle* output);
  // Get the candidate with highest <var>
  //  StatusCode BestCandidate( Particles& input,Particle* output);

  StatusCode initialize();
  StatusCode finalize();

protected:

private:

  /// Get m_number candidates with highest <var>
  StatusCode getCandidates( const ParticleVector&, ParticleVector&, 
                            const int& n);

  bool m_byPt;  ///< sort by Pt
  bool m_byP;   ///< sort by P
  bool m_byE;   ///< sort by E
  int m_number; ///< Number of Particles to get
  
  // class associating a Particle to the requested value  
  class PResult{
  public:
    PResult(Particle* P, double& v){m_p = P; m_value = v; return;};
    Particle* getP(){return m_p;};
    double getV(){return m_value;};
  private:
    Particle* m_p;
    double m_value;
  };
  
  
};
#endif // KINBESTPARTICLES_H
