// $Id: ITsaSeedStep.h,v 1.1 2007-09-16 09:44:44 mneedham Exp $
#ifndef _ITsaSeedStep_H
#define _ITsaSeedStep_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITsaSeedStep( "ITsaSeedStep", 0, 0 );

#include <vector>

namespace LHCb {
  class State;
}

namespace Tf
{
  namespace Tsa
  {

    class SeedTrack;
    class SeedHit;

    /** @class ITsaSeedStep ITsaSeedStep.h TsaKernel/ITsaSeedStep
     *
     *  Interface to data svc
     *
     *  @author M.Needham
     *  @date   12/11/2006
     */

    class ITsaSeedStep : virtual public IAlgTool 
    {
    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ITsaSeedStep ; }

      virtual StatusCode execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) = 0;

      virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) = 0;

    };

  }
}

#endif
