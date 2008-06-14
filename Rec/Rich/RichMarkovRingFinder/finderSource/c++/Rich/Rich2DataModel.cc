
#include "Rich/Rich2DataModel.h"
#include "Utils/MathsConstants.h"

namespace Lester
{

  Rich2DataModel::Rich2DataModel() : RichDataModel() { }

  void Rich2DataModel::initialise()
  {
   // f(a,e) = page 81 Eq 3.6 http://doc.cern.ch/archive/electronic/cern/preprints/thesis/thesis-2005-058.pdf

    //circleMeanRadiusParameter  = 0.030; // old
    circleMeanRadiusParameter  = 0.0283; // new

    backgroundRadius        = circleMeanRadiusParameter*3.0;
    backgroundMeanParameter = 75;
    meanNumberOfRings       = 7;
    circleHitsPerUnitLengthParameter = 30.0/(MathsConstants::twoPi*circleMeanRadiusParameter);
    circleRadiusSigmaAboutMean       = circleMeanRadiusParameter*0.1;
    circleCenXMean          = 0.;
    circleCenYMean          = 0.;
    circleCenXSig           = circleMeanRadiusParameter*1.5;
    circleCenYSig           = circleMeanRadiusParameter*1.5; 
    circleProbabilityDistributionAlphaParameter   = 2;
    circleProbabilityDistributionEpsilonParameter = 0.05;
    areaScaleForSignal      = MathsConstants::pi*circleCenXSig*circleCenYSig;
    areaScaleForEverything  = areaScaleForSignal; // since the signal and background distributions more-or-less over the same area

    m_thetaDataFile = "rich2_thetaDist.txt";
    m_cacheDataFile = "rich2_approxCoPointSep.cache";

    RichDataModel::initialise();
  }

}
