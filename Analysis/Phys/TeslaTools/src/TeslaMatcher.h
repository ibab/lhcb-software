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

class TeslaMatcher : public GaudiTool, virtual public ITeslaMatcher 
{
  public:
    TeslaMatcher (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

    ~TeslaMatcher();

    virtual StatusCode initialize();

    virtual StatusCode findBestMatch (
        const Particle* inputParticle,
        const Particle* &outputParticle,
        const std::string& tesLocation ) const;  



  protected:
    static const Particle* defaultBestMatching(
                               const Particle* original,
                               const Particle* cand,
                               const Particle* defaultCand);

  private:
    std::function<const Particle*(const Particle*, 
                                  const Particle*, 
                                  const Particle*)> m_bestMatching;

    bool m_checkPid;


};

#endif //TESLATOOLS_TESLAMATCHER_H
