// $ Exp $
#ifndef _TsaSelectorBase_H
#define _TsaSelectorBase_H

/** @class TsaXSearchBase TsaXSearchBase Tsa/TsaXSearchBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"

#include <string>

class TsaSelectorBase: public GaudiTool,
                       virtual public ITsaSeedStep{

public:

  /// constructer
  TsaSelectorBase(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  // destructer
  virtual ~TsaSelectorBase();

protected:

  // execute method
  StatusCode select(std::vector<SeedTrack*>& seeds);

private:

  double m_fracUsed;

};

#endif  // _TsaSeedSimpleSelector_H

  

