#ifndef TESLATOOLS_ITESLAMATCHER_H 
#define TESLATOOLS_ITESLAMATCHER_H 1

//==============================================================================
// Gaudi Kernel
//==============================================================================
#include "GaudiKernel/IAlgTool.h"
//==============================================================================
// DaVinci
//==============================================================================
#include <Event/Particle.h>
//==============================================================================
// STL
//==============================================================================
#include <string>


static const InterfaceID IID_ITeslaMatcher ( "ITeslaMatcher", 1, 0);

using namespace LHCb;

class ITeslaMatcher : virtual public IAlgTool
{
  public:
    static const InterfaceID& interfaceID() {return IID_ITeslaMatcher;}

    virtual StatusCode findBestMatch (
        const Particle* inputParticle,
        const Particle* &outputParticle,
        const std::string& tesLocation ) const =0;  



  private:
    std::function<const Particle*(const Particle*, 
                                  const Particle*, 
                                  const Particle*)> m_bestMatching;


     
};


#endif //TESLATOOLS_ITESLAMATCHER_H
