#include "GaudiKernel/MsgStream.h"

#include "GiGa/GiGaPhysList.h"

///////////////////////////////////////////////////////////////////////////////////////
GiGaPhysList::GiGaPhysList( const std::string& nick , ISvcLocator* loc) 
  : GiGaBase          ( nick , loc  )
  , m_DefaultCutValue ( 5.0 * mm    )
  , m_misc ( 0 ) 
{ 
  declareProperty( "cut" , m_DefaultCutValue ); 
  declareProperty( "misc" , m_misc ); 
};
///////////////////////////////////////////////////////////////////////////////////////
GiGaPhysList::~GiGaPhysList(){};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaPhysList::queryInterface( const InterfaceID& id , void** ppI) 
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0 ; 
  if   ( IID_IGiGaPhysList == id ) { *ppI = static_cast<IGiGaPhysList*> (this) ; } 
  else {  return GiGaBase::queryInterface( id , ppI ); } /// RETURN ;
  addRef();
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaPhysList::initialize ()  
{
  /// initialize base class

  std::cout << " 1misc = " << m_misc << std::endl; 
 
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() ){ return Error("Could not initialize GiGaBase class!", sc); }

  std::cout << " 2misc = " << m_misc << std::endl; 

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
StatusCode GiGaPhysList::finalize   ()  { return GiGaBase::finalize   (); };
///////////////////////////////////////////////////////////////////////////////////////

