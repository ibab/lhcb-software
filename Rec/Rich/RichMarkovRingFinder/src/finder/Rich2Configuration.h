#ifndef RICHMARKOV_RICH2_CONFIGURATION_H
#define RICHMARKOV_RICH2_CONFIGURATION_H

#include "RichConfiguration.h"

namespace RichMarkov {
  
  class Rich2Configuration : public RichConfiguration {

  public:

    typedef enum { leftPane, rightPane } Pane;

    Rich2Configuration( const Pane pane );

    inline double geometricXYAcceptanceBottomBound() const { return -730; }; // guessed extension of RICH acceptance
    inline double geometricXYAcceptanceTopBound   () const { return +730; }; // and here too
    inline double geometricXYAcceptanceLeftBound  () const { return m_pane != leftPane ? -700 :  -30 ; };
    inline double geometricXYAcceptanceRightBound () const { return m_pane != leftPane ?  30  :   700 ; };

    inline const double saturatedCircleTheta()       const { return Constants::saturatedCircleThetaRich2; }
    inline const double characteristicCircleRadius() const { return Constants::characteristicCircleRadiusRich2; }
    inline const double viewRangeParameter()         const { return Constants::viewRangeParameterRich2; }
    inline const double realXYScaleFactor()          const { return Constants::realXYDataInputScaleFactorRich2; }
    inline double realXYDataInputScaleFactor()       const { return realXYScaleFactor(); }
    inline const double saturatedCircleRadiusInXYExternalData() const { return Constants::saturatedCircleRadiusInXYExternalDataRich2; }

    inline const double circleCenXMean()             const { return Constants::circleCenXMean; }
    inline const double circleCenYMean()             const { return Constants::circleCenYMean; }
    inline const double circleCenXSig()              const { return Constants::circleCenXSigRich2; }
    inline const double circleCenYSig()              const { return Constants::circleCenYSigRich2; }

    inline const double areaScaleForSignal()         const { return Constants::areaScaleForSignalRich2; }
    inline const double areaScaleForEverything()     const { return Constants::areaScaleForEverythingRich2; }

    inline const double backgroundMeanParameter()    const { return Constants::backgroundMeanParameterRich2; }
    inline const double backgroundRadius()           const { return Constants::backgroundRadiusRich2; }
    inline const double meanNumberOfRings()          const { return Constants::meanNumberOfRingsRich2; }
    inline const double circleHitsParameter()        const { return Constants::circleHitsParameterRich2; }
    inline const double circleMeanRadiusParameter()  const { return Constants::circleMeanRadiusParameterRich2; }
    inline const double circleRadiusSigmaAboutMean() const { return Constants::circleRadiusSigmaAboutMeanRich2; }

  private:
    const Pane m_pane;

  };

  extern Rich2Configuration globalRich2LConfiguration; //declaration
  extern Rich2Configuration globalRich2RConfiguration; //declaration

}

#endif
