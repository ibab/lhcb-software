// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/IGiGaPhysListFactory.h,v 1.2 2001-03-18 16:04:27 ibelyaev Exp $ 
#ifndef    GIGA_IGiGaPhysListFACTORY_H
#define    GIGA_IGiGaPhysListFACTORY_H 1 
// STL 
#include <string>
// GaudiKernel
#include "GaudiKernel/IFactory.h"
// GiGa 
#include "GiGa/IIDIGiGaPhysListFactory.h"
//
class ISvcLocator;
class IGiGaPhysList;
// 

/** @class IGiGaPhysListFactory     IGiGaPhysListFactory.h GiGa/IGiGaPhysListFactory.h

    definition of (pseudu)abstract (pseudo)interface to factory for Geant 4 Physics List class

    @author Vanya Belyaev
*/


class IGiGaPhysListFactory: virtual public IFactory
{
  ///
 public:
  ///
  static const InterfaceID& interfaceID() { return IID_IGiGaPhysListFactory; }
  ///
  virtual IGiGaPhysList* instantiate(const std::string& name, ISvcLocator *svcloc) const = 0;
  /// Access to the physics list type
  virtual const std::string& physListType()    const  = 0;
  ///
  virtual ~IGiGaPhysListFactory(){};
  ///
};



#endif  // GIGA_IGiGaPhysListFACTORY_H
