// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/07/23 13:11:46  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef     GIGA_IGIGATrackACTION_H
#define     GIGA_IGIGATrackACTION_H 1 
// ============================================================================
// STL 
#include <string>
// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"
// base class from Geant4 
#include "G4UserTrackingAction.hh"
// unique IID 
#include "GiGa/IIDIGiGaTrackAction.h"
// 


/** @class IGiGaTrackAction IGiGaTrackAction.h GiGa/IGiGaTrackAction.h
 *
 *  Definition of "pseudo-abstract" pseudo-interface 
 *                          for GiGa Tracking Action
 *
 *  @author  Vanya Belyaev
 *   @date    21/02/2001
 */

class IGiGaTrackAction: virtual public G4UserTrackingAction , 
                        virtual public IInterface             
{
  ///
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaTrackAction ; }
  ///
  /// identification 
  virtual const std::string& name        () const = 0 ;
  /// initialization method 
  virtual StatusCode         initialize  ()       = 0 ;
  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;
  ///
  ///  virtual destructor   
  virtual ~IGiGaTrackAction(){};
  ///
};


// ============================================================================
#endif  ///<  GIGA_IGIGATrackACTION_H
// ============================================================================
