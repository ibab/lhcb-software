#ifndef RICHMARKOV_CONSTANTS_H
#define RICHMARKOV_CONSTANTS_H

#include "MathsConstants/MathsConstants.h"


namespace RichMarkov {

  namespace Constants {

    typedef enum { simpleModel, hardModel, rich1A, rich2A } Scenario;
    const Scenario scenario = rich2A;

    typedef enum { lesterFirst, topHat, power1 } ProbabilityFunc;
    const ProbabilityFunc probFuncType = lesterFirst;
    //const ProbabilityFunc probFuncType = topHat;
    //const ProbabilityFunc probFuncType = power1;

    // Scale factors
    // with this scale factor, circles in input XY data will have radii which can be considered measured
    // in radians.  In other words, all "far screen" coordinates may be considered measured in radians 
    // from iteraction point
    const bool   useRealData( true );
    const double saturatedCircleThetaRich1( 0.052 ); //radians! // needs changed!
    const double saturatedCircleThetaRich2( 0.030 ); //radians!
    const double saturatedCircleRadiusInXYExternalDataRich1(  96.0 );
    const double saturatedCircleRadiusInXYExternalDataRich2( 128.0 );
    const double realXYDataInputScaleFactorRich1( saturatedCircleThetaRich1 / saturatedCircleRadiusInXYExternalDataRich1 );  // needs changed!
    const double realXYDataInputScaleFactorRich2( saturatedCircleThetaRich2 / saturatedCircleRadiusInXYExternalDataRich2 );

    // provides a rough handle of circle size for things that want to be able
    // to jitter say 10% of a typical circle size ... unit used for jitters etc. ...
    const double characteristicCircleRadiusRich1( saturatedCircleThetaRich1 );
    const double characteristicCircleRadiusRich2( saturatedCircleThetaRich2 );

    const double viewRangeParameterRich1( 6 * characteristicCircleRadiusRich1 ); // lets aim to see a width and height of about 6 circles ...
    const double viewRangeParameterRich2( 6 * characteristicCircleRadiusRich2 ); // lets aim to see a width and height of about 6 circles ...
    //const bool   useOldNewPointMethod( false );
    const bool   useNewThreePointMethod( true );

    const double backgroundRadius( 0.030 * 3.0 );
    const double backgroundMeanParameter( 34.0 /* number of hits in a saturated circle*/ * 1.5 /*number of "rubbish circle equivalents" we expect to have to endure */ ); // was 10;
    const int    meanNumberOfRings( 21 ); // was 10;
    const double circleHitsParameter( 37500 ); // number of hits per circle is approximately given by 37500*r*r (r is measured in radians, of course!) 
    //const double circleHitsPerUnitLengthParameter = 30./*approx*/ / (pi*2.*0.030); // 3.; // was 1
    const double circleMeanRadiusParameter( 0.030 ); // was 1.0;
    const double circleRadiusSigmaAboutMean( 0.1 * circleMeanRadiusParameter ); // approx guess!

    // The following legacy parts are soon to be deleted!
    const double geometricXYAcceptanceLeftBoundLEGACYRich1   ( -350 );
    const double geometricXYAcceptanceRightBoundLEGACYRich1  ( +350 );
    const double geometricXYAcceptanceBottomBoundLEGACYRich1 ( -700 );
    const double geometricXYAcceptanceTopBoundLEGACYRich1    ( +700 );
    const double geometricXYAcceptanceLeftBoundLEGACYRich2   ( -350 );
    const double geometricXYAcceptanceRightBoundLEGACYRich2  ( +350 );
    const double geometricXYAcceptanceBottomBoundLEGACYRich2 ( -700 );
    const double geometricXYAcceptanceTopBoundLEGACYRich2    ( +700 );
    const double geometricThetaAcceptanceLeftBoundLEGACYRich1( realXYDataInputScaleFactorRich1 * geometricXYAcceptanceLeftBoundLEGACYRich1 );
    const double geometricThetaAcceptanceRightBoundLEGACYRich1( realXYDataInputScaleFactorRich1 * geometricXYAcceptanceRightBoundLEGACYRich1 );
    const double geometricThetaAcceptanceBottomBoundLEGACYRich1( realXYDataInputScaleFactorRich1 * geometricXYAcceptanceBottomBoundLEGACYRich1 );
    const double geometricThetaAcceptanceTopBoundLEGACYRich1( realXYDataInputScaleFactorRich1 * geometricXYAcceptanceTopBoundLEGACYRich1 );
    const double geometricThetaAcceptanceLeftBoundLEGACYRich2( realXYDataInputScaleFactorRich2 * geometricXYAcceptanceLeftBoundLEGACYRich2 );
    const double geometricThetaAcceptanceRightBoundLEGACYRich2( realXYDataInputScaleFactorRich2 * geometricXYAcceptanceRightBoundLEGACYRich2 );
    const double geometricThetaAcceptanceBottomBoundLEGACYRich2( realXYDataInputScaleFactorRich2 * geometricXYAcceptanceBottomBoundLEGACYRich2 );
    const double geometricThetaAcceptanceTopBoundLEGACYRich2( realXYDataInputScaleFactorRich2 * geometricXYAcceptanceTopBoundLEGACYRich2 );
    const double circleCenXMean( 0.0 );
    const double circleCenYMean( 0.0 );
    const double circleCenXSigRich1( 0.5 * 0.5 * (-geometricThetaAcceptanceLeftBoundLEGACYRich1 + geometricThetaAcceptanceRightBoundLEGACYRich1) ); //was 0.030*3.;
    const double circleCenYSigRich1( 0.5 * 0.5 * (-geometricThetaAcceptanceBottomBoundLEGACYRich1 + geometricThetaAcceptanceTopBoundLEGACYRich1) ); //was 0.030*3.;
    const double circleCenXSigRich2( 0.5 * 0.5 * (-geometricThetaAcceptanceLeftBoundLEGACYRich2 + geometricThetaAcceptanceRightBoundLEGACYRich2) ); //was 0.030*3.;
    const double circleCenYSigRich2( 0.5 * 0.5 * (-geometricThetaAcceptanceBottomBoundLEGACYRich2 + geometricThetaAcceptanceTopBoundLEGACYRich2) ); //was 0.030*3.;
    //std::cout << "bjksd: (" << circleCenXSigRich2 / realXYDataInputScaleFactorRich2 << ", " << circleCenYSigRich2 / realXYDataInputScaleFactorRich2 << ")" << std::endl;
    
    const double circleProbabilityDistributionAlphaParameter( 2 ); // or try something else!
    const double circleProbabilityDistributionEpsilonParameter( 0.05 ); // was 0.05; // or try something else!
    const bool   usePrior( true );

    const double areaScaleForSignalRich1( MathsConstants::pi * circleCenXSigRich1 * circleCenYSigRich1 );
    const double areaScaleForSignalRich2( MathsConstants::pi * circleCenXSigRich2 * circleCenYSigRich2 );
    const double areaScaleForEverythingRich1( areaScaleForSignalRich1 ); // since the signal and background distributions more-or-less over the same area
    const double areaScaleForEverythingRich2( areaScaleForSignalRich2 ); // since the signal and background distributions more-or-less over the same area

    // Discover what the hell these do!
    //const double upDown_ordinaryWeight( 1 );
    //const double upDown_special2Weight( 0.05 );
    //const double upDown_special3Weight( 0.2 );
    //const double upDown_totWeight( upDown_ordinaryWeight + upDown_special2Weight + upDown_special3Weight );
    //const double upDown_alphaOrdinary( upDown_ordinaryWeight / upDown_totWeight );
    //const double upDown_alphaSpecial2( upDown_special2Weight / upDown_totWeight );
    //const double upDown_alphaSpecial3( upDown_special3Weight / upDown_totWeight );
    //const double upDown_Q( 0.5 );

    // const bool   showSomeIdeas( false );
    //const unsigned int numberOfFramesBetweenIdeas( 25 );
    //const unsigned int numberOfIterationsPerFrame( 500 ); // was 500
    //const unsigned int numberOfFrames( 3 );
    const unsigned int DefaultNumberOfIterations( 1500 );

    const double circleDeletionProb( 0.4 );
  }

}

#endif
