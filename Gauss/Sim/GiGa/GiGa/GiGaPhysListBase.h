#ifndef       GIGA_GiGaPhysListBase_H
#define       GIGA_GiGaPhysListBase_H 1 




/// base classes 
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/GiGaBase.h"


/** @class  GiGaPhysListBase             GiGaPhysListBase.h GiGa/GiGaPhysListBase.h
    
    "implementation" of basis "Physics List"                                            
    it still has 3 pure abstract methods from G4VUserPhysicsList! 

    @author Vanya Belyaev
*/


class GiGaPhysListBase: virtual public IGiGaPhysList   , 
			public          GiGaBase
{
  ///
protected:
  ///
  inline GiGaPhysListBase( const std::string& , ISvcLocator* );
  ///
  virtual inline ~GiGaPhysListBase();
  ///
public:
  ///
  virtual const  std::string& name       () const { return GiGaBase::name() ; }
  /// 
  virtual inline StatusCode   queryInterface( const InterfaceID&  , void** ); 
  ///
  virtual inline StatusCode   initialize () ; 
  virtual inline StatusCode   finalize   () ; 
  ///
protected:
  ///
  double    defCut() const { return m_DefaultCutValue; } 
  ///
private:
  ///
  GiGaPhysListBase()                                     ; // no default constructor 
  GiGaPhysListBase           ( const GiGaPhysListBase& ) ; // no copy
  GiGaPhysListBase& operator=( const GiGaPhysListBase& ) ; // no assignment  
  ///
private:
  ///
  double    m_DefaultCutValue  ; //default cut value
  ///
};
///
#include "GiGa/GiGaPhysListBase.icpp"
///
 
#endif    //  GIGA_GiGaPhysListBase_H





