// $ Exp $
#ifndef _TsaLikelihood_H
#define _TsaLikelihood_H

#include "GaudiAlg/GaudiTool.h"

#include <string>
#include <vector>
#include <algorithm>
#include <utility>

#include "TsaKernel_Tf/ITsaSeedStep.h"
#include "TsaKernel_Tf/SeedPnt.h"
#include "TsaKernel_Tf/SeedHit.h"
#include "TsaKernel_Tf/SeedTrack.h"
#include "TsaKernel_Tf/SeedFunctor.h"
#include "TsaKernel_Tf/TsaConstants.h"
#include "TsaKernel_Tf/Parabola.h"
#include "TsaKernel_Tf/Line.h"
#include "TsaKernel_Tf/IOTExpectedHits.h"
#include "TsaKernel_Tf/IITExpectedHits.h"

#include "Kernel/OTChannelID.h"
#include "Kernel/STChannelID.h"
#include "Kernel/LHCbID.h"

// Event
#include "Event/State.h"

#include "SeedParabolaFit.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class Likelihood TsaLikelihood.h
     * Follow a track and pick up hits
     * @author M. Needham
     **/
    class Likelihood: public GaudiTool,
                      virtual public ITsaSeedStep{

    public:

      /// constructer
      Likelihood(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

      // destructer
      virtual ~Likelihood();

      virtual StatusCode initialize();

      // execute method
      virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

      virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0) ;

    private:

      void expectationIT(const Line& aLine, const Parabola& aParab,
                         std::vector<SeedPnt>& pnts, int* layers, const int& sect,
                         int& nFound,
                         int& nExp) const;

      void expectationOT(const Line& aLine, const Parabola& aParab,
                         std::vector<SeedPnt>& pnts, int* layers ,
                         const int& sect,
                         int& nxFound,
                         int& nxExp, double& xLik) const;

      void expectationMixed(const Line& aLine, const Parabola& aParab,
                            std::vector<SeedPnt>& pnts, int* layers ,
                            const int& sect,
                            int& nxFound,
                            int& nxExp, double& xLik) const;


      double binomialTerm(int sect,const double f, const double n, const double r) const;

      LHCb::OTChannelID channelHint(unsigned int station, unsigned int layer,
                                    std::vector<SeedPnt>& pnts) const;

      SeedParabolaFit* m_parabolaFit;

      double m_likCut;
      double m_maxDriftRadius;
      double m_itEff;
      double m_otEff;
      std::vector<double> m_f1Par;
      std::vector<double> m_f2Par;

      double m_logITEff;
      double m_logOTEff;
      double m_logITInEff;
      double m_logOTInEff;
      double m_f1Norm;
      double m_f2Norm;

      IOTExpectedHits* m_expectedHits;
      IITExpectedHits* m_expectedITHits;


      int m_uLayers[2];
      int m_xLayers[2];


    };

    inline LHCb::OTChannelID Likelihood::channelHint(unsigned int station, unsigned int layer,
                                                     std::vector<SeedPnt>& pnts) const{

      std::vector<SeedPnt>::iterator iter = pnts.begin();
      LHCb::OTChannelID testChan(station, layer, 0, 0, 0);
      while (iter != pnts.end() && iter->sameLayer(testChan) == false ){
        ++iter;
      } //iter

      return (iter == pnts.end() ? testChan : iter->hit()->OTChan() );
    }

    inline double Likelihood::binomialTerm(const int sect,const double f, const double n, const double r) const{
      // binomial term in likelihood
      return sect < 3 ? log( f) + r*m_logITEff + (n-r)*m_logITInEff:
        log( f) + r*m_logOTEff + (n-r)*m_logOTInEff;
    }

  }
}

#endif  // _TsaLikelihood_H




