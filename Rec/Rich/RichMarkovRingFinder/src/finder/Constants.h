#ifndef LESTER_CONSTANTS_H
#define LESTER_CONSTANTS_H

#include "MathsConstants/MathsConstants.h"

namespace Lester {

  namespace Constants {

    typedef enum { simpleModel, hardModel, rich1A, rich2A } Scenario;
    const Scenario scenario = rich2A;

    typedef enum { lesterFirst, topHat, power1 } ProbabilityFunc;
    const ProbabilityFunc probFuncType = lesterFirst;
    //const ProbabilityFunc probFuncType = topHat;
    //const ProbabilityFunc probFuncType = power1;

    const bool useRealData=true;
    const double saturatedCircleTheta=0.030; //radians!
    const double saturatedCircleRadiusInXYExternalData=128.;
    
    const double realXYDataInputScaleFactor=saturatedCircleTheta/saturatedCircleRadiusInXYExternalData; // with this scale factor, circles in input XY data will have radii which can be considered measured in radians.  In other words, all "far screen" coordinates may be considered measured in radians from iteraction point.
    const double characteristicCircleRadius=saturatedCircleTheta; // provides a rough handle of circle size for things that want to be able to jitter say 10% of a typical circle size ... unit used for jitters etc ...
    const double viewRangeParameter=characteristicCircleRadius*6; // lets aim to see a width and height of about 6 circles ...
    const bool useOldNewPointMethod=false;
    const bool useNewThreePointMethod=true;

    const double backgroundRadius=0.030*3.;
    const double backgroundMeanParameter=34./*number of hits in a saturated circle*/ * 1.5 /*number of "rubbish circle equivalents" we expect to have to endure*/; // was 10;
    const int meanNumberOfRings=21; // was 10;
    const double circleHitsParameter=37500; // number of hits per circle is approximately given by 37500*r*r (r is measured in radians, of course!) 
    //const double circleHitsPerUnitLengthParameter=30./*approx*/ / (pi*2.*0.030); // 3.; //was1
    const double circleMeanRadiusParameter=0.030; // 1.;
    const double circleRadiusSigmaAboutMean = circleMeanRadiusParameter*0.1; //approx guess!

    // The following legacy parts are soon to be deleted!
    const double geometricXYAcceptanceLeftBoundLEGACY   = -350;
    const double geometricXYAcceptanceRightBoundLEGACY  = +350;
    const double geometricXYAcceptanceBottomBoundLEGACY = -700;
    const double geometricXYAcceptanceTopBoundLEGACY    = +700;
    const double geometricThetaAcceptanceLeftBoundLEGACY 
      = geometricXYAcceptanceLeftBoundLEGACY  *realXYDataInputScaleFactor;
    const double geometricThetaAcceptanceRightBoundLEGACY  
      = geometricXYAcceptanceRightBoundLEGACY *realXYDataInputScaleFactor;
    const double geometricThetaAcceptanceBottomBoundLEGACY 
      = geometricXYAcceptanceBottomBoundLEGACY*realXYDataInputScaleFactor;
    const double geometricThetaAcceptanceTopBoundLEGACY 
      = geometricXYAcceptanceTopBoundLEGACY   *realXYDataInputScaleFactor;
    

    const double circleCenXMean=0.;
    const double circleCenYMean=0.;
    const double circleCenXSig=0.5*0.5*(-geometricThetaAcceptanceLeftBoundLEGACY+geometricThetaAcceptanceRightBoundLEGACY);// was 0.030*3.;
    const double circleCenYSig=0.5*0.5*(-geometricThetaAcceptanceBottomBoundLEGACY+geometricThetaAcceptanceTopBoundLEGACY);// was 0.030*3.;

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

    const bool showSomeIdeas = false;
    const unsigned int numberOfFramesBetweenIdeas = 25;
    const unsigned int numberOfIterationsPerFrame = 500; // was 500
    const unsigned int numberOfFrames = 3;

    const double circleDeletionProb = 0.4;

  }

}

#endif
