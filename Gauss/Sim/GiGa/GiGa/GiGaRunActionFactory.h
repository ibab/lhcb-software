// $Id: GiGaRunActionFactory.h,v 1.4 2002-04-25 13:02:04 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/08/12 15:42:43  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.2  2001/07/27 14:28:58  ibelyaev
// bug fix
//
// Revision 1.1  2001/07/26 16:09:20  ibelyaev
// GiGaRunActions budiness
//
// ============================================================================
#ifndef    GIGA_GiGaRunActionFACTORY_H
#define    GIGA_GiGaRunActionFACTORY_H 1 
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcLocator.h"
/// GiGa
#include "GiGa/IGiGaRunActionFactory.h"

/** @class GiGaRunActionFactory GiGaRunActionFactory.h 
 *
 *  implementation of factory to create "Run Action" factory class
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   17/03/2001 
 */

template <class ConcreteRA>
class GiGaRunActionFactory: public IGiGaRunActionFactory
{
  ///
public:
  /// Default constructor
  GiGaRunActionFactory() 
  {
    // Get the class name using the RTTI.
    m_raType     = System::typeinfoName( typeid( ConcreteRA ) );
    m_ident      = m_raType;
    FactoryTable::instance()->addFactory( this );
  };
  /// Default destructor
  virtual ~GiGaRunActionFactory(){};
  /// from IInterface  
  virtual unsigned long      addRef           ()       { return        1 ; }; 
  virtual unsigned long      release          ()       { return        1 ; }; 
  /// from IFactory 
  virtual unsigned long      addRef           () const { return        1 ; }; 
  virtual unsigned long      release          () const { return        1 ; }; 
  /// GiGaEventAction type name
  virtual const std::string& runActionType    () const { return m_raType ; };  
  virtual const std::string& ident            () const { return m_ident  ; };
  /// create the instance 
  virtual IGiGaRunAction*    instantiate( const std::string& name, 
                                          ISvcLocator *svcloc) const 
  { return new ConcreteRA( name, svcloc); }
  /// a little bit funny 
  virtual IInterface* instantiate( IInterface* parent ) const 
  {
    ISvcLocator* loc = 
      0 == parent                              ? Gaudi::svcLocator() : 
      0 == dynamic_cast<ISvcLocator*> (parent) ? Gaudi::svcLocator() : 
      dynamic_cast<ISvcLocator*>(parent);
    return new ConcreteRA( runActionType() , loc ); 
  }
  ///
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
  {
    if( 0 == ppI ) { return StatusCode::FAILURE; } 
    ppI = 0 ; 
    if      ( IID_IGiGaRunActionFactory == riid ) 
      { *ppI = static_cast<IGiGaRunActionFactory*> (this); }
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
  std::string m_raType;
  std::string m_ident;
  //
};

/** @def IMPLEMENT_GiGaRunAction
 *  useful macro to implement the concrete factory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 25 Apr 2002 
 */
#define IMPLEMENT_GiGaRunAction( x ) \
 static const     GiGaRunActionFactory<##x##>         s_##x##Factory ; \
 const           IGiGaRunActionFactory&##x##Factory = s_##x##Factory ;

// ============================================================================
// The END 
// ============================================================================
#endif  // GIGA_GiGaRunActionFACTORY_H
// ============================================================================













