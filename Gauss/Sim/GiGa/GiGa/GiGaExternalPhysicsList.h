// $Id: GiGaExternalPhysicsList.h,v 1.2 2007-03-26 09:01:39 gcorti Exp $
#ifndef GIGA_GIGAEXTERNALPHYSICSLIST_H 
#define GIGA_GIGAEXTERNALPHYSICSLIST_H 1

// Include files
#include "GiGa/GiGaPhysicsListBase.h"

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

#endif // GIGA_GIGAEXTERNALPHYSICSLIST_H
