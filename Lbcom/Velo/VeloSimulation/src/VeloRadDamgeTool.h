#ifndef VeloRadDamageTool_H
#define VeloRadDamageTool_H

#include <memory>

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"

// conditons
#include "DetDesc/Condition.h"

// VeloDet
#include "VeloDet/DeVelo.h"

// Interface
#include "IRadDamageTool.h"

/** @class VeloRadDamgeTool VeloRadDamgeTool.h
 *
 *  Tool to simulate the radiation damage effects to the deposited charge
 *
 *  @author D HutchcroftV
 *  @date   29/3/2011
 */

class VeloRadDamageTool : public GaudiTool,
                          virtual public IRadDamageTool {

public:

  /** Constructer */
  VeloRadDamageTool(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  /** Initialize */
  StatusCode initialize() override;

  /** fraction of charge seen due to radiation damage
  * @param point : position (global frame) of hit
  * @return fraction of charge after rad damage
  */
  double chargeFrac(const LHCb::MCHit & hit) const override;

  /** fraction of charge seen due to radiation damage
  * @param point : position (global frame) of hit
  * @param sens : DeVeloSensor of the point
  * @param chM2 : the channel of the inner strip
  * @param fracInner : fraction of charge to inner strip
  * @param fracMain : fraction of charge from main strip
  * @return Success if coupled to inner strip
  */
  StatusCode m2CouplingFrac(const Gaudi::XYZPoint &point,
                                    const DeVeloSensor *sens,
                                    LHCb::VeloChannelID &chM2,
                                    double &fracInner,
                                    double &fracMain) const override;


private:

  DeVelo* m_veloDet = nullptr; ///< Detector Element

  /// set up the tool by reading the conditions
  StatusCode i_loadCondtion();

  /// need the following per sensor to simulate radiation damage
  struct RadParam final{
    RadParam() = default;

    /// The fitted spline to the radius
    std::unique_ptr<GaudiMath::SimpleSpline> responseSpline;

    double rMin = 0.; ///< First entry in vector of radii
    double rMax = 0.; ///< Last entry in vector of radii
    double constantFrac = 1.; ///< set if you want a constant output

    bool useConst = false; ///< use the constant value (or if false the spline)

    double responseMin = 0.; ///< to return if r < rMin
    double responseMax = 0.; ///< to return if r > rMax

    double chargeFracWidth = 1.; ///< Coupling to metal 2 width
    double chargeFracMax = 0.; ///< coupling to metal 2 max fraction
    double stripDistScale =1.; ///< size of mask caused by 1st metal layer
    double fracOuter = 0.; ///< fraction of inner charge to remove from outer
  };

  /// dump the tool response to the log
  void dumpResponse(const DeVeloSensor& sens) const;

  /// each value corresponds to a sensor that needs a radiation tool
  std::map<unsigned int,RadParam> m_radParamMap;

  /// a map of the conditions verse sensor number
  std::map<unsigned int,Condition*> m_radConditionMap;

};

#endif // VeloRadDamageTool_H
