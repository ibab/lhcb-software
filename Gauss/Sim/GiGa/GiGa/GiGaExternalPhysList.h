// $Id: GiGaExternalPhysList.h,v 1.5 2007-03-26 09:01:39 gcorti Exp $
#ifndef GIGA_GIGAEXTERNALPHYSLIST_H 
#define GIGA_GIGAEXTERNALPHYSLIST_H 1

// Include files
// GiGa 
#include "GiGa/GiGaPhysListBase.h"

/** @class GiGaExternalPhysList GiGaExternalPhysList.h 
 *  
 *  Helper class to import external physics list into GiGa.
 *
 *  "External Physics List" PHYSLIST
 *  MUST have default constructor and 
 *  MUST have VIRTUAL inheritnce fro G4VUserPhysicsList
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */

template<class PHYSLIST>
class GiGaExternalPhysList : 
  virtual public GiGaPhysListBase ,
  virtual public PHYSLIST
{ 
  /// own type 
  typedef GiGaExternalPhysList<PHYSLIST>   MyType ;

public:
  
  /// standard method for construction of particles
  virtual void ConstructParticle () { PHYSLIST::ConstructParticle () ; }
  
  /// standard method for construction of processes
  virtual void ConstructProcess  () { PHYSLIST::ConstructProcess  () ; }
  
  /// standard method for setting ot cuts 
  virtual void SetCuts           () { PHYSLIST::SetCuts () ; }
  
protected:
  
  /** standard constructor 
   *  
   *  @warning Constructor exploits the  virtual inheritance 
   *  Constructor COULD produce compiler warning
   *  for CORRECT  inheritance schema and 
   *  it MUST produce the compiler error 
   *  for INCORECT inheritance schema  
   *
   *  @see GiGaPhysListBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @param name of the instance 
   *  @param svc  service locator 
   */
  GiGaExternalPhysList
  (  const std::string& type   , 
     const std::string& name   , 
     const IInterface*  parent )
    : GiGaPhysListBase   ( type , name , parent )
    , PHYSLIST           () 
  {
    // check for virtual inheritance 
    // the following line COULD produce compiler warning
    // for CORRECT  inheritance schema and 
    // it MUST produce the compiler error 
    // for INCORECT inheritance schema  
    G4ParticleTable* tt = theParticleTable ;
  };
  
  /// destructor (virtual and protected)
  virtual ~GiGaExternalPhysList(){};
  
private:
  
  /// default constructor is private 
  GiGaExternalPhysList() ; ///< no default constructor 
  /// copy constructor id private 
  GiGaExternalPhysList ( const MyType& ) ; 
  /// assignement operator is private 
  MyType& operator=    ( const MyType& ) ; 
  ///
};

#endif // GIGA_GIGAEXTERNALPHYSLIST_H
