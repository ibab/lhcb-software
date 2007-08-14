// $ Exp $
#ifndef _TsaStubFind_H
#define _TsaStubFind_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

#include <algorithm>

#include "TsaKernel/ITsaStubFind.h"
#include "TsaKernel/SeedFun.h"
#include "TsaKernel/SeedStub.h"
#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedFun.h"
#include "TsaKernel/TsaConstants.h"

#include "LHCbMath/GeomFun.h"

#include "TfKernel/RecoFuncs.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class StubFind TsaStubFind.h
     * Make stubs in an IT sector
     * @author M. Needham
     **/

    class StubFind:  public GaudiTool,
                     virtual public ITsaStubFind
    {

    public:

      /// constructer
      StubFind(const std::string& type,
               const std::string& name,
               const IInterface* parent);

      // destructer
      virtual ~StubFind();

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

  }
}

#endif  // _TsaStubFind_H



