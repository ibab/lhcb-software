// $Id: GiGaPhysListGeantino.cpp,v 1.7 2002-04-25 13:02:05 ibelyaev Exp $ 
// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa
#include "GiGa/GiGaPhysListFactory.h"
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

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaPhysList( GiGaPhysListGeantino ) ;
      
// ============================================================================
// ============================================================================
GiGaPhysListGeantino::GiGaPhysListGeantino( const std::string& nick , 
                                            ISvcLocator* loc ) 
  : GiGaPhysListBase( nick , loc )
{};

// ============================================================================
// ============================================================================
GiGaPhysListGeantino::~GiGaPhysListGeantino()
{};

// ============================================================================
/** initialization of the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysListGeantino::initialize()
{
  /// initialize the base class 
  StatusCode sc = GiGaPhysListBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc);}
  ///
  Print("initialized succesfully") ;
  ///
  return StatusCode::SUCCESS;  
};

// ============================================================================
/** finalization of the object
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysListGeantino::finalize  ()
{
  ///
  Print("finalization") ;
  ///
  return GiGaPhysListBase::finalize();
};

// ============================================================================
// ============================================================================
void GiGaPhysListGeantino::ConstructParticle()
{
  // In this method, static member functions should be called
  // for all particles which you want to use.
  // This ensures that objects of these particle types will be
  // created in the program. 

  G4Geantino::GeantinoDefinition();
};

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
};

// ============================================================================
// The END 
// ============================================================================





