// $Id: IGiGaVisManager.h,v 1.1 2002-12-04 21:12:49 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_IGIGAVISMANAGER_H 
#define GIGA_IGIGAVISMANAGER_H 1
// Include files
// GiGa
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IIDIGiGaVisManager.h"
// forward declaration class from Geant4
class G4VVisManager;

/** @class IGiGaVisManager IGiGaVisManager.h GiGa/IGiGaVisManager.h
 *  
 *
 *  Definition of pseudo-abstract pseudo-interface for 
 *  GiGa Interactive User interface  
 * 
 *  Here the first time the new phylosophy is applied, 
 *  inpired by J.P.Wellish
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-04
 */
class IGiGaVisManager : public virtual IGiGaInterface 
{

public:
  
  /** Retrieve the unique interface ID (static)
   *  @see IInterface
   */
  static const InterfaceID& interfaceID ()  { return IID_IGiGaVisManager ; }
  
  /** get the pointer to G4 visual manager
   *  @return pointer to G4 visual manager 
   */
  virtual G4VVisManager* visMgr () const = 0 ;
  
protected:
  
  /// destructor (virtual and protected)
  virtual ~IGiGaVisManager(){}; 
  
private:
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_IGIGAVISMANAGER_H
// ============================================================================
