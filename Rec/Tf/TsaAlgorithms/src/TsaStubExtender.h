// $ Exp $
#ifndef _TsaStubExtender_H
#define _TsaStubExtender_H

#include <algorithm>

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

#include "TsaKernel_Tf/ITsaStubExtender.h"
#include "TsaKernel_Tf/SeedFun.h"
#include "TsaKernel_Tf/SeedStub.h"
#include "TsaKernel_Tf/SeedTrack.h"
#include "TsaKernel_Tf/SeedPnt.h"
#include "TsaKernel_Tf/TsaConstants.h"

#include "SeedParabolaFit.h"
#include "SeedLineFit.h"

#include "LHCbMath/GeomFun.h"

#include "TfKernel/RecoFuncs.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class StubExtender TsaStubExtender.h
     * Extend the IT stubs into the OT
     * @author M. Needham
     **/

    class StubExtender:  public GaudiTool,
                         virtual public ITsaStubExtender 
    {

    public:

      /// constructer
      StubExtender(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

      /// destructer
      virtual ~StubExtender();

      virtual StatusCode initialize();

      virtual StatusCode execute(int& sect, std::vector<SeedStub*> stubs[], std::vector<SeedHit*> hits[],
                                 std::vector<SeedHit*> sHits[], std::vector<SeedTrack*>& seeds ) ;

    private:

      double m_scth;
      SeedLineFit* m_fitLine;
      SeedParabolaFit* m_parabolaFit;


      double m_dSlopeCut;
      double m_searchTol;
      int m_nxCut;
      int m_nyCut;
      double m_txCut;
      double m_tyCut;
      double m_yTol;
      int m_nTotalCut1;
      int m_nTotalCut2;
      double m_y0Cut1;
      double m_y0Cut2;
      double m_dxCut;
      double m_dyCut;

    };

  }
}

#endif  // _TsaStubLinker_H



