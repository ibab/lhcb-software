#ifndef TRACKEXTRAPOLATORS_DETAILEDMATERIALLOCATOR_H
#define TRACKEXTRAPOLATORS_DETAILEDMATERIALLOCATOR_H

// Include files
// -------------
#include "GaudiKernel/ServiceHandle.h"
#include "DetDesc/ITransportSvc.h"
#include "MaterialLocatorBase.h"

class IGeometryInfo ;

/** @class DetailedMaterialLocator DetailedMaterialLocator.h \
 *         "DetailedMaterialLocator.h"
 *
 * Implementation of a IMaterialLocator that uses the TransportSvc for
 * finding materials on a trajectory.
 *
 *  @author Wouter Hulsbergen
 *  @date   21/05/2007
 */

class DetailedMaterialLocator : public MaterialLocatorBase
{
public:
  /// Constructor
  DetailedMaterialLocator( const std::string& type,
			   const std::string& name,
			   const IInterface* parent ) ;
  /// intialize
  StatusCode initialize() override;

  using MaterialLocatorBase::intersect;
  /// Intersect a line with volumes in the geometry
  size_t intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v,
                    ILVolume::Intersections& intersepts ) const override;

private:
  double m_minRadThickness ;                     ///< minimum radiation thickness
  std::string m_geometrypath ;                   ///< name of the geometry
  ServiceHandle<ITransportSvc> m_transportSvc ;  ///< handle to transport service
  mutable IGeometryInfo* m_geometry ;
};

#endif // TRACKEXTRAPOLATORS_DETAILEDMATERIALLOCATOR_H
