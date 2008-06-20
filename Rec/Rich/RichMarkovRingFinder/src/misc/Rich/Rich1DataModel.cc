
#include "Rich/Rich1DataModel.h"
#include "Utils/MathsConstants.h"

namespace Lester
{

  Rich1DataModel::Rich1DataModel() : RichDataModel() { m_rad = Rich::Rich1Gas; }

  void Rich1DataModel::initialise()
  {
    // f(a,e) = page 81 Eq 3.6 http://doc.cern.ch/archive/electronic/cern/preprints/thesis/thesis-2005-058.pdf
    circleMeanRadiusParameter  = 0.047;

    backgroundRadius        = circleMeanRadiusParameter*3.0;
    backgroundMeanParameter = 75;
    meanNumberOfRings       = 5;
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

    m_thetaDataFile = "rich1_thetaDist.txt";
    m_cacheDataFile = "rich1_approxCoPointSep.cache";

    RichDataModel::initialise();
  }

}
