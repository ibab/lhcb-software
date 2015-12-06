// $Id: IPVolume_predicates.h,v 1.7 2007-01-17 12:10:12 cattanem Exp $ 
// ============================================================================
#ifndef     DETDESC_IPVOLUME_PREDICATES_H
#define     DETDESC_IPVOLUME_PREDICATES_H
// STD & STL 
#include <iostream>
#include <functional>
// Geometry definitions 
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"
// DetDesc 
#include "DetDesc/IPVolume.h"
#include "DetDesc/ILVolume.h"

/** @file IPVolume_predicates.h
 *  define useful predicate to deal with IPVolume and ILVolume 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

/** @class IPVolume_isInside IPVolume_predicates.h 
 *  
 * This predicate allows search from a sequence of IPVolume for a IPVolume, 
 * which contains the given point 
 * (container)<IPVolume*> volumes = ...
 * (container)<IPvolume*>::const_ietartor ci = 
 * std::find_if( volumes.begin() , volumes.end() , 
 *  IPVolume_isInside(Point) );
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 */
class IPVolume_isInside
{
public:
  /** explict constructor
   *  @param PointInMotherFrame point in mother reference system 
   */
  explicit IPVolume_isInside( const Gaudi::XYZPoint& PointInMotherFrame ) 
    : m_point( PointInMotherFrame ){};
  /** check for the point
   *  @param pv pointer to Physical volume 
   *  @return true if point is inside the physical volume 
   */
  inline bool operator() (  const IPVolume*  pv ) const 
  { return ( pv && pv->isInside( m_point ) ) ; }
 private:
  /// point to be checked 
  const Gaudi::XYZPoint m_point;
};


/** @class  IPVolume_byName IPVolume_predicates.h
 *  This predicate allows search from a sequence of 
 *  IPVolume for a IPVolume by name 
 *    (container)<IPVolume*> volumes = ...
 *  (container)<IPvolume*>::const_ietartor ci = 
 * std::find_if( volumes.begin() , volumes.end() , 
 *  IPVolume_byName(name) );
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
class IPVolume_byName
{
 public:
  /** explicit constructor 
   *  @param Name name of physical volume 
   */
  explicit IPVolume_byName( const std::string& Name ) 
    : m_name( Name ){};
  /** find physical volume by name
   *  @param pv pointer to physical volume 
   *  @return true if name matches the given name 
   */
  inline bool operator() (  const IPVolume*  pv )  const 
  { return pv && ( pv->name() == m_name); }
private:
  /// name 
  std::string m_name;  
};

/** @class IPVolume_accumulateMatrix IPVolume_predicates.h
 * 
 * used for std::accumulate algorithm  
 * (container)<IPVolume*> volumes = ...
 * std::accumulate(...)
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
class IPVolume_accumulateMatrix
{
public: 
  //
  inline Gaudi::Transform3D& operator() 
    ( Gaudi::Transform3D&  mtrx , const  IPVolume* pv   ) 
  { mtrx = pv->matrix()*mtrx; return mtrx; }
  //
}; 


/** @class IPVolume_fromReplica IPVolume_predicates.h 
 * 
 * used for transformation of ILVolume::ReplicaPath into 
 * ILVolume::PVolumePath
 * used for "std::transform" algorithm
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 */
class IPVolume_fromReplica
{
public: 
  /** constructor 
   *  @param LV pointer to Logical Volume 
   */
  IPVolume_fromReplica( const ILVolume* LV ) 
    : m_lv ( LV ) {};
  
  /** get pointer to physical volume by replica number 
   *  @param replica replica number 
   *  @return pointer to physical volume 
   */
  inline const IPVolume* operator() 
    ( const ILVolume::ReplicaType& replica ) const  
  {
    if( !m_lv ) {            return nullptr ; }  
    const IPVolume* pv = (*m_lv)[replica]; 
    if( !pv   ) { m_lv = nullptr ; return nullptr ; } 
    m_lv = pv->lvolume(); 
    return pv; 
  }
private:
  
  mutable const ILVolume* m_lv;
  
}; 

// ============================================================================
#endif  ///<  DETDESC_IPVOLUME_PREDICATES_H
// ============================================================================
