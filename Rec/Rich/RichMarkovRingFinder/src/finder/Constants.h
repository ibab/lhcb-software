#ifndef RICHMARKOV_CONSTANTS_H
#define RICHMARKOV_CONSTANTS_H

#include "MathsConstants/MathsConstants.h"


namespace RichMarkov {

  namespace Constants {

    typedef enum { simpleModel, hardModel, rich1A, rich2A } Scenario;
    const Scenario scenario = rich2A;

    typedef enum { lesterFirst, topHat, power1 } ProbabilityFunc;
    const ProbabilityFunc probFuncType = lesterFirst; // or "topHat" or "power1"...

    // Runtime internal configs
    const bool   useRealData( true );
    const bool   useNewThreePointMethod( true );
    const bool   usePrior( true );
    //const double viewRangeParameterRich1( 6 * characteristicCircleRadiusRich1 ); // lets aim to see a width and height of about 6 circles ...
    //const double viewRangeParameterRich2( 6 * characteristicCircleRadiusRich2 ); // lets aim to see a width and height of about 6 circles ...
    const double circleProbabilityDistributionAlphaParameter( 2 );
    const double circleProbabilityDistributionEpsilonParameter( 0.05 );
    const unsigned int DefaultNumberOfIterations( 1500 );
    const double circleDeletionProb( 0.4 );

    // Scale factors
    // with this scale factor, circles in input XY data will have radii which can be considered measured
    // in radians.  In other words, all "far screen" coordinates may be considered measured in radians
    // from iteraction point
    const double saturatedCircleThetaRich1( 0.052 ); //radians
    const double saturatedCircleThetaRich2( 0.030 ); //radians
    const double saturatedCircleRadiusInXYExternalDataRich1(  96.0 ); // double-check
    const double saturatedCircleRadiusInXYExternalDataRich2( 128.0 );
    const double realXYDataInputScaleFactorRich1( saturatedCircleThetaRich1 / saturatedCircleRadiusInXYExternalDataRich1 );
    const double realXYDataInputScaleFactorRich2( saturatedCircleThetaRich2 / saturatedCircleRadiusInXYExternalDataRich2 );
    // provides a rough handle of circle size for things that want to be able
    // to jitter say 10% of a typical circle size ... unit used for jitters etc. ...
    const double characteristicCircleRadiusRich1( saturatedCircleThetaRich1 );
    const double characteristicCircleRadiusRich2( saturatedCircleThetaRich2 );
    const double backgroundRadiusRich1( saturatedCircleThetaRich1 * 3.0 ); //??? ask Lester
    const double backgroundRadiusRich2( saturatedCircleThetaRich2 * 3.0 ); // "
    const double backgroundMeanParameterRich1( 34.0 /* number of hits in a saturated circle*/ * 1.5 /*number of "rubbish circle equivalents" we expect to have to endure */ ); // ??? ask Lester
    const double backgroundMeanParameterRich2( 34.0 /* number of hits in a saturated circle*/ * 1.5 /*number of "rubbish circle equivalents" we expect to have to endure */ ); // ???  "
    const int    meanNumberOfRingsRich1( 25 ); // ??? check
    const int    meanNumberOfRingsRich2( 21 );
    const double circleHitsParameterRich1( 37500 ); // number of hits per circle is approximately given by 37500*r*r (r is measured in radians, of course!)
    const double circleHitsParameterRich2( 37500 ); // number of hits per circle is approximately given by 37500*r*r (r is measured in radians, of course!)
    const double circleMeanRadiusParameterRich1( 0.030 );
    const double circleMeanRadiusParameterRich2( 0.030 );
    const double circleRadiusSigmaAboutMeanRich1( 0.1 * circleMeanRadiusParameterRich1 ); // change!
    const double circleRadiusSigmaAboutMeanRich2( 0.1 * circleMeanRadiusParameterRich2 ); // just a guess!
    const double circleCenXMean( 0.0 );
    const double circleCenYMean( 0.0 );
    const double circleCenXSigRich1( 0.25 * realXYDataInputScaleFactorRich1 * 700 );
    const double circleCenYSigRich1( 0.25 * realXYDataInputScaleFactorRich1 * 1400 );
    const double circleCenXSigRich2( 0.25 * realXYDataInputScaleFactorRich2 * 700 );
    const double circleCenYSigRich2( 0.25 * realXYDataInputScaleFactorRich2 * 1400 );
    const double areaScaleForSignalRich1( MathsConstants::pi * circleCenXSigRich1 * circleCenYSigRich1 );
    const double areaScaleForSignalRich2( MathsConstants::pi * circleCenXSigRich2 * circleCenYSigRich2 );
    const double areaScaleForEverythingRich1( areaScaleForSignalRich1 ); // since the signal and background distributions more-or-less over the same area
    const double areaScaleForEverythingRich2( areaScaleForSignalRich2 ); // since the signal and background distributions more-or-less over the same area
  }

}

#endif
