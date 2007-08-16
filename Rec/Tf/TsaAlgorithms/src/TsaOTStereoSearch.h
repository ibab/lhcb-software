// $ Exp $
#ifndef _TsaOTStereoSearch_H
#define _TsaOTStereoSearch_H

#include <string>

#include "TsaStereoBase.h"
#include "SeedParabolaFit.h"
#include "SeedLineFit.h"

#include "TsaOTStationHitManager.h"

#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedFun.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/TsaSeedingHit.h"
#include "TsaKernel/SeedTrack.h"

#include "TfTsHitNumMap.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class TsaOTStereoSearch TsaOTStereoSearch.h
     * Follow track and pick up hits
     * @author M. Needham
     **/
    class OTStereoSearch:  public StereoBase
    {

    public:

      /// constructer
      OTStereoSearch(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

      // destructer
      virtual ~OTStereoSearch();

      /// initialize
      StatusCode initialize();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

    private:

      void loadData(std::vector<SeedHit*> hits[6]) const;

      SeedLineFit* m_parabolaFit;

      std::string m_dataSvcType;
      std::string m_dataSvcName;

      typedef Tf::Tsa::OTStationHitManager OTHitMan;

      /// Pointer to the data manager
      OTHitMan * m_hitMan;

      double m_scth;
      double m_maxDriftRadius;
      int m_nHit;

      TfTsHitNumMap m_hitNumMap;

    };

  }
}

#endif  // _TsaOTStereoSearchBase_H



