// $Id: SolidSubtraction.cpp,v 1.11 2003-09-20 13:25:42 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2003/06/16 13:42:36  sponce
// fixes for gcc 3.2 and 3.3
//
// Revision 1.9  2003/05/16 13:59:14  ibelyaev
//  fix of ancient saga with 'double deletion of solids'
//
// Revision 1.8  2002/05/26 10:47:14  ibelyaev
//  remove checks/tests and update requiements
//
// Revision 1.7  2002/05/11 18:25:48  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
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
                                    ISolid*            First )
  : SolidBoolean ( name , First )
  , SolidBase    ( name         )
{
  if( 0 == First ) 
    { throw SolidException(" SolidSubtraction:: ISolid* points to NULL! "); }
};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the solid subtraction  
 */
// ============================================================================
SolidSubtraction::SolidSubtraction( const std::string& Name)
  : SolidBoolean ( Name )
  , SolidBase    ( Name )
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
bool SolidSubtraction::isInside     ( const HepPoint3D   & point ) const 
{ 
  /// check bounding box 
  if ( isOutBBox( point )          ) { return false ; }
  ///  is point inside the "main" volume?  
  if ( !first()->isInside( point ) ) { return false; }
  /// find the first daughter in which the given point is placed   
  SolidSubtraction::SolidChildrens::const_iterator ci = 
    std::find_if( childBegin() , childEnd() , Solid::IsInside( point ) );
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
                                        const HepTransform3D*  mtrx     )
{  return addChild( solid , mtrx ); };

// ============================================================================
/** subtract child solid from  the solid 
 *  @param solid pointer to child solid 
 *  @param position position  
 *  @return status code 
 */
// ============================================================================
StatusCode  SolidSubtraction::subtract ( ISolid*               solid    , 
                                         const HepPoint3D&     position , 
                                         const HepRotation&    rotation )
{ return addChild( solid , position , rotation ) ; };
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
