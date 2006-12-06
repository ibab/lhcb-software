// $ Exp $
#ifndef _TsaStubExtender_H
#define _TsaStubExtender_H

/** @class TsaStubExtender TsaStubExtender Tsa/TsaStubExtender
* Extend the IT stubs into the OT
* @author M. Needham
**/

#include "TsaKernel/ITsaStubExtender.h"
#include "GaudiAlg/GaudiTool.h"
#include <algorithm>

class SeedParabolaFit;
class SeedLineFit;
class ITsaSeedStep;

class TsaStubExtender:  public GaudiTool,
                         virtual public ITsaStubExtender {

public:

  /// constructer
  TsaStubExtender(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  /// destructer
  virtual ~TsaStubExtender();

  virtual StatusCode initialize();

  virtual StatusCode execute(int& sect, std::vector<SeedStub*> stubs[], std::vector<SeedHit*> hits[], 
                             std::vector<SeedHit*> sHits[], std::vector<SeedTrack*>& seeds ) ;

private:  

  double m_scth;
  SeedLineFit* m_fitLine;
  SeedParabolaFit* m_parabolaFit;
  ITsaSeedStep* m_finalSelection;


};


#endif  // _TsaStubLinker_H

  

