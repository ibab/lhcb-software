// $Id: TsaSeed.h,v 1.4 2009-02-11 08:19:54 wouter Exp $
#ifndef _TSASEED_H_
#define _TSASEED_H_

#include <string>
#include <vector>

#include "TsaBaseAlg.h"

//#include "TsaKernel/TimeSummary.h"
#include "TsaKernel/ITsaSeedStep.h"
#include "TsaKernel/ITsaStubFind.h"
#include "TsaKernel/ITsaStubLinker.h"
#include "TsaKernel/ITsaStubExtender.h"
#include "TsaKernel/SeedStub.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedHit.h"

#include "ITsaSeedAddHits.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class Seed
     *
     *  Main Seeding algorithm for Tsa
     *
     *  @author M.Needham
     *  @date   07/03/2002
     */

    class Seed: public BaseAlg 
    {

    public:

      // Constructors and destructor
      Seed(const std::string& name,
           ISvcLocator* pSvcLocator);
      virtual ~Seed();

      // IAlgorithm members
      virtual StatusCode initialize();
      virtual StatusCode execute();

    private:

      std::string m_seedTrackLocation;
      std::string m_seedHitLocation;
      std::string m_seedStubLocation;

      std::string m_selectorType;
      bool m_calcLikelihood;
      bool m_addHitsInITOverlap;

      std::vector<ITsaSeedStep*> m_xSearchStep;
      std::vector<ITsaSeedStep*> m_stereoStep;
      ITsaSeedStep* m_xSelection;
      ITsaSeedStep* m_finalSelection;
      ITsaSeedStep* m_likelihood;
      ITsaSeedAddHits* m_addHits;
      ITsaStubFind* m_stubFind;
      ITsaStubLinker* m_stubLinker;
      ITsaStubExtender* m_extendStubs;
    };

  }
}

#endif // _TSASEED
