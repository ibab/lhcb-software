#ifndef      __DETDESC_VOLUMES_IPVOLUME_PREDICATES_H__
#define      __DETDESC_VOLUMES_IPVOLUME_PREDICATES_H__

#include <iostream>
#include <functional>


#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"

#include "GaudiKernel/IPVolume.h"
#include "GaudiKernel/ILVolume.h"

//
//
//   define useful predicate to deal with IPVolume and ILVolume 
//

// 
// This predicate allows search from a sequence of IPVolume for a IPVolume, 
// which contains the given point 
//   (container)<IPVolume*> volumes = ...
//   (container)<IPvolume*>::const_ietartor ci = 
//         std::find_if( volumes.begin() , volumes.end() , IPVolume_isInside(Point) );
//
class IPVolume_isInside: public std::unary_function<IPVolume*,bool>
{
  //
 public:
  //
  explicit IPVolume_isInside( const HepPoint3D& PointInMotherFrame ) : m_point( PointInMotherFrame ){};
  //
  inline bool operator() (  IPVolume*&  pv ) { return ( ( 0 == pv ) ? false : pv->isInside( m_point ) ) ; }
  //
 private:
  //
  const HepPoint3D m_point;
  //
};


// 
// This predicate allows search from a sequence of IPVolume for a IPVolume by name 
//   (container)<IPVolume*> volumes = ...
//   (container)<IPvolume*>::const_ietartor ci = 
//         std::find_if( volumes.begin() , volumes.end() , IPVolume_byName(name) );
//
class IPVolume_byName: std::unary_function<IPVolume*,bool>
{
  //
 public:
  //
  explicit IPVolume_byName( const std::string& Name ) : m_name( Name ){};
  //
  inline bool operator() (  const IPVolume*  pv )  { return ( ( 0 == pv ) ? false : (pv->name() == m_name) ) ; }
  //
 private:
  //
  std::string m_name;  
  //
};

//
// 
//   used for std::accumulate algorithm  
//   (container)<IPVolume*> volumes = ...
//  std::accumulate(...)
//
class IPVolume_accumulateMatrix: public std::unary_function<IPVolume*,HepTransform3D&>
{
  //
 public: 
  //
  inline HepTransform3D& operator() ( HepTransform3D&  mtrx , 
				      const  IPVolume* pv   ) 
{ mtrx = pv->matrix()*mtrx; return mtrx; }
  //
}; 


//
//  used for transformation of ILVolume::ReplicaPath into 
//  ILVolume::PVolumePath
//  used for "std::transform" algorithm
//
class IPVolume_fromReplica:  public std::unary_function<ILVolume::ReplicaType,IPVolume*>
{
  //
 public: 
  //
  IPVolume_fromReplica( const ILVolume* LV ): lv ( LV ) {};
  // 
  inline IPVolume* operator() ( const ILVolume::ReplicaType& replica ) 
    {
      if( 0 == lv ) {          return 0 ; }  
      IPVolume* pv = (*lv)[replica]; 
      if( 0 == pv ) { lv = 0 ; return 0 ; } 
      lv = pv->lvolume(); 
      return pv; 
    }
  
 private:
  
  const ILVolume* lv;
  
}; 

//
//
//


#endif  //   __DETDESC_VOLUMES_IPVOLUME_PREDICATES_H__
