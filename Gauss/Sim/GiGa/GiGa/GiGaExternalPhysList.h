// $Id: GiGaExternalPhysList.h,v 1.2 2002-05-05 13:23:49 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/25 13:02:04  ibelyaev
//  small update
// 
// ============================================================================
#ifndef GIGA_GIGAEXTERNALPHYSLIST_H 
#define GIGA_GIGAEXTERNALPHYSLIST_H 1
// Include files
// GiGa 
#include "GiGa/GiGaPhysListBase.h"
#include "GiGa/GiGaPhysListFactory.h"

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
  /// friend factory for instantiation 
  friend class GiGaPhysListFactory<MyType>                ;
  ///
public:
  
  /// standard method for construction of particles
  virtual void ConstructParticle () { PHYSLIST::ConstructParticle () ; }
  
  /// standard method for construction of processes
  virtual void ConstructProcess  () { PHYSLIST::ConstructProcess  () ; }
  
  /// standard method for setting ot cuts 
  virtual void SetCuts           () { PHYSLIST::SetCuts () ; }
  
protected:
  
  /** standard constructor 
   *  @see GiGaPhysListBase 
   *  @see GiGaBase 
   *  @param name of the instance 
   *  @param svc  service locator 
   */
  GiGaExternalPhysList
  ( const std::string& name , 
    ISvcLocator*       svc  )
    : GiGaPhysListBase   ( name , svc )
    , PHYSLIST           () 
    //    , G4VUserPhysicsList ()
  {
    // check for virtual inheritance 
    // the following line could produce compiler warning
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

/** @def IMPLEMENT_ExternalPhysList
 *  useful macro to implement a static factory for
 *  instantiation of external("imported") physics list
 *  @see IMPLEMENT_GiGaPhysList
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define IMPLEMENT_ExternalPhysList( PL )                            \
 static const                                                       \
  GiGaPhysListFactory<GiGaExternalPhysList<PL> >  s_##PL##Factory ; \
 const                                                              \
 IGiGaPhysListFactory&##PL##Factory =             s_##PL##Factory;

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAEXTERNALPHYSLIST_H
// ============================================================================
