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
    m_dumpCutsTable(false)
{
  declareProperty( "PhysicsConstructors"  , m_physconstr);
  declareProperty( "DumpCutsTable", m_dumpCutsTable);
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
      
      m_constructors.push_back( theconstr );
      
      // register 
      RegisterPhysics( theconstr -> physicsConstructor() ) ;
    }
  
  return StatusCode::SUCCESS;
};

// ============================================================================
StatusCode GiGaPhysListModular::finalize () 
{
  // release all constructors 
  for( Constructors::iterator ic = m_constructors.begin() ; 
       m_constructors.end() != ic ; ++ic ) 
    { if( 0 != *ic ) { (*ic) -> finalize () ; } }
  m_constructors.clear() ;
  
  return GiGaPhysListBase::finalize  ();
};


// ============================================================================
void GiGaPhysListModular::SetCuts()
{
  MsgStream log( msgSvc() , name() );
  
  // set cut values for gamma
  SetCutValue ( cutForGamma       () , "gamma"        ) ;
  log << MSG::INFO 
      << " The production cut for gamma is set to \t" 
      << cutForGamma       () / mm << " mm " << endreq ;
  
  // set cut values for electron
  SetCutValue ( cutForElectron    () , "e-"           ) ;
  log << MSG::INFO 
      << " The production cut for electron is set to \t" 
      << cutForElectron    () / mm << " mm " << endreq ;
  
  // set cut values for positron
  SetCutValue ( cutForPositron    () , "e+"           ) ;
  log << MSG::INFO 
      << " The production cut for positron is set to \t" 
      << cutForPositron    () / mm << " mm " << endreq ;
  
  if (m_dumpCutsTable) { DumpCutValuesTable(); }
};
  

// ============================================================================
// The END 
// ============================================================================
