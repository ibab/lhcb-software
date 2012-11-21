// $ Exp $
#ifndef _TsaOTXSearch_H
#define _TsaOTXSearch_H

#include <string>
#include <algorithm>
#include <utility>

#include "TsaKernel/TsaSeedingHit.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/TsaTStationHitManager.h"

#include "TsaXSearchBase.h"

#include "TfTsHitNumMap.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class OTXSearch TsaOTXSearch.h
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

      using Tf::Tsa::XSearchBase::execute;
      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

    private:

      void loadData(std::vector<SeedHit*> hits[6]) const;

      std::string m_dataSvcType;
      std::string m_dataSvcName;

      double m_maxDriftRadius;

      typedef Tf::Tsa::TStationHitManager OTHitMan;

      /// Pointer to the data manager
      OTHitMan * m_hitMan;

      TfTsHitNumMap m_hitNumMap;

    };

  }
}

#endif  // _TsaOTXSearchBase_H



