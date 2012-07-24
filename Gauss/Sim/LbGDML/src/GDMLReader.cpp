// Gaudi
#include "GaudiKernel/ToolFactory.h"
// Geant4
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"
#include "G4GDMLParser.hh"
#include "G4VisAttributes.hh"
// Local
#include "GDMLReader.h"

/** @file GDMLReader.cpp
 *
 *  Implementation of class : GDMLReader
 *
 */

DECLARE_TOOL_FACTORY(GDMLReader);

//=============================================================================
/// Constructor
//=============================================================================
GDMLReader::GDMLReader(const std::string& type,
                       const std::string& name,
                       const IInterface* parent) : 
    GaudiTool(type, name, parent) {

  declareInterface<IGDMLReader>(this);

  declareProperty("FileName", m_gdmlfile);
  declareProperty("TranslationX", m_tx = 0.);
  declareProperty("TranslationY", m_ty = 0.);
  declareProperty("TranslationZ", m_tz = 0.);
  declareProperty("RotationX", m_rx = 0.);
  declareProperty("RotationY", m_ry = 0.);
  declareProperty("RotationZ", m_rz = 0.);

}

//=============================================================================
/// Import geometry from GDML file and attach to world volume
//=============================================================================
StatusCode GDMLReader::import(G4VPhysicalVolume* world) {

  info() << "Loading GDML geometry description from file "
         << m_gdmlfile << "." << endmsg;
  /// Read the GDML file.
  G4GDMLParser g4parser;
  g4parser.Read(m_gdmlfile, false);
  /// Get the world volume.
  G4VPhysicalVolume* gdmlWorldPV = g4parser.GetWorldVolume();
  if (!gdmlWorldPV) {
    error() << "Could not retrieve world volume from file " 
            << m_gdmlfile << "." << endmsg;
    return StatusCode::FAILURE; 
  }
  G4LogicalVolume* gdmlWorldLV = gdmlWorldPV->GetLogicalVolume();

  /// Make an assembly volume to hold the daughter volumes of the GDML world.
  G4AssemblyVolume* gdmlAssembly = new G4AssemblyVolume();
  // Loop over the daughter volumes.
  const int nDaughters = gdmlWorldLV->GetNoDaughters();
  info() << "GDML world volume " << gdmlWorldLV->GetName()
         << " has "<< nDaughters << " daughter(s)" << endmsg;
  for (int i = nDaughters; i--;) {
    G4VPhysicalVolume* gdmlDaughterPV = gdmlWorldLV->GetDaughter(i);
    std::string nameDaughter = gdmlDaughterPV->GetName();
    info() << "Picking up volume " << nameDaughter
           << " from " << m_gdmlfile << endmsg;
    G4ThreeVector transDaughter = gdmlDaughterPV->GetTranslation();
    G4RotationMatrix* rotDaughter = gdmlDaughterPV->GetRotation();
    G4LogicalVolume* gdmlDaughterLV = gdmlDaughterPV->GetLogicalVolume();
    /// Give the GDML volume a different colour (to better distinguish it
    /// in the G4 visualization)
    G4VisAttributes* gdmlDaughterVisAtt = new G4VisAttributes(G4Colour(0, 1, 1));
    gdmlDaughterLV->SetVisAttributes(gdmlDaughterVisAtt);

    /// Add the volume to the assembly
    gdmlAssembly->AddPlacedVolume(gdmlDaughterLV,
                                  transDaughter, rotDaughter);
  }
  /// Set the position of the GDML assembly in the LHCb world.
  G4ThreeVector transAssembly(m_tx, m_ty, m_tz);
  G4RotationMatrix* rotAssembly = new G4RotationMatrix();
  rotAssembly->rotateX(m_rx * deg);
  rotAssembly->rotateY(m_ry * deg);
  rotAssembly->rotateZ(m_rz * deg);
  /// Place the assembly volume in the LHCb world.
  gdmlAssembly->MakeImprint(world->GetLogicalVolume(),
                            transAssembly, rotAssembly);
  return StatusCode::SUCCESS;

}

