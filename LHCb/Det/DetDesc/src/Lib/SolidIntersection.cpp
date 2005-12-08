// $Id: SolidIntersection.cpp,v 1.12 2005-12-08 19:20:02 jpalac Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.11  2005/12/05 16:18:43  jpalac
//
// ! 2005-12-05 - Juan Palacios
//  - Add class Gaudi::Plane3D as stop-gap while MathCore equivalent is implemented.
//    Supports only double precision cartesian representation.
//  - Fix remaining MathCore-related compilation problems.
//
// Revision 1.10  2005/12/02 18:36:56  jpalac
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
// Revision 1.9  2003/09/20 13:25:42  ibelyaev
//  few fixes to solve Gauss problems
//
// Revision 1.8  2003/06/16 13:42:36  sponce
// fixes for gcc 3.2 and 3.3
//
// Revision 1.7  2003/05/16 13:59:14  ibelyaev
//  fix of ancient saga with 'double deletion of solids'
//
// Revision 1.6  2002/05/11 18:25:47  ibelyaev
//  see $DETDESCROOT/doc/release.notes 11 May 2002
//
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
  : SolidBoolean ( name , first )
  , SolidBase    ( name         )
{};
// ============================================================================

// ============================================================================
/** constructor 
 *  @param name name of the intersection
 */
// ============================================================================
SolidIntersection::SolidIntersection( const std::string& name )
  : SolidBoolean ( name )
  , SolidBase    ( name )
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















