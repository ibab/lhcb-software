#ifndef    GIGA_GiGaSensDetFACTORY_H
#define    GIGA_GiGaSensDetFACTORY_H 1 


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcLocator.h"

///
#include "GiGa/IGiGaSensDetFactory.h" 



/** @class GiGaSensDetFactory     GiGaSensDetFactory.h GiGa/GiGaSensDetFactory.h
    
    implementation of factory to create "Sensitive Detector" factory class
    
    @author Vanya Belyaev
*/


template <class ConcreteSD>
class GiGaSensDetFactory: public IGiGaSensDetFactory
{
  ///
 public:
  /// Default constructor
  GiGaSensDetFactory() 
    {
      // Get the class name using the RTTI.
      m_sdType     = System::typeinfoName( typeid( ConcreteSD ) );
      m_ident      = m_sdType;
      FactoryTable::instance()->addFactory( this );
    };
  /// Default destructor
  virtual ~GiGaSensDetFactory(){};
  /// from IInterface  
  virtual unsigned long      addRef       ()       { return        1 ; }; 
  virtual unsigned long      release      ()       { return        1 ; }; 
  /// from IFactory 
  virtual unsigned long      addRef       () const { return        1 ; }; 
  virtual unsigned long      release      () const { return        1 ; }; 
  /// GiGaSensDet type name
  virtual const std::string& sensDetType  () const { return m_sdType ; };  
  virtual const std::string& ident        () const { return m_ident  ; };
  /// create the instance 
  virtual IGiGaSensDet*    instantiate( const std::string& name, ISvcLocator *svcloc) const 
    { return new ConcreteSD(name, svcloc); }
  /// a little bit funny 
  virtual IInterface* instantiate( IInterface* parent                          ) const 
    {
      ISvcLocator* loc = 
        0 == parent                              ? Gaudi::svcLocator() : 
        0 == dynamic_cast<ISvcLocator*> (parent) ? Gaudi::svcLocator() : dynamic_cast<ISvcLocator*>(parent);
      return new ConcreteSD( sensDetType() , loc ); 
    }
  ///
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
    {
      if( 0 == ppI ) { return StatusCode::FAILURE; } 
      ppI = 0 ; 
      if      ( IID_IGiGaSensDetFactory == riid ) { *ppI = static_cast<IGiGaSensDetFactory*> (this); }
      else if ( IID_IFactory       == riid ) { *ppI = static_cast<IFactory*>       (this); }
      else if ( IID_IInterface     == riid ) { *ppI = static_cast<IInterface*>     (this); }
      else                                   { return   StatusCode::FAILURE; } /// RETURN!!!
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
///

#endif  // GIGA_GiGaSensDetFACTORY_H


