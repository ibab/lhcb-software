#ifndef    GIGA_GIGAMagFieldFACTORY_H
#define    GIGA_GIGAMagFieldFACTORY_H 1 


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/FactoryTable.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ISvcLocator.h"

///
#include "GiGa/IGiGaMagFieldFactory.h" 

/** @class GiGaMagFieldFactory     GiGaMagFieldFactory.h GiGa/GiGaMagFieldFactory.h
    
    implementationof factory to create "Magnetic Field" class
    
    @author Vanya Belyaev
*/


template <class ConcreteMagField>
class GiGaMagFieldFactory: public IGiGaMagFieldFactory
{
 public:
  ///
  /// Default constructor
  GiGaMagFieldFactory() 
    {
      // Get the class name using the RTTI.
      m_MagFieldType     = System::typeinfoName( typeid( ConcreteMagField ) );
      m_ident      = m_MagFieldType;
      FactoryTable::instance()->addFactory( this );
    };
  /// Default destructor
  virtual ~GiGaMagFieldFactory(){};
  /// from IInterface  
  virtual unsigned long      addRef        ()       { return        1 ; }; 
  virtual unsigned long      release       ()       { return        1 ; }; 
  /// from IFactory 
  virtual unsigned long      addRef        () const { return        1 ; }; 
  virtual unsigned long      release       () const { return        1 ; }; 
  /// type name
  virtual const std::string& magFieldType  () const { return m_MagFieldType ; };  
  virtual const std::string& ident         () const { return m_ident  ; };

  /// create the instance 
  virtual IGiGaMagField*    instantiate( const std::string& name, ISvcLocator *svcloc) const 
    { return new ConcreteMagField(name, svcloc); }
  /// a little bit funny 
  virtual IInterface* instantiate( IInterface* parent                          ) const 
    {
      ISvcLocator* loc = 
        0 == parent                              ? Gaudi::svcLocator() : 
        0 == dynamic_cast<ISvcLocator*> (parent) ? Gaudi::svcLocator() : dynamic_cast<ISvcLocator*>(parent);
      return new ConcreteMagField( magFieldType() , loc ); 
    }
  ///
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppI) 
    {
      if( 0 == ppI ) { return StatusCode::FAILURE; } 
      ppI = 0 ; 
      if      ( IID_IGiGaMagFieldFactory == riid ) { *ppI = static_cast<IGiGaMagFieldFactory*> (this); }
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
  std::string m_MagFieldType;
  std::string m_ident;
  //
};
///

#endif  // GIGA_GIGAMagFieldFACTORY_H


