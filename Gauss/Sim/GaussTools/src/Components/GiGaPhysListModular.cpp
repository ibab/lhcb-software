// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
#include "GiGa/IGiGaPhysicsConstructor.h"
// G4 
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4Material.hh"
#include "G4ios.hh"
#include "g4std/iomanip"                
// local
#include "GiGaPhysListModular.h"

/** @file 
 * 
 *  implementation of class GiGaPhysListModular
 *
 *  @author Witek Pokorski Witold.Pokorski@cern.ch
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaPhysListModular ) ;
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
GiGaPhysListModular::GiGaPhysListModular
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaPhysListBase( type , name , parent ),G4VModularPhysicsList(), 
    m_electronCut(10.0*km), m_gammaCut(20*mm)
{
  declareProperty( "PhysicsConstructors"  , m_physconstr);
  declareProperty( "ElectronCut", m_electronCut);
  declareProperty( "GammaCut", m_gammaCut);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysListModular::~GiGaPhysListModular()
{
  m_physconstr.clear();
};
// ============================================================================

// ============================================================================
StatusCode GiGaPhysListModular::initialize() 
{
  StatusCode sc=GiGaPhysListBase::initialize ();
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaPhysListBase class!", sc); }
  
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "GiGaPhysListModular initializing" << endreq;
  
  IGiGaPhysicsConstructor* theconstr;

  if( m_physconstr.empty() ) 
    { return Error ( "Invalid/Empty list of Physics constructors" ) ; }
  
  for ( std::vector<std::string>::iterator constructor = m_physconstr.begin() ; 
        m_physconstr.end() != constructor ; ++constructor )
    {
      IGiGaPhysicsConstructor* theconstr = 
        tool( *constructor , theconstr , this ) ;
      if( 0 == theconstr ) { return StatusCode::FAILURE ; }
      
      if( 0 == theconstr -> physicsConstructor() ) 
        { return Error ( "G4PhysicsConstructor* points to NULL!" ) ; }
      
      // register 
      RegisterPhysics( theconstr -> physicsConstructor() ) ;
    }
  
  return StatusCode::SUCCESS;
};

// ============================================================================
void GiGaPhysListModular::SetCuts()
{
  //G4VUserPhysicsList::SetCutsWithDefault method sets 
  //the default cut value for all particle types 
  //
  
  SetCutsWithDefault();
  
  SetCutValue(m_gammaCut,"gamma");
  SetCutValue(m_electronCut,"e-");
  
  if (verboseLevel>0) { DumpCutValuesTable(); }
};

  

// ============================================================================
// The END 
// ============================================================================
