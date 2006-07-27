
#undef USEROOT

#ifndef LESTER_CONSTANTS_H
#define LESTER_CONSTANTS_H

#include "Utils/MathsConstants.h"

namespace Lester {
  namespace Constants {
    const bool warpedView=true;
    typedef enum { simpleModel, hardModel, rich1A, rich2A } Scenario;
    const Scenario scenario = rich2A;

    const bool useRealData=true;
    const double realDataInputScaleFactor=0.030/128.;
    const double whiffWhaff=0.030; // unit used for jitters etc ...
    const double viewRangeParameter=0.030*5;
    const bool useOldNewPointMethod=false;
    const bool useNewThreePointMethod=true;

    const double backgroundRadius=0.030*3.;
    const double backgroundMeanParameter=10;//10;
    const int meanNumberOfRings=10; //;
    const double circleHitsPerUnitLengthParameter=30./*approx*/ / (MathsConstants::twoPi*0.030); // 3.; //was1
    const double circleMeanRadiusParameter=0.030; // 1.;
    const double circleRadiusSigmaAboutMean = circleMeanRadiusParameter*0.1; //approx guess!
    const double circleCenXMean=0.;
    const double circleCenYMean=0.;
    const double circleCenXSig=0.030*3.; //
    const double circleCenYSig=0.030*3.;
    const double circleProbabilityDistributionAlphaParameter=2; // or try something else!
    const double circleProbabilityDistributionEpsilonParameter=0.05; // was 0.05; // or try something else!
    const bool usePrior=true;

    const double areaScaleForSignal = MathsConstants::pi*circleCenXSig*circleCenYSig;
    const double areaScaleForEverything = areaScaleForSignal; // since the signal and background distributions more-or-less over the same area

    const double upDown_ordinaryWeight=1;
    const double upDown_special2Weight=0.05;
    const double upDown_special3Weight=0.2;
    const double upDown_totWeight=upDown_ordinaryWeight+upDown_special2Weight+upDown_special3Weight;
    const double upDown_alphaOrdinary = upDown_ordinaryWeight/upDown_totWeight;
    const double upDown_alphaSpecial2 = upDown_special2Weight/upDown_totWeight;
    const double upDown_alphaSpecial3 = upDown_special3Weight/upDown_totWeight;
    const double upDown_Q = 0.5;

    const bool showSomeIdeas = true;
    const bool numberOfFramesBetweenIdeas =25; //25;
    const int numberOfFramesBetweenUpdates = 500; //500;

    const double circleDeletionProb = 0.4;
  };
};
#endif
