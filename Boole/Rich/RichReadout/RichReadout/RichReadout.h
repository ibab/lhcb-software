#ifndef RICHREADOUT_H
#define RICHREADOUT_H 1

#include <algorithm>
#include <string>
#include <cmath>

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"

// local
#include "RichReadout/RichFrontEndResponse.h"
//---------------------------------------------------------------------------//

class RichReadout : public Algorithm {

public:

  RichReadout(const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichReadout();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  Algorithm* m_response;

};

#endif
