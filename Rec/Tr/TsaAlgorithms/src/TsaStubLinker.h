// $ Exp $
#ifndef _TsaStubLinker_H
#define _TsaStubLinker_H

/** @class TsaStubLinker TsaStubLinker Tsa/TsaStubLinker
* Link stubs in the IT
* @author M. Needham
**/

#include "TsaKernel/ITsaStubLinker.h"
#include "GaudiAlg/GaudiTool.h"
#include <algorithm>

class SeedParabolaFit;
class SeedLineFit;

class SeedHit;

class TsaStubLinker:  public GaudiTool,
                       virtual public ITsaStubLinker {

public:

  /// constructer
  TsaStubLinker(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~TsaStubLinker();

  virtual StatusCode execute(std::vector<SeedStub*> stubs[], std::vector<SeedTrack*>& seeds)  ;

private:  

 double xSlopeCon(const SeedStub* point1, const SeedStub* point2) const;

 SeedLineFit* m_fitLine;
 SeedParabolaFit* m_parabolaFit;

 double m_deltaSx;
 double m_deltaXCon;

};

#include "TsaKernel/SeedStub.h"

inline double TsaStubLinker::xSlopeCon(const SeedStub* point1,
                                       const SeedStub* point2) const{

 double slopeChord = (point1->x() - point2->x())/ (point1->z1()-point2->z1());
 return(slopeChord - 0.5*(point1->sx()+point2->sx()));
}

#endif  // _TsaStubLinker_H

  

