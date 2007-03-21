// $ Exp $
#ifndef _TsaStubFind_H
#define _TsaStubFind_H

/** @class TsaStubFind TsaStubFind.h
* Make stubs in an IT sector
* @author M. Needham
**/

#include "TsaKernel/ITsaStubFind.h"
#include "GaudiAlg/GaudiTool.h"
#include <algorithm>

class SeedHit;

class TsaStubFind:  public GaudiTool,
                       virtual public ITsaStubFind {

public:

  /// constructer
  TsaStubFind(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~TsaStubFind();

  virtual StatusCode execute(std::vector<SeedHit*> hits[], std::vector<SeedHit*> sHits[], 
                             std::vector<SeedStub*> stubs[]) ;

private:  

   
  double m_yTol;
  double m_xsParam;
  double m_syCut;
  double m_yCut;
  double m_sxCut;
  double m_dAngle;
  int m_sector;

};

#endif  // _TsaStubFind_H

  

