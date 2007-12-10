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

  void computeMaterialCorrection(Gaudi::TrackSymMatrix& noise,
				 Gaudi::TrackVector& delta,
				 const IMaterialLocator::Intersections& intersepts,
				 double zorigin,
				 double ztarget,
				 double momentum,
				 LHCb::ParticleID pid) const ;
  
private:
  size_t m_maxNumIntervals ;
  double m_maxDeviation ;
  double m_maxDeviationAtRefstates ;
  ToolHandle<IStateCorrectionTool> m_scatteringtool;
  ToolHandle<IStateCorrectionTool> m_dedxtool;
  ToolHandle<IStateCorrectionTool> m_elecdedxtool;
};

#endif // TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H 
