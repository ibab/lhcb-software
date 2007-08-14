// $ Exp $
#ifndef _TsaOTXSearch_H
#define _TsaOTXSearch_H

#include "TsaXSearchBase.h"

#include <string>
#include <algorithm>
#include <utility>

#include "TfKernel/OTStationHitManager.h"
#include "TsaKernel/TsaSeedingHit.h"
#include "SeedParabolaFit.h"
#include "TsaKernel/SeedTrack.h"

#include "TsaKernel/TsaConstants.h"

#include "TfTsHitNumMap.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class TsaOTXSearch TsaOTXSearch.h
     * Follow track and pick up hits
     * @author M. Needham
     **/

    class OTXSearch:  public XSearchBase 
    {

    public:

      /// constructer
      OTXSearch(const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

      // destructer
      virtual ~OTXSearch();

      /// initialize
      StatusCode initialize();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

    private:

      void loadData(std::vector<SeedHit*> hits[6]) const;

      SeedParabolaFit* m_parabolaFit;

      std::string m_dataSvcType;
      std::string m_dataSvcName;

      double m_maxDriftRadius;

      typedef Tf::OTStationHitManager<Tf::Tsa::SeedingHit> OTHitMan;

      /// Pointer to the data manager
      OTHitMan * m_hitMan;

      TfTsHitNumMap m_hitNumMap;

    };

  }
}

#endif  // _TsaOTXSearchBase_H



