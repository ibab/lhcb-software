/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/08/09 16:48:02  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
#include <iostream> 
#include <string> 
#include "DetDesc/SolidIntersection.h"
#include "DetDesc/SolidException.h"
#include "DetDesc/Solid.h"

// ============================================================================
/** @file SolidIntersection.cpp 
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
  : SolidBoolean( name , first )
{};

// ============================================================================
/** constructor 
 *  @param name name of the intersection
 */
// ============================================================================
SolidIntersection::SolidIntersection( const std::string& name )
  : SolidBoolean( name )
{};

// ============================================================================
/// destructor 
// ============================================================================
SolidIntersection::~SolidIntersection(){ reset(); }

// ============================================================================
// ============================================================================
bool SolidIntersection::isInside( const HepPoint3D   & point ) const 
{ 
  ///  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; }
  /// find the first daughter in which the given point is NOT placed   
  SolidBoolean::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , 
                  std::not1(Solid::IsInside( point ) ) );
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
                                          const HepTransform3D* mtrx      )
{  return addChild( solid , mtrx ); };

// ============================================================================
/** add intersections 
 *  @param solid pointer         to new solid 
 *  @param position position 
 *  @param rotation rotation 
 *  @return status code
 */
// ============================================================================
StatusCode  SolidIntersection::intersect ( ISolid*               solid    , 
                                           const HepPoint3D&     position , 
                                           const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; }; 

// ============================================================================















