#ifndef     GIGA_IGiGaSensDetFACTORY_H
#define     GIGA_IGiGaSensDetFACTORY_H 1


// 
#include "GaudiKernel/IFactory.h"

#include "GiGa/IIDIGiGaSensDetFactory.h"


class ISvcLocator;
class IGiGaSensDet;


/** @class IGiGaSensDetFactory IGiGaSensDetFactory.h GiGa/IGiGaSensDetFactory.h
    
    The IGiGaSensDetFactory is the interface used to create contrete instances of GiGa Sensitive Detectors
    
    @author Vanya Belyaev
    @author 23/01/2001
*/

class IGiGaSensDetFactory : virtual public IFactory 
{
  ///
 public:
  ///
  /// Retrieve interface ID
  static  const InterfaceID&  interfaceID () { return IID_IGiGaSensDetFactory; }
  ///
  ///
  /// Create an instance of a concrete GiGaSensDet
  virtual       IGiGaSensDet* instantiate ( const std::string& ,  ISvcLocator* ) const = 0;
  ///
  /// Access to the GiGaSensDet type.
  virtual const std::string&  sensDetType ()    const  = 0;
  ///
};

///
///
///

#endif   // GIGA_IGiGaSensDetFACTORY_H

