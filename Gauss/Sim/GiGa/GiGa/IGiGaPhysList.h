#ifndef        GIGA_IGiGaPhysList_H
#define        GIGA_IGiGaPhysList_H 1 



#include "GaudiKernel/IInterface.h"


#include "G4VUserPhysicsList.hh"

#include "GiGa/IIDIGiGaPhysList.h"


/** @class IGiGaPhysList            IGiGaPhysList.h GiGa/IGiGaPhysList.h

    definition of (pseudu)abstract (pseudo)interface to Geant 4 Physics List class

    @author Vanya Belyaev
*/



class IGiGaPhysList: virtual public G4VUserPhysicsList ,
               virtual public IInterface         
{
  ///
 public:
  ///
  static const InterfaceID& interfaceID() { return IID_IGiGaPhysList; }
  /// ident 
  virtual const std::string&  name       () const = 0 ;
  /// initialization
  virtual StatusCode          initialize ()       = 0 ;
  /// finalization
  virtual StatusCode          finalize   ()       = 0 ; 
  /// virtual destructor 
  virtual ~IGiGaPhysList(){};
  ///
};



#endif    //   GIGA_IGiGaPhysList_H
