// $Id: GiGaEventActionFactory.h,v 1.6 2002-04-25 13:02:04 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/08/12 15:42:43  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.4  2001/07/23 13:11:42  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GiGaEventActionFACTORY_H
#define    GIGA_GiGaEventActionFACTORY_H 1 
// ============================================================================
// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcLocator.h"
// local 
#include "GiGa/IGiGaEventActionFactory.h"

/** @class GiGaEventActionFactory GiGaEventActionFactory.h 
 *
 *  implementation of factory to create "Event Action" factory class
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   17/03/2001 
 */

template <class ConcreteEA>
class GiGaEventActionFactory: public IGiGaEventActionFactory
{
  ///
public:
  /// Default constructor
  GiGaEventActionFactory() 
  {
    // Get the class name using the RTTI.
    m_eaType     = System::typeinfoName( typeid( ConcreteEA ) );
    m_ident      = m_eaType;
    FactoryTable::instance()->addFactory( this );
  };
  /// Default destructor
  virtual ~GiGaEventActionFactory(){};
  /// from IInterface  
  virtual unsigned long      addRef           ()       { return        1 ; }; 
  virtual unsigned long      release          ()       { return        1 ; }; 
  /// from IFactory 
  virtual unsigned long      addRef           () const { return        1 ; }; 
  virtual unsigned long      release          () const { return        1 ; }; 
  /// GiGaEventAction type name
  virtual const std::string& eventActionType  () const { return m_eaType ; };  
  virtual const std::string& ident            () const { return m_ident  ; };
  /// create the instance 
  virtual IGiGaEventAction*    instantiate( const std::string& name, 
                                            ISvcLocator *svcloc) const 
  { return new ConcreteEA(name, svcloc); }
  /// a little bit funny 
  virtual IInterface* instantiate( IInterface* parent ) const 
  {
    ISvcLocator* loc = 
      0 == parent                              ? Gaudi::svcLocator() : 
      0 == dynamic_cast<ISvcLocator*> (parent) ? Gaudi::svcLocator() : 
      dynamic_cast<ISvcLocator*>(parent);
    return new ConcreteEA( eventActionType() , loc ); 
  }
  ///
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
  {
    if( 0 == ppI ) { return StatusCode::FAILURE; } 
    ppI = 0 ; 
    if      ( IID_IGiGaEventActionFactory == riid ) 
      { *ppI = static_cast<IGiGaEventActionFactory*> (this); }
    else if ( IID_IFactory                == riid )
      { *ppI = static_cast<IFactory*>       (this); }
    else if ( IID_IInterface              == riid )
      { *ppI = static_cast<IInterface*>     (this); }
    else                { return   StatusCode::FAILURE; } /// RETURN!!!
    addRef();
    return StatusCode::SUCCESS;
  }
  ///
private:
  ///
  std::string m_eaType;
  std::string m_ident;
  //
};

/** @def IMPLEMENT_GiGaEventAction
 *  useful macro to implement teh concrete factory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 25 Apr 2002 
 */
#define IMPLEMENT_GiGaEventAction( x ) \
 static const     GiGaEventActionFactory<##x##>         s_##x##Factory ; \
 const           IGiGaEventActionFactory&##x##Factory = s_##x##Factory ;

// ============================================================================
// The END 
// ============================================================================
#endif  // GIGA_GiGaEventActionFACTORY_H
// ============================================================================













