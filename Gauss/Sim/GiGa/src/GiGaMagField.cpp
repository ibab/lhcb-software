

#include "GiGa/GiGaMagField.h"
#include "GiGa/GiGaMagFieldFactory.h"


///////////////////////////////////////////////////////////////////////////////////////
GiGaMagField::GiGaMagField( const std::string& nick , ISvcLocator* loc) : GiGaBase ( nick , loc ) {};
///////////////////////////////////////////////////////////////////////////////////////
GiGaMagField::~GiGaMagField(){};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMagField::queryInterface( const InterfaceID& id , void** ppI) 
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0 ; 
  if   ( IID_IGiGaMagField == id ) { *ppI = static_cast<IGiGaMagField*> (this) ; } 
  else                           {  return GiGaBase::queryInterface( id , ppI ); } /// RETURN ;
  addRef();
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMagField::initialize ()  
{
  /// initialize base class 
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() ) { return Error("Could not initialize base class!", sc); }
  /// check explicitely for IMagneticFieldSvc!
  if( 0 == mfSvc  () ) { return Error("MagneticField Service is not located!"); } 
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMagField::finalize   ()  { return GiGaBase::finalize   (); };
///////////////////////////////////////////////////////////////////////////////////////
