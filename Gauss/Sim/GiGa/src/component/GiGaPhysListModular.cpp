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
  : GiGaPhysListBase( type , name , parent ),G4VModularPhysicsList()
{ 
  declareProperty( "PhysicsConstructors"  , m_physconstr);
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysListModular::~GiGaPhysListModular(){};
// ============================================================================

// ============================================================================
StatusCode GiGaPhysListModular::initialize() 
{
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "GiGaPhysListModular initializing" << endreq;

  IGiGaPhysConstructor* theconstr;
  IToolSvc* toolSvc;
  
  StatusCode sc = svcLoc()->service( "ToolSvc" , toolSvc , true );
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
          StatusCode scc= toolSvc->retrieveTool(*i,theconstr);
          if(!scc) {
            log << MSG::WARNING << "Could not find: " << (*i) << endreq;
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

  SetCutsWithDefault();

  if (verboseLevel>0) { DumpCutValuesTable(); }

};

  

// ============================================================================
// The END 
// ============================================================================
