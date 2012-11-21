#ifndef TRACKEXTRAPOLATORS_TRACKFASTMATERIALINTERSECTOR_H 
#define TRACKEXTRAPOLATORS_TRACKFASTMATERIALINTERSECTOR_H

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "MaterialLocatorBase.h"

// from std
#include <vector>

namespace MaterialLocatorUtils {
  class PVolumeWrapper ;
}

/** @class SimplifiedMaterialLocator SimplifiedMaterialLocator.h \
 *         "SimplifiedMaterialLocator.h"
 *
 * Implementation of a IMaterialLocator that uses a simplified geometry
 * finding materials on a trajectory.
 *
 *  @author Wouter Hulsbergen
 *  @date   21/05/2007
 */

class SimplifiedMaterialLocator : public MaterialLocatorBase
{
public: 
  /// Constructor
  SimplifiedMaterialLocator( const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent ) ;
  
  /// intialize
  virtual StatusCode initialize() ;
  virtual StatusCode finalize() ;

  using MaterialLocatorBase::intersect;
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
			    ILVolume::Intersections& intersepts ) const ;
  
private:
  std::string m_tgvolname ;   ///< path to the tracking geometry
  typedef std::vector<MaterialLocatorUtils::PVolumeWrapper*> VolumeContainer ;
  VolumeContainer m_volumes ; ///< wrappers around geo volumes to speed things up
};

#endif // TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H 
