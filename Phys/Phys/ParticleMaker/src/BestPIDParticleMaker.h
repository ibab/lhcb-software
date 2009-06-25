#ifndef BESTPIDPARTICLEMAKER_H 
#define BESTPIDPARTICLEMAKER_H 1

#include "CombinedParticleMaker.h"

/** @class BestPIDParticleMaker BestPIDParticleMaker.h
 * It is based on CombinedParticleMaker
 *  
 * BestPIDParticleMaker makes Particles from ProtoParticles.
 * For each ProtoParticle a Particle with the best DLL that passes the 
 * ProtoParticle*Filter.
 * It uses all detector information available for a given hypothesis.
 * Possible Particles to create are muon, electron, kaon, proton, pion.
 * They can be specifed in the Particle option.
 *
 * Avoid using this algorithm if you are a b-physicist :
 *
 * If you do B->pipi and one pi decays to a muon, it will never be a pi
 * candidate. Even worse, if you or someone relaxes the mu ID cuts  
 * between two jobs or two DaVinci versions, then suddenly what used to be a 
 * nice pion in one version of DV might now becomes a muon candidate. Your
 * efficiency decreases although nothing affecting pion or kaon ID  
 * changed.
 *  That makes it very hard to estimate signal efficiencies as all
 * efficiencies depend on everything.
 * If you want to cut hard on PID, use PID cuts. If you don't want to use
 * the same track several times (which is a valid point for tagging),  
 * then use the Overlap Tool and re-weight the track accordingly if needed.
 *
 *
 *  @Version 1r0
 *  @author Neal Gueissaz
 *  @date   22 june 2009
 */

class BestPIDParticleMaker : public CombinedParticleMaker {
public: 
  /// Standard constructor
  BestPIDParticleMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BestPIDParticleMaker( ); ///< Destructor

  // Standard initialization of algorithm
  virtual StatusCode initialize();

  // Make the particles
  virtual StatusCode makeParticles( LHCb::Particle::Vector & parts );

  // Standard finalization of algorithm
  virtual StatusCode finalize  ();

protected:

private:

  /** @brief Job Option List of particles type to make.
   * Possible values are muons, electrons, kaons, protons, pions
   */
  std::vector<std::string> m_ParticleList;

  // Map type that takes a particle type to a ProtoParticle filter
  typedef std::pair< const LHCb::ParticleProperty *, 
                     const IProtoParticleFilter* > ProtoPair;
  typedef std::vector < ProtoPair > ProtoMap;
  ProtoMap m_protoMap;

  class DLLPIDPair {

  private:
    std::string pid;
    double dll;

    DLLPIDPair() {};

  public:
    DLLPIDPair( std::string name, double val) :
      pid (name),
      dll (val) {};
    
    ~DLLPIDPair() {};
    std::string GetPid () const { return pid; }
    double GetDll () const { return dll; }

    static bool cmp ( const DLLPIDPair& one, const DLLPIDPair& two )  {
      return one.GetDll () > two.GetDll ();
    }
  };

  // DLL, PID pairs
  typedef std::vector < DLLPIDPair > DLLPIDVector;

};

#endif // BESTPIDPARTICLEMAKER_H
