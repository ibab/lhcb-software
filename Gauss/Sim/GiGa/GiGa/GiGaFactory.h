// $Id: GiGaFactory.h,v 1.1 2002-05-07 12:21:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_GIGAFACTORY_H 
#define GIGA_GIGAFACTORY_H 1
// Include files
// GaudiKernel 
#include "GaudiKernel/IToolFactory.h"
#include "GaudiKernel/System.h"

/** @class GiGaFactory GiGaFactory.h GiGa/GiGaFactory.h
 *  
 *  Generic GiGa Factory ( indeed it is just a Tool Factory )
 *  @see ToolFactory
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   06/05/2002
 */
// ============================================================================

template <class TYPE>
class GiGaFactory : virtual public IToolFactory
{ 
  std::string m_toolType;
  std::string m_ident;
  
public:
  
  // Default constructor.
  GiGaFactory()    
  {
    /// Get the class name using the RTTI.
    m_toolType = System::typeinfoName( typeid( TYPE ) );
    m_ident    = m_toolType; 
    FactoryTable::instance()->addFactory( this );
  };
  
  // Default Destructor
  virtual ~GiGaFactory()   {}
  
  // Tool type, meaning the concrete class. 
  virtual const std::string& toolType() const { return m_toolType; }
  
  // Create an instance of a concrete tool.
  virtual IAlgTool* instantiate   ( const std::string& name   , 
                                    const IInterface*  parent ) const 
  { return new TYPE( toolType() , name      , parent  ) ; }
  
  /** Create an instance of a generic Gaudi object:
   *   Method is disabled but it is necessary!
   */
  virtual IInterface* instantiate (       IInterface* parent    ) const 
  { return new TYPE( toolType() , "UNNAMED" , parent  ); }
  
  // IFactory implementation
  virtual unsigned long addRef     () const { return 1; }
  virtual unsigned long release    () const { return 1; }
  virtual const std::string& ident () const { return m_ident; }
  
  // IInterface implementation
  virtual unsigned long addRef  () { return 1; }
  virtual unsigned long release () { return 1; }
  
  virtual StatusCode queryInterface
  ( const InterfaceID& iid , 
    void**             ppi ) 
  {
    if( 0 == ppi )     { return StatusCode::FAILURE ; }
    if      ( IToolFactory:: interfaceID ()  ==   iid ) 
      { *ppi =  static_cast<IToolFactory*> ( this ) ; }
    else if ( IFactory    :: interfaceID ()  ==   iid ) 
      { *ppi =  static_cast<IFactory*>     ( this ) ; }
    else if ( IInterface  :: interfaceID ()  ==   iid ) 
      { *ppi =  static_cast<IInterface*>   ( this ) ; }
    else               { return StatusCode::FAILURE ; }
    //
    return StatusCode::SUCCESS;
  }
};

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAFACTORY_H
// ============================================================================
