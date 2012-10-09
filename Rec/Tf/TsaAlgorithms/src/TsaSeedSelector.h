#ifndef _TsaSeedSelector_H
#define _TsaSeedSelector_H

#include "TsaSelectorBase.h"

#include <string>

namespace Tf
{
  namespace Tsa
  {

    /** @class SeedSelector TsaSeedSelector.h
     * Follow track and pick up hits
     * @author M. Needham
     **/

    class SeedSelector: public SelectorBase
    {

    public:

      /// constructer
      SeedSelector(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

      // destructer
      virtual ~SeedSelector();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

      virtual StatusCode execute(LHCb::State& hint,std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

    };

  }
}

#endif  // _TsaSeedSelector_H



