#ifndef TRACKTOOLS_STATEDETAILEDBETHEBLOCHENERGYCORRECTIONTOOL_H 
#define TRACKTOOLS_STATEDETAILEDBETHEBLOCHENERGYCORRECTIONTOOL_H 1

// Include files
// -------------

#include <unordered_map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IStateCorrectionTool.h"

// From PartProp
#include "Kernel/IParticlePropertySvc.h"

// from TrackEvent
#include "Event/State.h"

/** @class StateDetailedBetheBlochEnergyCorrectionTool
 *  
 *  This state correction tool applies a dE/dx energy loss correction
 *  with the full version of the Bethe-Bloch equation.
 *
 *  @author Stephanie Hansmann-Menzemer
 *  @date   2008-05-02
 *  
 */
class StateDetailedBetheBlochEnergyCorrectionTool : public GaudiTool,
                                                  virtual public IStateCorrectionTool {
public:
  /// Correct a State for dE/dx energy losses with a simplified Bethe-Bloch equiaton
  void correctState( LHCb::State& state,
                     const Material* material,
                     double wallThickness = 0,
                     bool upstream = true,
                     LHCb::ParticleID pid = LHCb::ParticleID(211) ) const ;
  
  /// Standard constructor
  StateDetailedBetheBlochEnergyCorrectionTool( const std::string& type, 
                                             const std::string& name,
                                             const IInterface* parent);
  
  virtual ~StateDetailedBetheBlochEnergyCorrectionTool( ); ///< Destructor
  
  StatusCode initialize();
  
protected:
  
private:
  // Job options
  double m_energyLossCorr;      ///< tunable energy loss correction
  double m_maxEnergyLoss;       ///< maximum energy loss in dE/dx correction
  LHCb::IParticlePropertySvc* m_pp;   /// particle property service

  typedef std::unordered_map<unsigned, double> PID2MassMap;
  enum class Mat { X0, C, X1, a, m, DensityFactor, LogI };
  typedef std::unordered_map<const Material*, std::tuple<
      double, double, double, double, double, double, double> >
      Material2FactorMap;

  mutable PID2MassMap m_pid2mass;
  mutable Material2FactorMap m_material2factors;
  mutable PID2MassMap::iterator m_lastCachedPID;
  mutable Material2FactorMap::iterator m_lastCachedMaterial;
  //mutable GaudiUtils::VectorMap<LHCb::ParticleID,double> m_pid2mass;
};
#endif // TRACKTOOLS_STATEDETAILEDBETHEBLOChENERGYCORRECTIONTOOL_H
