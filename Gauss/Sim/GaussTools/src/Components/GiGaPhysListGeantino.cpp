// $Id: GiGaPhysListGeantino.cpp,v 1.2 2007-01-12 15:36:52 ranjard Exp $ 
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// GiGa
//#include "GiGa/GiGaMACROs.h"
// G4 
#include "G4ParticleTypes.hh"
// local
#include "GiGaPhysListGeantino.h"

/** @file 
 * 
 *  Implementation of class GiGaPjysListGeantino
 *  
 *  @author Vanya Belyaev
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaPhysListGeantino )
      
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
GiGaPhysListGeantino::GiGaPhysListGeantino
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysListBase( type , name , parent )
{}


// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysListGeantino::~GiGaPhysListGeantino() {}


// ============================================================================
// ============================================================================
void GiGaPhysListGeantino::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  G4Geantino::GeantinoDefinition();
}

// ============================================================================
// ============================================================================
void GiGaPhysListGeantino::ConstructProcess()
{
  // Define transportation process

  AddTransportation();
}

// ============================================================================
// ============================================================================
void GiGaPhysListGeantino::SetCuts()
{
  // uppress error messages even in case e/gamma/proton do not exist            
  G4int temp = GetVerboseLevel(); SetVerboseLevel(0);

  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  SetCutsWithDefault();   

  // Retrieve verbose level
  SetVerboseLevel(temp);  
}

// ============================================================================
// The END 
// ============================================================================





