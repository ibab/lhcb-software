// $Id: Solid.h,v 1.7 2007-02-26 12:07:11 cattanem Exp $
/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef DETDESC_SOLID_H 
#define DETDESC_SOLID_H 1
/// STD & STL 
#include <string>
#include <functional>
/// Geometry 
#include "GaudiKernel/Point3DTypes.h"
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
  /** @class IsInside Solid.h "DetDesc/Solid.h"
   *  
   *  useful class to check if point is inside of 
   *  the given solid 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 03/08/2001 
   */
  template<class aPoint>
  class IsInside: 
    public std::unary_function<const ISolid*,bool>
  {
  public:
    
    /** (explicit) constructor
     *   @param LocalPoint  point (in local reference system of solid)
     */
    explicit IsInside( const aPoint& LocalPoint  ) 
      : m_point( LocalPoint ){};
    
    /** check if the point is inside of solid 
     *  @param solid pointer to ISolid object
     *  @return true, if point is inside the solid 
     */
    inline bool operator() ( const ISolid* solid ) const  
    { return ( 0 == solid ? false : solid->isInside( m_point ) ); };
    
  private:
    
    aPoint m_point; ///< point in the Local Reference System of Solid 
    
  };

  
}; ///< end of namespace Solid 

/// ===========================================================================
#endif ///< DETDESC_SOLID_H
/// ===========================================================================
