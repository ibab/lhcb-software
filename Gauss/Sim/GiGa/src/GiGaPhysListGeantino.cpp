
// GiGa
#include "GiGa/GiGaPhysListFactory.h"

// G4 
#include "G4ParticleTypes.hh"


// local
#include "GiGaPhysListGeantino.h"


/// factory
static const GiGaPhysListFactory<GiGaPhysListGeantino>             s_GiGaPhysListGeantinoFactory;
const       IGiGaPhysListFactory&    GiGaPhysListGeantinoFactory = s_GiGaPhysListGeantinoFactory;
      

/// constructor 
GiGaPhysListGeantino::GiGaPhysListGeantino( const std::string& nick , ISvcLocator* loc ) 
  : GiGaPhysListBase( nick , loc )
{};
/// destructor 
GiGaPhysListGeantino::~GiGaPhysListGeantino()
{};
//////////////////////////////////////////////////////////////////
void GiGaPhysListGeantino::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  G4Geantino::GeantinoDefinition();
};
////////////////////////////////////////////////////////////////////////////////
void GiGaPhysListGeantino::ConstructProcess()
{
  // Define transportation process

  AddTransportation();
}
////////////////////////////////////////////////////////////////////////////////
void GiGaPhysListGeantino::SetCuts()
{
  // uppress error messages even in case e/gamma/proton do not exist            
  G4int temp = GetVerboseLevel(); SetVerboseLevel(0);                                         
  
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  SetCutsWithDefault();   

  // Retrieve verbose level
  SetVerboseLevel(temp);  
};
///////////////////////////////////////////////////////////////////////////////





