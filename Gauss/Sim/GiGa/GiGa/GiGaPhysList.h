#ifndef       GIGA_GiGaPhysList_H
#define       GIGA_GiGaPhysList_H 1 




/// base classes 
#include "GiGa/IGiGaPhysList.h"
#include "GiGa/GiGaBase.h"


/** @class  GiGaPhysList             GiGaPhysList.h GiGa/GiGaPhysList.h
    
    "implementation" of basis "Physics List"                                            
    it still has 3 pure abstract methods from G4VUserPhysicsList! 

    @author Vanya Belyaev
*/


class GiGaPhysList: virtual public IGiGaPhysList   , 
		    public          GiGaBase
{
  ///
 public:
  ///
  virtual StatusCode queryInterface( const InterfaceID& id , void** ppI); 
  ///
  virtual const std::string& name       () const { return GiGaBase::name() ; }
  /// 
  virtual StatusCode         initialize () ; 
  virtual StatusCode         finalize   () ; 
  ///
 protected:
  ///
  GiGaPhysList( const std::string& nick , ISvcLocator* loc);
  virtual ~GiGaPhysList();
  ///
  double    defCut() const { return m_DefaultCutValue; } 
  ///
 private:
  ///
  GiGaPhysList()                           ; // no default constructor 
  GiGaPhysList           ( const GiGaPhysList& ) ; // no copy
  GiGaPhysList& operator=( const GiGaPhysList& ) ; // no assignment  
  ///
 private:
  ///
  double    m_DefaultCutValue  ; //default cut value
  int       m_misc ; 
  ///
};
///

#endif    //  GIGA_GiGaPhysList_H
