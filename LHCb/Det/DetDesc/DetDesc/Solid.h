// $Id: Solid.h,v 1.2 2003-04-25 08:52:23 sponce Exp $
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/08/09 16:47:57  ibelyaev
/// update in interfaces and redesign of solids
/// 
/// ===========================================================================
#ifndef DETDESC_SOLID_H 
#define DETDESC_SOLID_H 1
/// STD & STL 
#include <string>
#include <functional>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
/// DetDesc 
#include "DetDesc/ISolid.h"

/** @namespace Solid Solid.h "DetDesc/Solid.h"
 *  
 *  A number of useful methods to deal with solids 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   03/08/2001
 */

namespace Solid 
{
  
  /** create (using abstract factory technique) the 
   *  concrete solid of given type 
   *  
   *  @see ISolid
   *  @see ISolidFactory
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date 03/08/2001 
   *
   *  @exception SolidException  wrong solid type or environment 
   *  @param solidType           type of solid 
   *  @return pointer to new created ISolid object 
   */
  ISolid* createSolid( const std::string& solidType );
  
  /** @class IsInside Solid.h "DetDesc/Solid.h"
   *  
   *  useful class to check if point is inside of 
   *  the given solid 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 03/08/2001 
   */
  class IsInside: 
    public std::unary_function<const ISolid*,bool>
  {
  public:
    
    /** (explicit) constructor
     *   @param point  point (in local reference system of solid)
     */
    explicit IsInside( const HepPoint3D& LocalPoint  ) 
      : m_point( LocalPoint ){};
    
    /** check if the point is inside of solid 
     *  @param solid pointer to ISolid object
     *  @return true, if point is inside the solid 
     */
    inline bool operator() ( const ISolid* solid ) const  
    { return ( 0 == solid ? false : solid->isInside( m_point ) ); };
    
  private:
    
    HepPoint3D m_point; ///< point in the Local Reference System of Solid 
    
  };

  
}; ///< end of namespace Solid 

/// ===========================================================================
#endif ///< DETDESC_SOLID_H
/// ===========================================================================
