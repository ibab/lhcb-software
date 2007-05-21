#ifndef TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H 
#define TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IMaterialLocator.h"


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
  
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint&, const Gaudi::XYZVector&, 
			    ILVolume::Intersections&) const = 0 ;
  
  /// Intersect a line with volumes in the geometry
  virtual size_t intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
			    Intersections& intersepts) const ;
private:
};

#endif // TRACKEXTRAPOLATORS_TRACKMATERIALINTERSECTORBASE_H 
