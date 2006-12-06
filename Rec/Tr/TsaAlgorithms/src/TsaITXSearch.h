// $ Exp $
#ifndef _TsaITXSearch_H
#define _TsaITXSearch_H

/** @class TsaXSearchBase TsaXSearchBase Tsa/TsaXSearchBase
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaXSearchBase.h"

#include <string>

class IITDataSvc;
class SeedParabolaFit;

class TsaITXSearch:  public TsaXSearchBase {

public:

  /// constructer
  TsaITXSearch(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaITXSearch();

  /// initialize
  StatusCode initialize();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

private:

  void loadData(std::vector<SeedHit*> hits[6]) const;

  SeedParabolaFit* m_parabolaFit;

  std::string m_dataSvcType;
  std::string m_dataSvcName;
  IITDataSvc* m_dataSvc;



};

#endif  // _TsaITXSearchBase_H

  

