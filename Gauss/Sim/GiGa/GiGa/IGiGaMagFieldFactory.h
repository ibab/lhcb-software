#ifndef    GIGA_IGIGAMagFieldFACTORY_H
#define    GIGA_IGIGAMagFieldFACTORY_H 1 


#include <string>

#include "GaudiKernel/IFactory.h"

#include "GiGa/IIDIGiGaMagFieldFactory.h"


class ISvcLocator;
class IGiGaMagField;


/** @class IGiGaMagFieldFactory     IGiGaMagFieldFactory.h GiGa/IGiGaMagFieldFactory.h

    definition of (pseudu)abstract (pseudo)interface to factory for creation of Geant 4 Magnetic field class

    @author Vanya Belyaev
*/


class IGiGaMagFieldFactory: virtual public IFactory
{
  ///
 public:
  ///
  static const InterfaceID& interfaceID() { return IID_IGiGaMagFieldFactory; }
  ///
  virtual IGiGaMagField* instantiate(const std::string& name, ISvcLocator *svcloc) const = 0;
  /// Access to the magnetic field type
  virtual const std::string& magFieldType()    const  = 0;
  ///
  virtual ~IGiGaMagFieldFactory(){};
  ///
};



#endif  // GIGA_IGIGAMagFieldFACTORY_H
