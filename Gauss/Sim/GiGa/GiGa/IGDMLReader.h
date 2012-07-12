#ifndef IGDML_READER_H
#define IGDML_READER_H 1

#include "GaudiKernel/IAlgTool.h"

/** @class IGDMLReader IGDMLReader.h
 *  Interface class for importing GDML files 
 *
 */

// Declaration of the interface id 
static const InterfaceID IID_IGDMLReader("IGDMLReader", 1, 0);

class G4VPhysicalVolume;

class IGDMLReader : virtual public IAlgTool {

public:
  /// Static access to interface id
  static const InterfaceID& interfaceID() {return IID_IGDMLReader;}

  virtual StatusCode import(G4VPhysicalVolume* world) = 0;

};
#endif 
