// $ Exp $
#ifndef _TsaXProjSelector_H
#define _TsaXProjSelector_H

/** @class TsaXSearchBase TsaXSearchBase Tsa/TsaXSearchBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"

#include <string>

class TsaXProjSelector: public GaudiTool,
                       virtual public ITsaSeedStep{

public:

  /// constructer
  TsaXProjSelector(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaXProjSelector();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

  virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0 ) ;

private:

  double m_fracUsed;

};

#endif  // _TsaXProjSelector_H

  

