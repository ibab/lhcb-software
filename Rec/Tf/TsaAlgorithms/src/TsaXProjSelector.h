// $ Exp $
#ifndef _TsaXProjSelector_H
#define _TsaXProjSelector_H

#include "TsaKernel/ITsaSeedStep.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"

#include "GaudiAlg/GaudiTool.h"

#include <string>

namespace Tf
{
  namespace Tsa
  {

    /** @class XProjSelector TsaXProjSelector.h
     * Follow track and pick up hits
     * @author M. Needham
     **/

    class XProjSelector: public GaudiTool,
                         virtual public ITsaSeedStep
    {

    public:

      /// constructer
      XProjSelector(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

      // destructer
      virtual ~XProjSelector();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

      virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

    private:

      double m_fracUsed;

    };

  }
}

#endif  // _TsaXProjSelector_H



