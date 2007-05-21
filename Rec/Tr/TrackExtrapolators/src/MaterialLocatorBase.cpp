#include "MaterialLocatorBase.h"

MaterialLocatorBase::MaterialLocatorBase(const std::string& type,
					 const std::string& name,
					 const IInterface* parent ) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IMaterialLocator>(this);
}


StatusCode MaterialLocatorBase::initialize()
{
  return StatusCode::SUCCESS ;
}

size_t MaterialLocatorBase::intersect( const Gaudi::XYZPoint& p, const Gaudi::XYZVector& v, 
				       IMaterialLocator::Intersections& intersepts ) const 
  
{
  static ILVolume::Intersections origintersepts ;
  origintersepts.clear() ;
  size_t rc = intersect(p,v,origintersepts) ;
  double dz = v.z() ;
  double z1 = p.z() ; 
  intersepts.resize(origintersepts.size()) ;
  int i(0) ;
  for( ILVolume::Intersections::const_iterator it = origintersepts.begin() ;
       it != origintersepts.end() ; ++it, ++i ) {
    intersepts[i].z1 = z1 + dz * it->first.first ;
    intersepts[i].z2 = z1 + dz * it->first.second ;
    intersepts[i].material  = it->second ;
  }
  return rc ;
}
