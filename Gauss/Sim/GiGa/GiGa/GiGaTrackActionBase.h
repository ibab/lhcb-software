// $Id: GiGaTrackActionBase.h,v 1.12 2007-03-18 18:25:05 gcorti Exp $ 
#ifndef     GIGA_GiGaTrackActionBase_H
#define     GIGA_GiGaTrackActionBase_H 1 

// Include files
// from STL
#include <vector> 
// base class 
#include "GiGa/IGiGaTrackAction.h" 
#include "GiGa/GiGaBase.h" 

// Forward declaration
class G4Track;


/** @class GiGaTrackActionBase GiGaTrackActionBase.h
 *  
 *  Base class for implementation of concrete Tracking Action for GiGa
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/01/2001
 */

class GiGaTrackActionBase: 
  public virtual IGiGaTrackAction ,
  public         GiGaBase
{

public:

  /** standard constructor 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaTrackActionBase
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ;

  virtual ~GiGaTrackActionBase();  ///< Destructor

  /** initialize the track action  
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         initialize () ; 
  
  /** finalize the track action
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         finalize   () ;

  virtual void PreUserTrackingAction  ( const G4Track* );
  virtual void PostUserTrackingAction ( const G4Track* );
  
protected:
  
  /** accessor to Geant4 Tracking Manager 
   *  @return pointer to Geant4 Tracking Manager 
   */
  inline G4TrackingManager* trackMgr() const 
  { return G4UserTrackingAction::fpTrackingManager; }
  
private:

  GiGaTrackActionBase (); ///< no default constructor 
  GiGaTrackActionBase           ( const GiGaTrackActionBase& ); ///< no copy
  GiGaTrackActionBase& operator=( const GiGaTrackActionBase& ); ///< no =

};

#endif   ///< GIGA_GiGaTrackActionBase_H 















