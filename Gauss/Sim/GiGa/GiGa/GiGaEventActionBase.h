// $Id: GiGaEventActionBase.h,v 1.7 2002-05-07 12:21:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_GiGaEventActionBase_H
#define     GIGA_GiGaEventActionBase_H 1 
// ============================================================================
// base class 
#include "GiGa/IGiGaEventAction.h" 
#include "GiGa/GiGaBase.h" 

class G4EventManager;
class G4Event; 

/** @class GiGaEventActionBase GiGaEventActionBase.h GiGa/GiGaEventActionBase.h
 *
 *  Base class for implementation of concrete Event Action for GiGa
 *  
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class GiGaEventActionBase : 
  public virtual IGiGaEventAction ,
  public          GiGaBase
{
  
protected:
  
  /** standard  constructor
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type tool type (?)
   *  @param name  tool name 
   *  @param parent 
   */
  GiGaEventActionBase 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent );
  
  /// virtual destructor 
  virtual ~GiGaEventActionBase() ;
  
public: 
  
  /** initialize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 
  
  /** finalize the object 
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () ; 

protected: 
  
  /// accessor to Geant4 Event Manager 
  inline const G4EventManager*   evtMgr() const 
  { return G4UserEventAction::fpEventManager; }
  
private:
  ///
  GiGaEventActionBase () ; ///< no default constructor!                   
  GiGaEventActionBase ( const GiGaEventActionBase& ); ///< no copy!
  GiGaEventActionBase& operator=( const GiGaEventActionBase& ); ///< no = ! 
  ///
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif   // GIGA_GiGaEventActionBase_H
// ============================================================================














