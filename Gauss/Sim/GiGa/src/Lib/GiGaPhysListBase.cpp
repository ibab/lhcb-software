// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/Lib/GiGaPhysListBase.cpp,v 1.2 2001-04-27 08:54:50 ibelyaev Exp $ 
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// G4Wrapper
#include "G4Wrapper/Particle.h"
/// GiGa
#include "GiGa/GiGaPhysListBase.h"


///////////////////////////////////////////////////////////////////////////////////////
GiGaPhysListBase::GiGaPhysListBase( const std::string& nick , ISvcLocator* loc) 
  : GiGaBase          ( nick , loc  )
  , G4VUserPhysicsList()
  , m_DefaultCutValue ( 2.0 * mm    )
{ 
  /// redefine soem variables to exploit dinamic loading on Win2K
  G4VUserPhysicsList::theParticleTable    = G4Wrapper::getG4ParticleTable();
  G4VUserPhysicsList::theParticleIterator = G4Wrapper::getG4ParticleTable()->GetIterator();
  /// 
  declareProperty( "Cut" , m_DefaultCutValue );
 };
///////////////////////////////////////////////////////////////////////////////////////
GiGaPhysListBase::~GiGaPhysListBase(){};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaPhysListBase::queryInterface( const InterfaceID& id , void** ppI) 
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0 ; 
  if   ( IGiGaPhysList::interfaceID() == id ) 
    { *ppI = static_cast<IGiGaPhysList*> (this)    ; } 
  else                                        
    {  return GiGaBase::queryInterface( id , ppI ) ; } /// RETURN ;
  addRef();
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaPhysListBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() ){ return Error("Could not initialize GiGaBase class!", sc); }
  MsgStream log( msgSvc() , name() ); 
  if( defCut() != GetDefaultCutValue() )
    {
      log << MSG::DEBUG                << " Value of Default Cut is changed from " 
          << GetDefaultCutValue() / mm << " [mm] to " 
          << defCut()             / mm << " [mm] " << endreq; 
      SetDefaultCutValue( defCut() );        
    } 
  log << MSG::INFO << " Default Cut Value is set to be " 
      << GetDefaultCutValue() / mm << " [mm]" << endreq; 
  ///
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaPhysListBase::finalize   ()  { return GiGaBase::finalize   (); };
///////////////////////////////////////////////////////////////////////////////////////


