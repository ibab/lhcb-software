/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
/// Local 
#include "GDMLRunAction.h"

// GDML writer
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"
#include "WriterG4/G4GDMLWriter.h"


// ============================================================================
/// Factory business
// ============================================================================
IMPLEMENT_GiGaFactory( GDMLRunAction ) ;
// ============================================================================

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
GDMLRunAction::GDMLRunAction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaRunActionBase( type , name , parent )
{  
  m_test=false;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GDMLRunAction::~GDMLRunAction()
{
};

// ============================================================================
/** performe the action at the begin of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void GDMLRunAction::BeginOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("BeginOfRunAction:: G4Run* points to NULL!") ; }
  /// get Geant4 UI manager 

  if(!m_test)
  {
    m_test=true;
    
    G4VPhysicalVolume* g4wv = G4TransportationManager::GetTransportationManager()->
      GetNavigatorForTracking()->GetWorldVolume();
    
    if(g4wv)
    {
      G4GDMLWriter g4writer("/afs/cern.ch/sw/lcg/app/releases/GDML/pro/schema/gdml_2.0.xsd", "lhcbtest.gdml");
      
      try
      {
        g4writer.DumpGeometryInfo(g4wv);
      }
      catch(std::logic_error &lerr)
      {
        std::cout << "Caught an exception: " 
                  << lerr.what () << std::endl;
      }
    }
    else std::cout << "GDMLRunAction: Null pointer to world volume" << std::endl;
  }
};
// ============================================================================

// ============================================================================
/** performe the action at the end of each run 
 *  @param run pointer to Geant4 run object 
 */
// ============================================================================
void GDMLRunAction::EndOfRunAction( const G4Run* run )
{
  if( 0 == run ) 
    { Warning("EndOfRunAction:: G4Run* points to NULL!") ; }
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================

