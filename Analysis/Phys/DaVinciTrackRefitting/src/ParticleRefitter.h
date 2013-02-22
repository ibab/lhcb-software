// $Id: $
#ifndef INCLUDE_PARTICLEREFITTER_H
#define INCLUDE_PARTICLEREFITTER_H 1

#include "Kernel/DaVinciAlgorithm.h"

class IParticle2State;
class IVertexFit;

/**
 * @class ParticleRefitter ParticleRefitter.h
 * @brief update LHCb::Particles to collect updates to their LHCb::Track objects
 * mainly copied from preliminary work by Christian Voss
 *
 * @author Paul Seyfert <pseyfert@cern.ch>
 * @date   2013-02-19
 */
class ParticleRefitter : public DaVinciAlgorithm {

  public:

    /// Standard Constructor
    ParticleRefitter(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~ParticleRefitter(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode    execute(); ///< Algorithm event execution
    //  virtual StatusCode   finalize(); ///< Algorithm finalize

  protected:

    virtual StatusCode execute(LHCb::Particle*);

  private:

    IParticle2State* m_stateToParticle;
    std::string m_trktoolname,  m_fittoolname;
    IVertexFit         *m_fit;
    std::vector<LHCb::Vertex*> m_vt;  // @todo: keeping vertices like this in the storage isn't exactly good practice (use TES instead)


};
#endif // INCLUDE_PARTICLEREFITTER_H

