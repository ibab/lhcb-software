// $Id: GiGaStackActionFactory.h,v 1.8 2002-04-25 13:02:04 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2001/08/12 15:42:44  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.6  2001/07/23 13:11:43  ibelyaev
// the package restructurisation(II)
// 
// ============================================================================
#ifndef    GIGA_GiGaStackActionFACTORY_H
#define    GIGA_GiGaStackActionFACTORY_H 1 
// ============================================================================
// GauidiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcLocator.h"
// GiGa 
#include "GiGa/IGiGaStackActionFactory.h" 


/** @class GiGaStackActionFactory GiGaStackActionFactory.h 
 * 
 *  implementation of factory to create "Stacking Action" factory class
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */


template <class ConcreteSA>
class GiGaStackActionFactory: public IGiGaStackActionFactory
{
  ///
public:
  /// Default constructor
  GiGaStackActionFactory() 
  {
    // Get the class name using the RTTI.
    m_sdType     = System::typeinfoName( typeid( ConcreteSA ) );
    m_ident      = m_sdType;
    FactoryTable::instance()->addFactory( this );
  };
  /// Default destructor
  virtual ~GiGaStackActionFactory(){};
  /// from IInterface  
  virtual unsigned long      addRef           ()       { return        1 ; }; 
  virtual unsigned long      release          ()       { return        1 ; }; 
  /// from IFactory 
  virtual unsigned long      addRef           () const { return        1 ; }; 
  virtual unsigned long      release          () const { return        1 ; }; 
  /// GiGaStackAction type name
  virtual const std::string& stackActionType  () const { return m_sdType ; };  
  virtual const std::string& ident            () const { return m_ident  ; };
  /// create the instance 
  virtual IGiGaStackAction*    instantiate( const std::string& name, 
                                            ISvcLocator *svcloc) const 
  { return new ConcreteSA(name, svcloc); }
  /// a little bit funny 
  virtual IInterface* instantiate( IInterface* parent ) const 
  {
    ISvcLocator* loc = 
      0 == parent                              ? Gaudi::svcLocator() : 
      0 == dynamic_cast<ISvcLocator*> (parent) ? Gaudi::svcLocator() : 
      dynamic_cast<ISvcLocator*>(parent);
    return new ConcreteSA( stackActionType() , loc ); 
  }
  ///
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
  {
    if( 0 == ppI ) { return StatusCode::FAILURE; } 
    ppI = 0 ; 
    if      ( IID_IGiGaStackActionFactory == riid ) 
      { *ppI = static_cast<IGiGaStackActionFactory*> (this); }
    else if ( IID_IFactory                == riid ) 
      { *ppI = static_cast<IFactory*>       (this); }
    else if ( IID_IInterface              == riid )
      { *ppI = static_cast<IInterface*>     (this); }
    else 
      { return   StatusCode::FAILURE; } /// RETURN!!!
    addRef();
    return StatusCode::SUCCESS;
  }
  ///
private:
  ///
  std::string m_sdType;
  std::string m_ident;
  //
};

/** @def IMPLEMENT_GiGaStackAction
 *  useful macro to implement the concrete factory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 25 Apr 2002 
 */
#define IMPLEMENT_GiGaStackAction( x ) \
 static const     GiGaStackActionFactory<##x##>         s_##x##Factory ; \
 const           IGiGaStackActionFactory&##x##Factory = s_##x##Factory ;

// ============================================================================
// The END 
// ============================================================================
#endif  // GIGA_GiGaStackActionFACTORY_H
// ============================================================================













