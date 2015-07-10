#ifndef TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H 
#define TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h" 

// from TrackInterfaces
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackInterfaces/IStateCorrectionTool.h"

/** @class MaterialLocatorBase MaterialLocatorBase.h
 *  
 *  A MaterialLocatorBase is a base class implementing methods
 *  from the IMaterialLocatorBase interface.
 *
 *  @author Wouter Hulsbergen 
 *  @date   12/05/2007
 */

class MaterialLocatorBase : public GaudiTool, virtual public IMaterialLocator 
{
public: 
  /// Constructor
  MaterialLocatorBase(const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent) ;
  
  /// intialize
  virtual StatusCode initialize();

  /// finalize
  virtual StatusCode finalize();
  
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint&, const Gaudi::XYZVector&, 
			    ILVolume::Intersections&) const = 0 ;
  
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
			    Intersections& intersepts) const ;

  /// Intersect a trajectory with volumes in the geometry
  virtual size_t intersect( const LHCb::ZTrajectory& traj, Intersections& intersepts ) const ;

  /// Intersect a trajectory interpolated between two statevectors with volumes in the geometry
  virtual size_t intersect( const LHCb::StateVector& origin, const LHCb::StateVector& target,
			    Intersections& intersepts ) const ;

  void applyMaterialCorrections(LHCb::State& stateAtTarget,
				const IMaterialLocator::Intersections& intersepts,
				double zorigin,
				LHCb::ParticleID pid,
				bool applyScatteringCorrection,
				bool applyEnergyLossCorrection) const ;
private:
  std::string m_dedxtoolname;
  std::string m_msCorrectionToolName;
  size_t m_maxNumIntervals ;
  double m_maxDeviation ;
  double m_maxDeviationAtRefstates ;
  double m_maxDeviationAtVeloRefstates ;
  IStateCorrectionTool* m_scatteringTool;
  IStateCorrectionTool* m_dedxtool;
  ToolHandle<IStateCorrectionTool> m_elecdedxtool;
};

#endif // TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H 
