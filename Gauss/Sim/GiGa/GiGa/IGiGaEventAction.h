/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef     GIGA_IGIGAEventACTION_H
#define     GIGA_IGIGAEventACTION_H 1 
/// ===========================================================================
// STL 
#include <string>
// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"
// base class from Geant4 
#include "G4UserEventAction.hh"
// unique IID 
#include "GiGa/IIDIGiGaEventAction.h"


/** @class IGiGaEventAction IGiGaEventAction.h GiGa/IGiGaEventAction.h
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
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaEventAction ; }
  ///
  /// identification 
  virtual const std::string& name        () const = 0 ;
  /// initialization method 
  virtual StatusCode         initialize  ()       = 0 ;
  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;
  ///
  ///  virtual destructor   
  virtual ~IGiGaEventAction(){};
  ///
};


/// ===========================================================================
#endif  ///<  GIGA_IGIGAEventACTION_H
/// ===========================================================================
