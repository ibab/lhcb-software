// $ Exp $
#ifndef _TsaOTStereoSearch_H
#define _TsaOTStereoSearch_H

/** @class TsaXSearchBase TsaXSearchBase Tsa/TsaXSearchBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaStereoBase.h"

#include <string>

class IOTDataSvc;
class SeedLineFit;

class TsaOTStereoSearch:  public TsaStereoBase {

public:

  /// constructer
  TsaOTStereoSearch(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaOTStereoSearch();

  /// initialize
  StatusCode initialize();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

private:

  void loadData(std::vector<SeedHit*> hits[6]) const;

  SeedLineFit* m_parabolaFit;

  std::string m_dataSvcType;
  std::string m_dataSvcName;
  IOTDataSvc* m_dataSvc;

  double m_scth;
  double m_maxDriftRadius;
  int m_nHit;

};

#endif  // _TsaOTStereoSearchBase_H

  

