// $Id: SolidSubtraction.cpp,v 1.16 2009-04-17 08:54:24 cattanem Exp $
// ===========================================================================
/** STD & STL */
#include <iostream> 
#include <string>
/** DetDesc */
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidException.h"
#include "DetDesc/Solid.h"

// ============================================================================
/** @file 
 *
 *  implementation of class SolidSubtraction 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the intersection
 *  @param first pointer to first/main solid 
 */
// ============================================================================
SolidSubtraction::SolidSubtraction( const std::string& name  , 
                                    ISolid*            first )
  : SolidBase    ( name         )
  , SolidBoolean ( name , first )
{
  if( 0 == first ) 
    { throw SolidException(" SolidSubtraction:: ISolid* points to NULL! "); }
};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the solid subtraction  
 */
// ============================================================================
SolidSubtraction::SolidSubtraction( const std::string& Name)
  : SolidBase    ( Name )
  , SolidBoolean ( Name )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
SolidSubtraction::~SolidSubtraction(){}

// ============================================================================
/** - check for the given 3D-point. 
 *    Point coordinates are in the local reference 
 *    frame of the solid.   
 *  - implementation of ISolid absstract interface  
 *  @see ISolid 
 *  @param point point (in local reference system of the solid)
 *  @return true if the point is inside the solid
 */
// ============================================================================
bool SolidSubtraction::isInside( const Gaudi::XYZPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidSubtraction::isInside( const Gaudi::Polar3DPoint& point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidSubtraction::isInside( const Gaudi::RhoZPhiPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
template <class aPoint>
bool SolidSubtraction::isInsideImpl( const aPoint& point ) const 
{ 
  /// check bounding box 
  if ( isOutBBox( point )          ) { return false ; }
  ///  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; }
  /// find the first daughter in which the given point is placed   
  SolidSubtraction::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , 
                  childEnd() , 
                  Solid::IsInside<aPoint>( point ) );
  ///
  return ( childEnd() == ci ? true : false );   
};

// ============================================================================
/** subtract child solid to the solid
 *  @param solid pointer to child solid 
 *  @param mtrx  pointer to transformation 
 *  @return status code 
 */
// ============================================================================
StatusCode  SolidSubtraction::subtract( ISolid*                solid    , 
                                        const Gaudi::Transform3D*  mtrx     )
{  return addChild( solid , mtrx ); };

// ============================================================================
/** subtract child solid from  the solid 
 *  @param child    pointer to child solid 
 *  @param position position  
 *  @param rotation rotation  
 *  @return status code 
 */
// ============================================================================
StatusCode  SolidSubtraction::subtract ( ISolid*                  child    , 
                                         const Gaudi::XYZPoint&   position , 
                                         const Gaudi::Rotation3D& rotation )
{ return addChild( child , position , rotation ) ; };
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
