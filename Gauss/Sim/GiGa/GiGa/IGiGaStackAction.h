/// ===========================================================================
/// CVS tag $Name:
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef     GIGA_IGIGASTACKACTION_H
#define     GIGA_IGIGASTACKACTION_H 1 
/// ===========================================================================
// STL 
#include <string> 
// base classes from GaudiKernel  
#include "GaudiKernel/IInterface.h"
// base class from Geant4 
#include "G4UserStackingAction.hh"
// unique IID 
#include "GiGa/IIDIGiGaStackAction.h"



/** @class IGiGaStackAction IGiGaStackAction.h GiGa/IGiGaStackAction.h
 *
 *  Definition of "pseudo-abstract" pseudo-interface 
 *                           for GiGa Stacking Action
 *
 *  @author  Vanya Belyaev
 *   @date    21/02/2001
 */

class IGiGaStackAction: virtual public G4UserStackingAction ,
                        virtual public IInterface            
{
  ///
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID ()  { return IID_IGiGaStackAction ; }
  ///
  /// identification 
  virtual const std::string& name        () const = 0 ;
  /// initialization method 
  virtual StatusCode         initialize  ()       = 0 ;
  /// finalization method 
  virtual StatusCode         finalize    ()       = 0 ;
  ///
  ///  virtual destructor   
  virtual ~IGiGaStackAction(){};
  ///
};


/// ===========================================================================
#endif  ///<  GIGA_IGIGASTACKACTION_H
/// ===========================================================================
