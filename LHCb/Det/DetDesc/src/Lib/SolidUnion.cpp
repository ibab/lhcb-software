/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/08/09 16:48:03  ibelyaev
/// update in interfaces and redesign of solids
///
/// ===========================================================================
///@{
/** STD & STL  */
#include <iostream> 
#include <string>
///@} 
///@{
/** DetDesc */ 
#include "DetDesc/SolidUnion.h"
#include "DetDesc/SolidException.h"
#include "DetDesc/SolidFactory.h"
#include "DetDesc/Solid.h"
///@} 

// ============================================================================
/** @file SolidUnion.cpp
 *
 *  implementation of class SolidUnion
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   xx/xx/xxxx
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the intersection
 *  @param first pointer to first/main solid 
 */
// ============================================================================
SolidUnion::SolidUnion( const std::string& name  , 
                        ISolid*            First )
  : SolidBoolean( name , First )
{
  if( 0 == First ) 
    { throw SolidException(" SolidUnion:: ISolid* points to NULL!"); }
};

// ============================================================================
/** constructor 
 *  @param name name of the solid union 
 */
// ============================================================================
SolidUnion::SolidUnion( const std::string& Name )
  : SolidBoolean( Name )
{};

// ============================================================================
/// destructor 
// ============================================================================
SolidUnion::~SolidUnion(){ reset(); }

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
bool SolidUnion::isInside     ( const HepPoint3D   & point ) const 
{ 
  ///  is point inside the "main" volume?  
  if ( first()->isInside( point ) ) { return true ; }
  /// find the first daughter in which the given point is placed   
  SolidUnion::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin () , 
                  childEnd   () , 
                  Solid::IsInside( point ) ) ;
  ///
  return ( childEnd() == ci ? false : true );   
};

// ============================================================================
/** add child solid to the solid union
 *  @param solid pointer to child solid 
 *  @param mtrx  pointer to transformation 
 *  @return status code 
 */
// ============================================================================
StatusCode  SolidUnion::unite( ISolid*                solid    , 
                               const HepTransform3D*  mtrx     )
{  return addChild( solid , mtrx ); };

// ============================================================================
/** add child solid to the solid union
 *  @param solid pointer to child solid 
 *  @param position position  
 *  @return status code 
 */
// ============================================================================
StatusCode  SolidUnion::unite ( ISolid*               solid    , 
                                const HepPoint3D&     position , 
                                const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; };

// ============================================================================

