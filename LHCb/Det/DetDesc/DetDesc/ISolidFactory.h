/// ==========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ==========================================================================
/// $Log: not supported by cvs2svn $ 
/// ==========================================================================
#ifndef DETDESC_ISOLIDFACTORY_H 
#define DETDESC_ISOLIDFACTORY_H 1
/// GaudiKernel
#include "GaudiKernel/IFactory.h"  
/// forward declarations 
class ISolid;   ///< DetDesc 

/// Declaration of the interface ID ( interface id, major & minor versions) 
static const InterfaceID IID_ISolidFactory( 151 , 1 , 0 ); 

/** @class ISolidFactory ISolidFactory.h DetDesc/ISolidFactory.h
 *   
 *  An abstract interface to abstract factory to instantiate
 *  solids 
 *  
 *  @author Ivan Belyaev
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

/// ==========================================================================
#endif ///< DETDESC_ISOLIDFACTORY_H
/// ==========================================================================
