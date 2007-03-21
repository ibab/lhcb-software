// $ Exp $
#ifndef _TsaLikelihood_H
#define _TsaLikelihood_H

/** @class TsaLikelihood TsaLikelihood.h
* Follow track and pick up hits
* @author M. Needham
**/

#include "TsaKernel/ITsaSeedStep.h"
#include "GaudiAlg/GaudiTool.h"

#include <string>
#include <vector>

class IOTExpectedHits;
class IITExpectedHits;
class SeedParabolaFit;

namespace Tsa{
  class Line;
  class Parabola;
}

class SeedPnt;
class SeedHit;
class SeedTrack;

class TsaLikelihood: public GaudiTool,
                       virtual public ITsaSeedStep{

public:

  /// constructer
  TsaLikelihood(const std::string& type,
               const std::string& name,
               const IInterface* parent);

  // destructer
  virtual ~TsaLikelihood();

  virtual StatusCode initialize();

  // execute method
  virtual StatusCode execute(std::vector<SeedTrack*>& seeds,  std::vector<SeedHit*> hits[6] = 0);

  virtual StatusCode execute(LHCb::State& hint, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] = 0) ;

private:

 void expectationIT(const Tsa::Line& aLine, const Tsa::Parabola& aParab, 
                    std::vector<SeedPnt>& pnts, int* layers, const int& sect,
                    int& nFound, 
                    int& nExp) const;

 void expectationOT(const Tsa::Line& aLine, const Tsa::Parabola& aParab, 
                     std::vector<SeedPnt>& pnts, int* layers ,
                     const int& sect,
                     int& nxFound, 
                     int& nxExp, double& xLik) const;

 void expectationMixed(const Tsa::Line& aLine, const Tsa::Parabola& aParab, 
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

#include "TsaKernel/SeedPnt.h"
#include "TsaKernel/SeedHit.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/LHCbID.h"

inline LHCb::OTChannelID TsaLikelihood::channelHint(unsigned int station, unsigned int layer,
                                             std::vector<SeedPnt>& pnts) const{
   
 std::vector<SeedPnt>::iterator iter = pnts.begin();
 LHCb::OTChannelID testChan(station, layer, 0, 0, 0); 
 while (iter != pnts.end() && iter->sameLayer(testChan) == false ){
   ++iter;
 } //iter

 return (iter == pnts.end() ? testChan : iter->hit()->OTChan() );
}

inline double TsaLikelihood::binomialTerm(const int sect,const double f, const double n, const double r) const{
 // binomial term in likelihood
 return sect < 3 ? log( f) + r*m_logITEff + (n-r)*m_logITInEff:
                   log( f) + r*m_logOTEff + (n-r)*m_logOTInEff;
}

#endif  // _TsaLikelihood_H

  


