// $ Exp $
#ifndef _TsaSeedSimpleSelector_H
#define _TsaSeedSimpleSelector_H

/** @class TsaSeedSimpleSelector TsaSeedSimpleSelector.h
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaSelectorBase.h"

#include <string>

class TsaSeedSimpleSelector: public TsaSelectorBase {

public:

  /// constructer
  TsaSeedSimpleSelector(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaSeedSimpleSelector();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

  virtual StatusCode execute(LHCb::State& hint,std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

};

#endif  // _TsaSeedSimpleSelector_H

  

