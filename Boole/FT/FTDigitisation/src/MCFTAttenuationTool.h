#ifndef MCFTATTENUATIONTOOL_H 
#define MCFTATTENUATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
// FTDet
#include "FTDet/DeFTDetector.h"

#include "IMCFTAttenuationTool.h"            // Interface

/** @class MCFTAttenuationTool MCFTAttenuationTool.h
 *  
 *  Tool to calculate the attenuation length for direct and reflectec light in a scintillating fibre.
 *
 *  - ShortAttenuationLength: Distance along the fibre to divide the light amplitude by a factor e : short component
 *  - LongAttenuationLength: Distance along the fibre to divide the light amplitude by a factor e : long component
 *  - FractionShort: Fraction of short attenuation at SiPM
 *  - XMaxIrradiatedZone: Size in x of the zone where fibres are irradiated
 *  - YMaxIrradiatedZone: Size in y of the zone where fibres are irradiated
 *  - IrradiatedAttenuationLength:  Attenuation length by steps in the zone
 *  - ReflectionCoefficient: Reflection coefficient of the fibre mirrored side, from 0 to 1
 *  - BeginReflectionLossY:  Beginning of zone where reflection is too late and lost
 *  - EndReflectionLossY: End of zone where reflection is too late and lost
 *  - XStepOfMap:  Step  along x-axis of the FullAttenuationMap (in mm)
 *  - YStepOfMap:  Step  along y-axis of the FullAttenuationMap (in mm)
 *
 *
 *
 *  @author Michel De Cian, based on Eric Cogneras' implementation
 *  @date   2015-01-15
 */


class MCFTAttenuationTool : public GaudiHistoTool, virtual public IMCFTAttenuationTool {

public: 
  /// Standard constructor
  MCFTAttenuationTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~MCFTAttenuationTool( ); ///< Destructor

  /// Initialize the transmission map
  StatusCode initializeTool();
  
  /// Calculate the direct attenuation and the attenuation with reflection
  void attenuation(const LHCb::MCHit* ftHit, double& att, double& attRef);


protected:

private:

  inline double calcAtt(const double fracX, const double fracY, const int kx, const int ky) const{
    return fracX * ( fracY     * m_transmissionMap[m_nYSteps*(kx+1)+ky+1] + 
                     ( 1-fracY ) * m_transmissionMap[m_nYSteps*(kx+1)+ky]   ) +
      (1-fracX) * ( fracY     * m_transmissionMap[m_nYSteps*kx+ky+1] + 
                    (1-fracY) * m_transmissionMap[m_nYSteps*kx+ky]   );
  }
  
  inline double calcAttRef(const double fracX, const double fracY, const int kx, const int ky)const{
    return fracX * ( fracY     * m_transmissionRefMap[m_nYSteps*(kx+1)+ky+1] + 
                     ( 1-fracY ) * m_transmissionRefMap[m_nYSteps*(kx+1)+ky]   ) +
      (1-fracX) * ( fracY     * m_transmissionRefMap[m_nYSteps*kx+ky+1] + 
                    (1-fracY) * m_transmissionRefMap[m_nYSteps*kx+ky]   );
  }


  double       m_xStepOfMap;               ///< Step  along X-axis of the FullAttenuationMap(in mm)
  double       m_yStepOfMap;               ///< Step  along Y-axis of the FullAttenuationMap(in mm)
  int          m_nXSteps;
  int          m_nYSteps;
  double       m_xMax;
  double       m_yMax;
  std::vector<double> m_transmissionMap;    ///< Maps hits to transmitted energy from the direct pulse
  std::vector<double> m_transmissionRefMap; ///< Maps hits to transmitted energy from the reflected pulse
  double       m_xMaxIrradiatedZone;     ///< Size in x of the zone where fibres are irradiated
  double       m_yMaxIrradiatedZone;     ///< Size in y of the zone where fibres are irradiated
  double       m_shortAttenuationLength; ///< Attenuation lengh of the light along the fibre : short component
  double       m_longAttenuationLength;  ///< Attenuation lengh of the light along the fibre : long component
  double       m_fractionShort;          ///< Fraction of short attenuation length at SiPM
  std::vector<double> m_irradiatedAttenuationLength;    ///< Attenuation length by steps in the zone.
  double       m_reflectionCoefficient;   ///< reflection coefficient of the mirror at the y=0 side of the fibre
  double       m_beginReflectionLossY;    ///< begin zone where reflectio is too late and lost
  double       m_endReflectionLossY;      ///< end of this zone
  

  DeFTDetector* m_deFT; ///< pointer to FT detector description
  
};
#endif // MCFTATTENUATIONTOOL_H
