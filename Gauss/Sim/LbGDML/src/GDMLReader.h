#ifndef GDML_READER_H
#define GDML_READER_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "GiGa/IGDMLReader.h"

/** @class GDMLReader GDMLReader.h
 *
 *  Tool to import geometry from GDML file
 *
 */

class GDMLReader : public GaudiTool, 
                   virtual public IGDMLReader {
  
public: 
  /// Constructor
  GDMLReader(const std::string& type, 
             const std::string& name,
             const IInterface* parent);
  /// Destructor
  virtual ~GDMLReader() {}

  virtual StatusCode import(G4VPhysicalVolume* world);

private:

  /// Name of the GDML file to be loaded
  std::string m_gdmlfile;
  /// Translation
  double m_tx, m_ty, m_tz;
  /// Rotation (in degrees)
  double m_rx, m_ry, m_rz;
 
};
#endif 
