// $Id:
// ==========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ==========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/08/09 16:47:57  ibelyaev
// update in interfaces and redesign of solids
// 
// ==========================================================================
#ifndef DETDESC_ISOLIDFACTORY_H 
#define DETDESC_ISOLIDFACTORY_H 1
// GaudiKernel
#include "GaudiKernel/IFactory.h"  
// forward declarations 
class ISolid;   // DetDesc

/** @var IID_ISolidFactory
 *  Declaration of the interface ID ( interface id, major & minor versions) 
 */
static const InterfaceID IID_ISolidFactory( "ISolidFactory" , 2 , 0 ); 

/** @class ISolidFactory ISolidFactory.h DetDesc/ISolidFactory.h
 *   
 *  An abstract interface to abstract factory to instantiate
 *  solids 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/08/2001
 */

class ISolidFactory : public virtual IFactory
{
  
public:
  
  /** retrieve the uninque interface ID 
   *  @return uniqie interface ID 
   */
  static const InterfaceID& interfaceID() { return IID_ISolidFactory; }
  
  /** create an instance of a concrete ISolid Object
   *  @return pointer to created ISolid Object 
   */
  virtual ISolid* instantiate () const = 0;
  
protected:
  
  /// derstructor 
  virtual ~ISolidFactory(){};
  
};

/** @def DECLARE_SOLID 
 *  useful macro for declaration of Solid Factories 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   6 May 2002
 */
#define DECLARE_SOLID(x) extern const ISolidFactory& x##Factory; \
                                                     x##Factory.addRef();


// ==========================================================================
#endif ///< DETDESC_ISOLIDFACTORY_H
// ==========================================================================
