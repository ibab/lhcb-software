#ifndef    GIGA_GiGaPhysListFACTORY_H
#define    GIGA_GiGaPhysListFACTORY_H 1 


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcLocator.h"

///
#include "GiGa/IGiGaPhysListFactory.h" 

/** @class GiGaPhysListFactory     GiGaPhysListFactory.h GiGa/GiGaPhysListFactory.h
    
    implementationof factory to create "Physics List" class
    
    @author Vanya Belyaev
*/


template <class ConcretePL>
class GiGaPhysListFactory: public IGiGaPhysListFactory
{
 public:
  ///
  /// Default constructor
  GiGaPhysListFactory() 
    {
      // Get the class name using the RTTI.
      m_plType     = System::typeinfoName( typeid( ConcretePL ) );
      m_ident      = m_plType;
      FactoryTable::instance()->addFactory( this );
    };
  /// Default destructor
  virtual ~GiGaPhysListFactory(){};
  /// from IInterface  
  virtual unsigned long      addRef        ()       { return        1 ; }; 
  virtual unsigned long      release       ()       { return        1 ; }; 
  /// from IFactory 
  virtual unsigned long      addRef        () const { return        1 ; }; 
  virtual unsigned long      release       () const { return        1 ; }; 
  /// GiGaPhysList type name
  virtual const std::string& physListType  () const { return m_plType ; };  
  virtual const std::string& ident         () const { return m_ident  ; };

  /// create the instance 
  virtual IGiGaPhysList*    instantiate( const std::string& name, ISvcLocator *svcloc) const 
    { return new ConcretePL(name, svcloc); }
  /// a little bit funny 
  virtual IInterface* instantiate( IInterface* parent                          ) const 
    {
      ISvcLocator* loc = 
        0 == parent                              ? Gaudi::svcLocator() : 
        0 == dynamic_cast<ISvcLocator*> (parent) ? Gaudi::svcLocator() : dynamic_cast<ISvcLocator*>(parent);
      return new ConcretePL( physListType() , loc ); 
    }
  ///
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
    {
      if( 0 == ppI ) { return StatusCode::FAILURE; } 
      ppI = 0 ; 
      if      ( IID_IGiGaPhysListFactory == riid ) { *ppI = static_cast<IGiGaPhysListFactory*> (this); }
      else if ( IID_IFactory       == riid ) { *ppI = static_cast<IFactory*>       (this); }
      else if ( IID_IInterface     == riid ) { *ppI = static_cast<IInterface*>     (this); }
      else                                   { return   StatusCode::FAILURE; } /// RETURN!!!
      ///
      addRef();
      ///
      return StatusCode::SUCCESS;
    }
  ///
 private:
  ///
  std::string m_plType;
  std::string m_ident;
  //
};
///

#endif  // GIGA_GiGaPhysListFACTORY_H


