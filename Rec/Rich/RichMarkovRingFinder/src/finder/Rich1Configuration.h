#ifndef RICHMARKOV_RICH1_CONFIGURATION_H
#define RICHMARKOV_RICH1_CONFIGURATION_H

#include <iostream>
#include <string>

#include "RichConfiguration.h"

namespace RichMarkov {
  
  class Rich1Configuration : public RichConfiguration {

  public:

  typedef enum { upperPane, lowerPane } Pane;

    Rich1Configuration( const Pane pane );

    inline double geometricXYAcceptanceBottomBound() const { return m_pane != upperPane ? -600 :  -30 ; }
    inline double geometricXYAcceptanceTopBound   () const { return m_pane != upperPane ?  30  :   600 ; }
    inline double geometricXYAcceptanceLeftBound  () const { return -730; }
    inline double geometricXYAcceptanceRightBound () const { return  730; }

    inline const double saturatedCircleTheta()       const { return Constants::saturatedCircleThetaRich1; }
    inline const double characteristicCircleRadius() const { return Constants::characteristicCircleRadiusRich1; }
    //inline const double viewRangeParameter()         const { return Constants::viewRangeParameterRich1; }
    inline const double realXYScaleFactor()          const { return Constants::realXYDataInputScaleFactorRich1; }
    inline double realXYDataInputScaleFactor()       const { return realXYScaleFactor(); }
    inline const double saturatedCircleRadiusInXYExternalData() const { return Constants::saturatedCircleRadiusInXYExternalDataRich1; }

    inline const double circleCenXMean()             const { return Constants::circleCenXMean; }
    inline const double circleCenYMean()             const { return Constants::circleCenYMean; }
    inline const double circleCenXSig()              const { return Constants::circleCenXSigRich1; }
    inline const double circleCenYSig()              const { return Constants::circleCenYSigRich1; }

    inline const double areaScaleForSignal()         const { return Constants::areaScaleForSignalRich1; }
    inline const double areaScaleForEverything()     const { return Constants::areaScaleForEverythingRich1; }

    inline const double backgroundMeanParameter()    const { return Constants::backgroundMeanParameterRich1; }
    inline const double backgroundRadius()           const { return Constants::backgroundRadiusRich1; }
    inline const double meanNumberOfRings()          const { return Constants::meanNumberOfRingsRich1; }
    inline const double circleHitsParameter()        const { return Constants::circleHitsParameterRich1; }
    inline const double circleMeanRadiusParameter()  const { return Constants::circleMeanRadiusParameterRich1; }
    inline const double circleRadiusSigmaAboutMean() const { return Constants::circleRadiusSigmaAboutMeanRich1; }

    inline const std::string approxCoPointSepCacheFileName() const 
    { 
      const std::string paramfilesroot( static_cast<std::string>(getenv("PARAMFILESROOT")) );
      return paramfilesroot + "/data/Rich1_MarkovRingFinder_approxCoPointSep.cache"; 
    }

  private:
    const Pane m_pane;

  };

  extern Rich1Configuration globalRich1LConfiguration; //declaration
  extern Rich1Configuration globalRich1UConfiguration; //declaration

}

#endif
