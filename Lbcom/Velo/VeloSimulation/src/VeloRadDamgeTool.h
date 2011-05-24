#ifndef VeloRadDamageTool_H
#define VeloRadDamageTool_H

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

  /** destructer */
  virtual ~VeloRadDamageTool();

  /** Initialize */
  virtual StatusCode initialize();

  /** fraction of charge seen due to radiation damage
  * @param point : position (global frame) of hit
  * @return fraction of charge after rad damage
  */
  virtual double chargeFrac(const LHCb::MCHit & hit) const;
  
protected:

  DeVelo* m_veloDet; ///< Detector Element

  /// set up the tool by reading the conditions
  StatusCode i_loadCondtion();

  /// need the following per sensor to simulate radiation damage
  struct RadParam{
    /// zeroing constructor
    RadParam(): 
      responseSpline(0),
      rMin(0.),
      rMax(0.),
      constantFrac(1.),
      useConst(false),
      responseMin(0.),
      responseMax(0.){};


    /// The fitted spline to the radius
    GaudiMath::SimpleSpline* responseSpline; 
    
    double rMin; ///< First entry in vector of radii
    double rMax; ///< Last entry in vector of radii
    double constantFrac; ///< set if you want a constant output
    
    bool useConst; ///< use the constant value (or if false the spline)
    
    double responseMin; ///< to return if r < rMin
    double responseMax; ///< to return if r > rMax
  };

  /// dump the tool response to the log
  void dumpResponse(const DeVeloSensor *sens);

  /// each value corresponds to a sensor that needs a radiation tool 
  std::map<unsigned int,RadParam> m_radParamMap; 

  /// a map of the conditions verse sensor number
  std::map<unsigned int,Condition*> m_radConditionMap;

};

#endif // VeloRadDamageTool_H
