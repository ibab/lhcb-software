// $Id: GiGaBase.cpp,v 1.23 2005-12-09 17:20:22 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.22  2004/02/20 18:13:35  ibelyaev
//  major update in GiGaBase and GiGaTrajectory
//
// Revision 1.21  2003/09/22 13:57:10  ibelyaev
//  polishing of addRef/release/releaseTools/finalize
//
// ===========================================================================
#define GIGA_GIGABASE_CPP 1 
// ============================================================================
/// includes 
///  STL
#include <string>
#include <map>
///  GaudiKernel
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/IChronoStatSvc.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/IMagneticFieldSvc.h" 
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
///  GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Stat.h"
///  from GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaUtil.h" 
#include "GiGa/GiGaBase.h" 

// ============================================================================
/** standard constructor 
 *  @see AlgTool 
 *  @param type tool   type (?)  
 *  @param Name object name 
 *  @param parent point to parent object 
 */
// ============================================================================
GiGaBase::GiGaBase
( const std::string& type   ,
  const std::string& Name   ,
  const IInterface*  parent )
  : GaudiTool( type , Name , parent )
  ///
  , m_gigaName   ( "GiGa"                ) 
  , m_setupName  ( "GiGa"                ) 
  ///
  , m_gigaSvc    ( 0                     ) 
  , m_setupSvc   ( 0                     ) 
{
  ///
  if( 0 == svcLoc() ) 
  { throw GiGaException("GiGaBase():: ISvcLocator* points to NULL!") ; }
  ///
  declareInterface<IGiGaInterface>    ( this );
  declareInterface<IIncidentListener> ( this );
  ///
  declareProperty( "GiGaService"               ,  m_gigaName    ) ;
  declareProperty( "GiGaSetUpService"          ,  m_setupName   ) ;
  ///
};
// ============================================================================

// ============================================================================
// destructor 
// ============================================================================
GiGaBase::~GiGaBase() {};
// ============================================================================

// ============================================================================
/** initialize the object
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::initialize() 
{
  // initialize the base class 
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { 
    return Error("Could not initialize the base class AlgTool",sc);
  }

  // Check that can locate Services
  if( 0 == svcLoc()     ) {
    throw GiGaException("GiGaBase::ini ISvcLocator* points to NULL!");
  }

  // Load GiGa
  if( !m_gigaName.empty() ) { 
    m_gigaSvc = svc<IGiGaSvc>( m_gigaName , true );
  }
  else { 
    Warning("GiGa Service is not requested to be located"); 
  }

  // If the setup service is the same, just get the interface
  if( !m_setupName.empty() ) { 
    if( m_setupName == m_gigaName ) {
      m_setupSvc = dynamic_cast<IGiGaSetUpSvc*>( m_gigaSvc );
      
      // cast to correct type
    }
    else {
      m_setupSvc = svc<IGiGaSetUpSvc> ( m_setupName , true );
    }
  }
  else { 
    Warning("GiGaSetUp Service is not requested to be located");
  }
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** finalize the object 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaBase::finalize() { return GaudiTool::finalize() ; };
// ============================================================================

// ============================================================================
/** handle the incident
 *  @param incident reference to the incident
 */
// ============================================================================
void  GiGaBase::handle( const Incident& incident ) 
{ 
  Print ( " Incident = '"  + incident .type   () + "' " + 
          " \tsource = '"  + incident .source () + "' " ) ; 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================











