// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/07/27 17:03:18  ibelyaev
/// improved printout
///
/// Revision 1.5  2001/07/23 13:12:11  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaPhysListBase.h"

/** imeplementation of class GiGaPhysListBase
 *
 *  @author Vanya Belyaev 
 */

// ============================================================================
// ============================================================================
GiGaPhysListBase::GiGaPhysListBase( const std::string& nick , ISvcLocator* loc) 
  : GiGaBase          ( nick , loc  )
  , m_DefaultCutValue ( 2.0 * mm    )
{ declareProperty( "Cut" , m_DefaultCutValue ); };

// ============================================================================
// ============================================================================
GiGaPhysListBase::~GiGaPhysListBase(){};

// ============================================================================
// ============================================================================
StatusCode GiGaPhysListBase::queryInterface( const InterfaceID& id , 
                                             void** ppI) 
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

// ============================================================================
// ============================================================================
StatusCode GiGaPhysListBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaBase class!", sc); }
  MsgStream log( msgSvc() , name() ); 
  if( defCut() != GetDefaultCutValue() )
    {
      log << MSG::DEBUG                
          << " Value of Default Cut is changed from " 
          << GetDefaultCutValue() / mm << " [mm] to " 
          << defCut()             / mm << " [mm] " << endreq; 
      SetDefaultCutValue( defCut() );        
    } 
  log << MSG::INFO << " Default Cut Value is set to be " 
      << GetDefaultCutValue() / mm << " [mm]" << endreq; 
  ///
  Print("GiGaPhysListBase initialized succesfully" ,
        StatusCode::SUCCESS , MSG::DEBUG );
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
// ============================================================================
StatusCode GiGaPhysListBase::finalize   ()  
{ 
  ///
  Print("GiGaPhysListBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG );
  ///
  return GiGaBase::finalize   (); 
};

// ============================================================================

