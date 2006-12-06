// $ Exp $
#ifndef _TsaSeedSelector_H
#define _TsaSeedSelector_H

/** @class TsaXSearchBase TsaXSearchBase Tsa/TsaXSearchBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaSelectorBase.h"

#include <string>


class TsaSeedSelector: public TsaSelectorBase{

public:

  /// constructer
  TsaSeedSelector(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  // destructer
  virtual ~TsaSeedSelector();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

  virtual StatusCode execute(LHCb::State& hint,std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

 private:

  ITsaSeedStep* m_finalSelection;

};

#endif  // _TsaSeedSelector_H

  

