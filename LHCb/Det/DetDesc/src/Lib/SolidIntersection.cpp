// $Id: SolidIntersection.cpp,v 1.13 2006-06-02 06:59:02 cattanem Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#include <iostream> 
#include <string> 
#include "DetDesc/SolidIntersection.h"
#include "DetDesc/SolidException.h"
#include "DetDesc/Solid.h"

// ============================================================================
/** @file
 *
 *  implementation of class SolidIntersection
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 * @date xx/xx/xxx
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the intersection
 *  @param first pointer to first/main solid 
 */
// ============================================================================
SolidIntersection::SolidIntersection( const std::string& name  , 
                                      ISolid*            first )
  : SolidBase    ( name         )
  , SolidBoolean ( name , first )
{};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the intersection
 */
// ============================================================================
SolidIntersection::SolidIntersection( const std::string& name )
  : SolidBase    ( name )
  , SolidBoolean ( name )
{};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
SolidIntersection::~SolidIntersection(){}

// ============================================================================
bool SolidIntersection::isInside( const Gaudi::XYZPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidIntersection::isInside( const Gaudi::Polar3DPoint& point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
bool SolidIntersection::isInside( const Gaudi::RhoZPhiPoint   & point ) const 
{
  return isInsideImpl(point);
}
// ============================================================================
template <class aPoint>
bool SolidIntersection::isInsideImpl( const aPoint   & point ) const 
{ 
  ///  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; }
  /// find the first daughter in which the given point is NOT placed   
  SolidBoolean::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , 
                  std::not1(Solid::IsInside<aPoint>( point ) ) );
  /// 
  return ( childEnd() == ci ? true : false );   
};

// ============================================================================
/** add intersections 
 *  @param solid pointer         to new solid 
 *  @param mtrx  pointer transformation
 *  @return status code
 */
// ============================================================================
StatusCode  SolidIntersection::intersect( ISolid*               solid     , 
                                          const Gaudi::Transform3D* mtrx      )
{  return addChild( solid , mtrx ); };

// ============================================================================
/** add intersections 
 *  @param solid pointer         to new solid 
 *  @param position position 
 *  @param rotation rotation 
 *  @return status code
 */
// ============================================================================
StatusCode  SolidIntersection::intersect 
( ISolid*               solid    , 
  const Gaudi::XYZPoint&     position , 
  const Gaudi::Rotation3D&    rotation )
{ return addChild( solid , position , rotation ) ; }; 

// ============================================================================















