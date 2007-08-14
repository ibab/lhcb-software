// $Id: TsaInitialization.h,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
#ifndef _TSAINITIALIZATION_H_
#define _TSAINITIALIZATION_H_

/** @class TsaInitialization  TsaInitialization.h TsaAlgorithms/TsaInitialization.h
 *
 *  Class for Initialization
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "TsaBaseAlg.h"
#include <string>

#include "TsaKernel/TsaSeedingHit.h"

#include "TfKernel/OTStationHitManager.h"
#include "TfKernel/ITStationHitManager.h"
#include "TfKernel/TTStationHitManager.h"

namespace Tf
{
  namespace Tsa
  {

    class Initialization: public BaseAlg
    {

    public:

      // Constructors and destructor
      Initialization(const std::string& name,
                     ISvcLocator* pSvcLocator);

      virtual ~Initialization();

      // IAlgorithm members
      virtual StatusCode initialize();
      virtual StatusCode execute();

    private:

      typedef Tf::OTStationHitManager<Tf::Tsa::SeedingHit> OTHitMan;
      typedef Tf::ITStationHitManager<Tf::Tsa::SeedingHit> ITHitMan;
      typedef Tf::TTStationHitManager<Tf::Tsa::SeedingHit> TTHitMan;

      OTHitMan* otHitMan()
      {
        if ( !m_otDataSvc )  m_otDataSvc = tool<OTHitMan>(m_otDataSvcType,m_otDataSvcName);
        return m_otDataSvc;
      }

      ITHitMan* itHitMan()
      {
        if ( !m_itDataSvc )  m_itDataSvc = tool<ITHitMan>(m_itDataSvcType,m_itDataSvcName);
        return m_itDataSvc;
      }

      TTHitMan* ttHitMan()
      {
        if ( !m_ttDataSvc )  m_ttDataSvc = tool<TTHitMan>(m_ttDataSvcType,m_ttDataSvcName);
        return m_ttDataSvc;
      }

    private:

      std::string m_otDataSvcType;
      std::string m_itDataSvcType;
      std::string m_ttDataSvcType;
      std::string m_otDataSvcName;
      std::string m_itDataSvcName;
      std::string m_ttDataSvcName;

      OTHitMan* m_otDataSvc;
      ITHitMan* m_itDataSvc;
      TTHitMan* m_ttDataSvc;

      bool m_initIT;
      bool m_initOT;
      bool m_initTT;

    };

  }
}

#endif // _TSAINITIALIZATION_H_











