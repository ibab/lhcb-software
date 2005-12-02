// $Id: IPVolume_predicates.h,v 1.6 2005-12-02 18:36:55 jpalac Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/11/18 15:32:44  ibelyaev
//  update for Logical Assemblies
//
// Revision 1.4  2001/08/24 12:06:13  ibelyaev
//  changes to take into account Assembly Volumes
// 
// ============================================================================
#ifndef     DETDESC_IPVOLUME_PREDICATES_H
#define     DETDESC_IPVOLUME_PREDICATES_H
// STD & STL 
#include <iostream>
#include <functional>
// CLHEP 
#include "Kernel/Point3DTypes.h"
#include "Kernel/Transform3DTypes.h"
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
class IPVolume_isInside: 
  public std::unary_function<const IPVolume*,bool>
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
  { return ( ( 0 == pv ) ? false : pv->isInside( m_point ) ) ; }
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
class IPVolume_byName: 
  std::unary_function<const IPVolume*,bool>
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
  { return ( ( 0 == pv ) ? false : ( pv->name() == m_name) ) ; }
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
class IPVolume_accumulateMatrix: 
  public std::unary_function<const IPVolume*,Gaudi::Transform3D&>
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
class IPVolume_fromReplica:  
  public std::unary_function<ILVolume::ReplicaType,const IPVolume*>
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
    if( 0 == m_lv ) {            return 0 ; }  
    const IPVolume* pv = (*m_lv)[replica]; 
    if( 0 == pv   ) { m_lv = 0 ; return 0 ; } 
    m_lv = pv->lvolume(); 
    return pv; 
  }
private:
  
  mutable const ILVolume* m_lv;
  
}; 

// ============================================================================
#endif  ///<  DETDESC_IPVOLUME_PREDICATES_H
// ============================================================================
