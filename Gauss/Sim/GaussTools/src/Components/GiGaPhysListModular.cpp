// ============================================================================
// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IToolSvc.h"
// GiGa
#include "GiGa/GiGaMACROs.h"
#include "GiGa/IGiGaPhysConstructor.h"
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
    m_nuCut(5*m), m_electronCut(10.0*km), m_gammaCut(20*mm)
{
  declareProperty( "PhysicsConstructors"  , m_physconstr);
  declareProperty( "NuCut", m_nuCut);
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
  
  IGiGaPhysConstructor* theconstr;
  IToolSvc* toolSvc;
  
  sc = svcLoc()->service( "ToolSvc" , toolSvc , true );
  if(!sc)
    {
      log << MSG::ERROR << "Could not find ToolSvc..." << endreq;
    }
  else
    {
      std::vector<std::string>::iterator i;  
      for( i=m_physconstr.begin(); i!= m_physconstr.end(); i++)
        {
          log << MSG::INFO << "Registering physics list: " << (*i) << endreq;
          StatusCode scc= toolSvc->retrieveTool( *i , theconstr , this );
          if(!scc) {
            return Error("Physics constructor '"+(*i)+"i is not found!",scc);
          }
          else
            {
              RegisterPhysics(theconstr);
            }
        }  
    }
  return StatusCode::SUCCESS;
};

// ============================================================================
void GiGaPhysListModular::SetCuts()
{
  //G4VUserPhysicsList::SetCutsWithDefault method sets 
  //the default cut value for all particle types 
  //
  
  SetCutValue(m_nuCut,"nu_e");
  SetCutValue(m_nuCut,"nu_tau");
  SetCutValue(m_nuCut,"nu_mu");
  SetCutValue(m_nuCut,"anti_nu_e");
  SetCutValue(m_nuCut,"anti_nu_tau");
  SetCutValue(m_nuCut,"anti_nu_mu");
  SetCutValue(m_gammaCut,"gamma");
  SetCutValue(m_electronCut,"e-");

  SetCutsWithDefault();
  
  if (verboseLevel>0) { DumpCutValuesTable(); }
};

  

// ============================================================================
// The END 
// ============================================================================
