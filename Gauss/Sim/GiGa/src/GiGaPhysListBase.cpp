/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaPhysListBase.h"

///////////////////////////////////////////////////////////////////////////////////////
GiGaPhysListBase::GiGaPhysListBase( const std::string& nick , ISvcLocator* loc) 
  : GiGaBase          ( nick , loc  )
  , m_DefaultCutValue ( 2.0 * mm    )
{ declareProperty( "Cut" , m_DefaultCutValue ); };
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


