///
#include "GiGa/GiGaMagFieldBase.h"

///////////////////////////////////////////////////////////////////////////////////////
GiGaMagFieldBase::GiGaMagFieldBase( const std::string& nick , ISvcLocator* loc ) 
  : GiGaBase( nick , loc ) {};
/// virtual destructor ////////////////////////////////////////////////////////////////
GiGaMagFieldBase::~GiGaMagFieldBase() {};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMagFieldBase::queryInterface( const InterfaceID& id , void** ppI) 
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0 ; 
  if   ( IGiGaMagField::interfaceID() == id ) { *ppI = static_cast<IGiGaMagField*> (this) ; } 
  else                                        {  return GiGaBase::queryInterface( id , ppI ); } /// RETURN ;
  addRef();
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMagFieldBase::initialize ()  
{
  /// initialize base class 
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() ) { return Error("Could not initialize base class!", sc); }
  /// check explicitely for IMagneticFieldSvc!
  if( 0 == mfSvc  () ) { return Error("MagneticField Service is not located!"); } 
  return StatusCode::SUCCESS;
};
///////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMagFieldBase::finalize   ()  { return GiGaBase::finalize   (); };
///////////////////////////////////////////////////////////////////////////////////////












