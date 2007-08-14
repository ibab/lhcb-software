// $Id: TsaITXSearch.h,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
#ifndef _TsaITXSearch_H
#define _TsaITXSearch_H

#include <string>

#include "TsaXSearchBase.h"

#include "TfKernel/ITStationHitManager.h"
#include "TsaKernel/TsaSeedingHit.h"

#include "TfTsHitNumMap.h"

namespace Tf
{
  namespace Tsa
  {

    class SeedParabolaFit;

    /** @class ITXSearch TsaITXSearch.h
     * Follow track and pick up hits
     * @author M. Needham
     **/
    class ITXSearch:  public XSearchBase {

    public:

      /// constructer
      ITXSearch(const std::string& type,
                const std::string& name,
                const IInterface* parent);

      // destructer
      virtual ~ITXSearch();

      /// initialize
      StatusCode initialize();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

    private:

      void loadData(std::vector<SeedHit*> hits[6]) const;

      SeedParabolaFit* m_parabolaFit;

      std::string m_dataSvcType;
      std::string m_dataSvcName;

      typedef Tf::ITStationHitManager<Tf::Tsa::SeedingHit> ITHitMan;

      /// Pointer to the data manager
      ITHitMan * m_hitMan;

      TfTsHitNumMap m_hitNumMap;

    };

  }
}

#endif  // _TsaITXSearchBase_H



