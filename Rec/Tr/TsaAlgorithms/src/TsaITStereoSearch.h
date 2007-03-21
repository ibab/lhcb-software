// $ Exp $
#ifndef _TsaITStereoSearch_H
#define _TsaITStereoSearch_H

/** @class TsaITStereoSearch TsaITStereoSearch.h
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaStereoBase.h"

#include <string>

class IITDataSvc;
class SeedLineFit;

class TsaITStereoSearch:  public TsaStereoBase {

public:

  /// constructer
  TsaITStereoSearch(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaITStereoSearch();

  /// initialize
  StatusCode initialize();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

private:

  void loadData(std::vector<SeedHit*> hits[6]) const;

  SeedLineFit* m_parabolaFit;

  std::string m_dataSvcType;
  std::string m_dataSvcName;
  IITDataSvc* m_dataSvc;

};

#endif  // _TsaITStereoSearchBase_H

  

