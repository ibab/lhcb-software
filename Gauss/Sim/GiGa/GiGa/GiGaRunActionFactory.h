/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef    GIGA_GiGaRunActionFACTORY_H
#define    GIGA_GiGaRunActionFACTORY_H 1 
/// ===========================================================================
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
 *  @author Vanya Belyaev
 *  @date   17/03/2001 
 */

template <class ConcreteRA>
class GiGaRunActionFactory: public IGiGaRunActionFactory
{
  ///
 public:
  /// Default constructor
  GiGaEventRunFactory() 
    {
      // Get the class name using the RTTI.
      m_raType     = System::typeinfoName( typeid( ConcreteEA ) );
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
///

#endif  // GIGA_GiGaEventActionFACTORY_H













