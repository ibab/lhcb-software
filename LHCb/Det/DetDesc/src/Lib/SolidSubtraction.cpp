// $Id: SolidSubtraction.cpp,v 1.14 2005-12-08 19:20:02 jpalac Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.13  2005/12/05 16:18:43  jpalac
//
// ! 2005-12-05 - Juan Palacios
//  - Add class Gaudi::Plane3D as stop-gap while MathCore equivalent is implemented.
//    Supports only double precision cartesian representation.
//  - Fix remaining MathCore-related compilation problems.
//
// Revision 1.12  2005/12/02 18:36:56  jpalac
//
// ! 2005-02-12 - Juan Palacios
//  - Add Kernel/LHCbDefinitions to requirements
//  - Change CLHEP geometry classes to LHCbDefinitions typedefs:
//             * These typedefs point to MathCore classes with the
//               exception of Gaudi::Plane3D, which pointe to HepPoint3D.
//               Awaiting implementation of MathCore Plane3D class.
//  - Make changes to all code to adapt to MathCore syntax
//  - TO-DO: Not compiling due to Plane3D operaitons with MathCore classes
//
// Revision 1.11  2003/09/20 13:25:42  ibelyaev
//  few fixes to solve Gauss problems
//
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
 *  @param solid pointer to child solid 
 *  @param position position  
 *  @return status code 
 */
// ============================================================================
StatusCode  SolidSubtraction::subtract ( ISolid*                  solid    , 
                                         const Gaudi::XYZPoint&   position , 
                                         const Gaudi::Rotation3D& rotation )
{ return addChild( solid , position , rotation ) ; };
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
