// $Id: GiGaExternalPhysicsList.h,v 1.1 2003-04-06 18:49:45 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_GIGAEXTERNALPHYSICSLIST_H 
#define GIGA_GIGAEXTERNALPHYSICSLIST_H 1
// ============================================================================
// Include files
#include "GiGa/GiGaPhysicsListBase.h"
#include "GiGa/GiGaFactory.h"

/** @class GiGaExternalPhysicsList GiGaExternalPhysicsList.h
 *  
 *  Helper class to import external physics list into GiGa.
 *
 *  "External Physics List" PHYSLIST
 *  MUST have default constructor 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
template <class PHYSLIST>
class GiGaExternalPhysicsList : public  GiGaPhysicsListBase 
{
  // PhysList type 
  typedef PHYSLIST                           PhysList ;
  /// own type 
  typedef GiGaExternalPhysicsList<PhysList>  OwnType  ;
  /// friend factory for instantion
  friend class GiGaFactory<OwnType>  ;
  
public:
  
  /** get G4VUserPhysicsList 
   *  @see IGiGaPhysList
   *  @see IGiGaPhysicsList
   *  @see G4VUserPhysicsList
   *  @return pointer to G4VUserPhysicsList object
   */
  virtual G4VUserPhysicsList* physicsList ()  const 
  {
    if( 0 == m_physList ) 
      {
        m_physList = new PhysList(); 
        m_physList -> SetDefaultCutValue ( defaultCut() ) ;
      }
    return m_physList ;
  };
  
protected:
  
  /** standard constructor 
   *  @see GiGaPhysicsListBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @param name of the instance 
   *  @param svc  service locator 
   */
  GiGaExternalPhysicsList
  (  const std::string& type   , 
     const std::string& name   , 
     const IInterface*  parent )
    : GiGaPhysicsListBase   ( type , name , parent )
    , m_physList            ( 0                    )
  {};
  
  // destructor 
  virtual ~GiGaExternalPhysicsList() {}
  
private:
  
  // the physic slist itself 
  mutable G4VUserPhysicsList* m_physList ;
  
};
// ============================================================================

// ============================================================================
/** @def IMPLEMENT_ExternalPhysList
 *  useful macro to implement a static factory for
 *  instantiation of external("imported") physics list
 *  @see IMPLEMENT_GiGaPhysList
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================
#define IMPLEMENT_ExternalPhysicsList( PL )                                   \
 static const GiGaFactory<GiGaExternalPhysicsList<##PL##> > s_##PL##Factory ; \
 const           IFactory&##PL##Factory                   = s_##PL##Factory ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAEXTERNALPHYSICSLIST_H
// ============================================================================
