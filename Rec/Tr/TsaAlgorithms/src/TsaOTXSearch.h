// $ Exp $
#ifndef _TsaOTXSearch_H
#define _TsaOTXSearch_H

/** @class TsaOTXSearch TsaOTXSearch.h
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaXSearchBase.h"

#include <string>

class IOTDataSvc;
class SeedParabolaFit;

class TsaOTXSearch:  public TsaXSearchBase {

public:

  /// constructer
  TsaOTXSearch(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaOTXSearch();

  /// initialize
  StatusCode initialize();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

private:

  void loadData(std::vector<SeedHit*> hits[6]) const;

  SeedParabolaFit* m_parabolaFit;

  std::string m_dataSvcType;
  std::string m_dataSvcName;
  IOTDataSvc* m_dataSvc;
  double m_maxDriftRadius;
 


};

#endif  // _TsaOTXSearchBase_H

  

