#ifndef TESLATOOLS_TESLAMATCHER_H 
#define TESLATOOLS_TESLAMATCHER_H 1

//------------------------------------------------------------------------------
// TeslaTools
//------------------------------------------------------------------------------
#include "TeslaTools/ITeslaMatcher.h"
//------------------------------------------------------------------------------
// STL
//------------------------------------------------------------------------------
#include <string>
#include <vector>
//------------------------------------------------------------------------------
// DaVinci
//------------------------------------------------------------------------------
#include <Event/Particle.h>
//------------------------------------------------------------------------------
// Gaudi
//------------------------------------------------------------------------------
#include "GaudiAlg/GaudiTool.h"

#define MATCHING_FUNC(name, original, cand, defaultCand) \
    const Particle* name(\
                               const Particle* original,\
                               const Particle* cand,\
                               const Particle* defaultCand)

class TeslaMatcher : public GaudiTool, virtual public ITeslaMatcher 
{
  public:
    TeslaMatcher (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

    ~TeslaMatcher();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    virtual StatusCode findBestMatch (
        const Particle* inputParticle,
        const Particle* &outputParticle,
        const std::string& tesLocation ) const;  



  protected:
    static MATCHING_FUNC(defaultBestMatching,original,cand,defaultCand);
    static MATCHING_FUNC(t2bBestMatching,original,cand,defaultCand);
    static MATCHING_FUNC(trackBestMatching,original,cand,defaultCand);
    static MATCHING_FUNC(momentumBestMatching,original,cand,defaultCand);

  private:
    std::function<const Particle*(const Particle*, 
                                  const Particle*, 
                                  const Particle*)> m_bestMatching;

    bool m_checkPid;
    std::string m_matchingTechnique;


};

#undef MATCHING_FUNC

#endif //TESLATOOLS_TESLAMATCHER_H
