// $ Exp $
#ifndef _TsaITStereoSearch_H
#define _TsaITStereoSearch_H

#include <string>

#include "TsaStereoBase.h"
#include "TsaTStationHitManager.h"
#include "TsaKernel/TsaSeedingHit.h"
#include "TfTsHitNumMap.h"

namespace Tf
{
  namespace Tsa
  {

    class SeedLineFit;

    /** @class ITStereoSearch TsaITStereoSearch.h
     *  Follow track and pick up hits
     * @author M. Needham
     **/

    class ITStereoSearch:  public StereoBase
    {

    public:

      /// constructer
      ITStereoSearch(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

      // destructer
      virtual ~ITStereoSearch();

      /// initialize
      StatusCode initialize();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

    private:

      void loadData(std::vector<SeedHit*> hits[6]) const;

      SeedLineFit* m_parabolaFit;

      std::string m_dataSvcType;
      std::string m_dataSvcName;

      typedef Tf::Tsa::TStationHitManager ITHitMan;

      /// Pointer to the data manager
      ITHitMan * m_hitMan;

      TfTsHitNumMap m_hitNumMap;

    };

  }
}

#endif  // _TsaITStereoSearchBase_H



