// $ Exp $
#ifndef _TsaOTStereoSearch_H
#define _TsaOTStereoSearch_H

#include <string>

#include "TsaStereoBase.h"

#include "TsaKernel/TsaTStationHitManager.h"
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
    /** @class OTStereoSearch TsaOTStereoSearch.h
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

      using Tf::Tsa::StereoBase::execute;
      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

    private:

      void loadData(std::vector<SeedHit*> hits[6]) const;

      std::string m_dataSvcType;
      std::string m_dataSvcName;

      typedef Tf::Tsa::TStationHitManager OTHitMan;

      /// Pointer to the data manager
      OTHitMan * m_hitMan;

      double m_scth;
      double m_maxDriftRadius;
      int m_nHit;
      double m_yTol2;

      TfTsHitNumMap m_hitNumMap;

    };

  }
}

#endif  // _TsaOTStereoSearchBase_H



