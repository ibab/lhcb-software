// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/07/23 13:11:44  ibelyaev
// the package restructurisation(II)
// 
// ============================================================================
#ifndef     GIGA_IGIGAEventACTION_H
#define     GIGA_IGIGAEventACTION_H 1 
// ============================================================================
// STL 
#include <string>
// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"
// base class from Geant4 
#include "G4UserEventAction.hh"
// unique IID 
#include "GiGa/IIDIGiGaEventAction.h"


/** @interface IGiGaEventAction IGiGaEventAction.h "GiGa/IGiGaEventAction.h"
 *
 *  Definition of "pseudo-abstract" pseudo-interface for GiGa Event Action
 *
 *  @author  Vanya Belyaev
 *  @date    17/03/2001
*/

class IGiGaEventAction: virtual public G4UserEventAction ,  
                        virtual public IInterface          
{
  ///
 public
:
  /// Retrieve the unique interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaEventAction ; }

  /// object identification 
  virtual const std::string& name        () const = 0 ;

  /// initialization method
  virtual StatusCode         initialize  ()       = 0 ;

  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;

  ///  virtual destructor   
  virtual ~IGiGaEventAction(){};

};


// ============================================================================
#endif  ///<  GIGA_IGIGAEventACTION_H
// ============================================================================
