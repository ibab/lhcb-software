// $Id: GiGaPhysListBase.cpp,v 1.10 2003-04-06 18:49:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// Revision 1.8  2002/05/07 12:21:33  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaPhysListBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  implementation of class GiGaPhysListBase
 *
 *  @author Vanya Belyaev 
 */
// ============================================================================

namespace GiGaPhysListBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaPhysListBase> s_Counter ;
#endif   
};

// ============================================================================
/** standard constructor
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaPhysListBase::GiGaPhysListBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaPhysicsListBase          ( type , name , parent  )
  , m_self                       ( 0 ) 
{ 
  declareInterface<IGiGaPhysList> ( this ) ;
  //
#ifdef GIGA_DEBUG
  GiGaPhysListBaseLocal::s_Counter.increment () ;
#endif
  //
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaPhysListBase::~GiGaPhysListBase()
{
  //
#ifdef GIGA_DEBUG
  GiGaPhysListBaseLocal::s_Counter.decrement () ;
#endif
  //
};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysListBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaPhysicsListBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaPhysicsListBase class!" , sc ) ; }
  
  // initilize the self pointer  
  m_self = this ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** "SetCuts" method sets a cut value for all particle types 
 *   in the particle table
 *   @see IGiGaPhysList 
 *   @see IGiGaPhysicsList 
 *   @see G4VUserPhysicsList 
 */
// ============================================================================
void GiGaPhysListBase::SetCuts () 
{
  MsgStream log( msgSvc() , name() );
  // set defautl cut
  SetDefaultCutValue( defaultCut () );
  log << MSG::INFO 
      << " The default cut          is set to \t" 
      << defaultCut        () / mm << " mm " << endreq ;
  
  // set cut values for gamma
  SetCutValue ( cutForGamma       () , "gamma"        ) ;
  log << MSG::INFO 
      << " The cut for gamma        is set to \t" 
      << cutForGamma       () / mm << " mm " << endreq ;
  
  // set cut values for electron
  SetCutValue ( cutForElectron    () , "e-"           ) ;
  log << MSG::INFO 
      << " The cut for electron     is set to \t" 
      << cutForElectron    () / mm << " mm " << endreq ;
  
  // set cut values for positron
  SetCutValue ( cutForPositron    () , "e+"           ) ;
  log << MSG::INFO 
      << " The cut for positron     is set to \t" 
      << cutForPositron    () / mm << " mm " << endreq ;
  
  // set cut values for proton 
  SetCutValue ( cutForProton      () , "proton"       ) ;
  log << MSG::INFO 
      << " The cut for proton       is set to \t" 
      << cutForProton      () / mm << " mm " << endreq ;
  
  // set cut values for anti_proton
  SetCutValue ( cutForAntiProton  () , "anti_proton"  ) ;
  log << MSG::INFO 
      << " The cut for anti_proton  is set to \t" 
      << cutForAntiProton  () / mm << " mm " << endreq ;
  
  // set cut values for neutron 
  SetCutValue ( cutForNeutron     () , "neutron"      ) ;
  log << MSG::INFO 
      << " The cut for neutron      is set to \t" 
      << cutForNeutron     () / mm << " mm " << endreq ;
  
  // set cut values for anti_neutron 
  SetCutValue ( cutForAntiNeutron () , "anti_neutron" ) ;
  log << MSG::INFO 
      << " The cut for anti_neutron is set to \t" 
      << cutForAntiNeutron () / mm << " mm " << endreq ;
  
};
// ============================================================================

// ============================================================================
/** finalize initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysListBase::finalize   ()  
{ return GiGaPhysicsListBase::finalize () ; };
// ============================================================================

// ============================================================================
/** get G4VUserPhysicsList 
 *  @see IGiGaPhysList
 *  @see IGiGaPhysicsList
 *  @see G4VUserPhysicsList
 *  @return pointer to G4VUserPhysicsList object
 */
// ============================================================================
G4VUserPhysicsList* GiGaPhysListBase::physicsList ()  const 
{
  if( 0 == m_self ) 
    {
      const G4VUserPhysicsList* aux = this ;
      m_self=const_cast<G4VUserPhysicsList*> ( aux );
    }
  if( 0 == m_self ) 
    { Error("physicsList(): G4VUserPhysicsList* invalid!"); }
  //
  return m_self ;
};
// ============================================================================
 
// ============================================================================
// The END 
// ============================================================================
