/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// Tool factory 
#include "GaudiKernel/DeclareFactoryEntries.h"
/// Local 
#include "GDMLRunAction.h"

/// GDML writer
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"
#include "G4GDMLParser.hh"

DECLARE_TOOL_FACTORY(GDMLRunAction);

// ============================================================================
/** standard constructor 
 *  @see GiGaPhysListBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GDMLRunAction::GDMLRunAction(const std::string& type,
                             const std::string& name,
                             const IInterface*  parent) : 
    GiGaRunActionBase(type, name, parent),
    m_done(false) {  

  declareProperty("Schema", m_schema = "$GDML_base/src/GDMLSchema/gdml.xsd");
  declareProperty("Output", m_outfile = "LHCb.gdml");

};

// ============================================================================
/// Action performed at the begin of each run
// ============================================================================
void GDMLRunAction::BeginOfRunAction(const G4Run* run) {

  if (0 == run) { 
    Warning("BeginOfRunAction:: G4Run* points to NULL!"); 
  }

  if (m_done) return;
  m_done = true;
  /// Get world volume.
  G4VPhysicalVolume* g4wv = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
  if (!g4wv) {
    error() << "Null pointer to world volume" << endmsg;
    return;
  } 
  G4GDMLParser g4writer;
  try {
    g4writer.Write(m_outfile, g4wv, false, m_schema);
  } catch(std::logic_error &lerr) {
    error() << "Caught an exception " << lerr.what() << endmsg;
  }

}

