// $ Exp $
#ifndef _TsaSelectorBase_H
#define _TsaSelectorBase_H

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"

#include <string>

namespace Tf
{
  namespace Tsa
  {

    /** @class SelectorBase TsaSelectorBase.h
     * Follow track and pick up hits
     * @author M. Needham
     **/

    class SelectorBase: public GaudiTool,
                        virtual public ITsaSeedStep
    {

    public:

      /// constructer
      SelectorBase(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

      // destructer
      virtual ~SelectorBase();

    protected:

      // execute method
      StatusCode select(std::vector<SeedTrack*>& seeds);

    private:

      double m_fracUsed;

    };

  }
}

#endif  // _TsaSeedSimpleSelector_H



