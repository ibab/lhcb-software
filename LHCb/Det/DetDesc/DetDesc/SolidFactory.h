// $Id: SolidFactory.h,v 1.5 2004-07-09 14:19:41 cattanem Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
#ifndef DETDESC_SOLIDFACTORY_H 
#define DETDESC_SOLIDFACTORY_H 1
/// include files 
/// GaudiKernel
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
/// DetDesc 
#include "DetDesc/ISolidFactory.h"

/** @class SolidFactory SolidFactory.h DetDesc/SolidFactory.h
 *  
 *  Templated class to instrantiate concreate Solids 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/08/2001
 */

template <class ConcreteSolid> 
class SolidFactory : virtual public ISolidFactory 
{
public:
  
  /** standard (default)  constructor
   */
  SolidFactory() 
  {
    // Get the class name using the RTTI.
    m_ident     = System::typeinfoName( typeid( ConcreteSolid ) );
    FactoryTable::instance()->addFactory( this );
  };
  
  /** virtual destructor
   */
  virtual ~SolidFactory(){};

  /** implementation of IInterface::addRef() 
   */  
  virtual unsigned long      addRef           ()       { return        1 ; }; 
  /** implementation of IInterface::release() 
   */  
  virtual unsigned long      release          ()       { return        1 ; }; 

  /** implementation of IFactory::addRef() 
   */  
  virtual unsigned long      addRef           () const { return        1 ; }; 

  /** implementation of IFactory::release() 
   */  
  virtual unsigned long      release          () const { return        1 ; }; 
  
  /** retrieve the identification type 
   *  @return identification/name 
   */
  virtual const std::string& ident            () const { return m_ident  ; };
  
  /** create the instance of ISolid object
   *  @return pointer to new ISolid object
   */
  virtual ISolid*    instantiate() const 
  { return new ConcreteSolid(); }
  
  /** create the instance of ISolid object
   *  @return pointer to new ISolid object
   */
  virtual IInterface* instantiate( IInterface* /* parent */ ) const 
  { return new ConcreteSolid(); }

  /** query the interface 
   *  @param riid interface ID 
   *  @param ppI  placeholder for returned interface 
   *  @return status code 
   */ 
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
  {
    if( 0 == ppI ) { return StatusCode::FAILURE; } 
    ppI = 0 ; 
    if      ( IID_ISolidFactory == riid ) 
      { *ppI = static_cast<ISolidFactory*> (this); }
    else if ( IID_IFactory                == riid )
      { *ppI = static_cast<IFactory*>       (this); }
    else if ( IID_IInterface              == riid )
      { *ppI = static_cast<IInterface*>     (this); }
    else                { return   StatusCode::FAILURE; } /// RETURN!!!
    ///
    addRef();
    ///
    return StatusCode::SUCCESS;
  }
  ///
private:
  ///
  std::string m_ident;
  //
};

/** @def IMPLEMENT_SOLID
 *  Useful MACRO to implement static Solid Factory
 *  @see ISolidFactory
 *  @see DECLARE_SOLID
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 4 May 2002 
 */
#define IMPLEMENT_SOLID(x) \
 static const SolidFactory<x>         s_##x##Factory ; \
 const       ISolidFactory& x##Factory = s_##x##Factory ;

// ===========================================================================
// The END 
// ===========================================================================
#endif ///< DETDESC_SOLIDFACTORY_H
// ===========================================================================
