// $Id: GiGaFlushAlgorithm.cpp,v 1.1 2002-01-22 18:20:53 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// from GiGa 
#include "GiGa/IGiGaSvc.h"
// local
#include "GiGaFlushAlgorithm.h"

// From Geant4 
class G4Event;

// ============================================================================
/** @file GiGaFlushAlgorithm.cpp
 * 
 *  Implementation file for class : GiGaFlushAlgorithm
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 22/01/2002 
 */
// ============================================================================

// ============================================================================
/** decalration of teh mandatory factory for instantiation
 */
// ============================================================================
static const  AlgFactory<GiGaFlushAlgorithm>         s_factory ;
const        IAlgFactory&GiGaFlushAlgorithmFactory = s_factory ; 

// ============================================================================
/** Standard constructor
 *  @param Name name of the algorithm 
 *  @param SvcLoc pointer to service locator 
 */
// ============================================================================
GiGaFlushAlgorithm::GiGaFlushAlgorithm( const std::string& Name   ,
                                        ISvcLocator*       SvcLoc )
  : Algorithm ( Name , SvcLoc ) 
  , m_gigaSvcName ( "GiGaSvc" ) 
  , m_gigaSvc     ( 0         ) 
{ declareProperty( "GiGaService" , m_gigaSvcName ) ; };

// ============================================================================
/** destructor 
 */
// ============================================================================
GiGaFlushAlgorithm::~GiGaFlushAlgorithm() {}; 

// ============================================================================
/** standard algorithm initialization 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaFlushAlgorithm::initialize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  /// locate GiGa service
  StatusCode status =  
    serviceLocator()->service( m_gigaSvcName, m_gigaSvc , true ) ;
  if( status.isFailure() ) 
    {
      log << MSG::ERROR 
          << " Could not locate GiGaService using the name '" 
          << m_gigaSvcName << "'" << endreq ;
      return status ;
    }
  if( 0 == gigaSvc() ) 
    {
      log << MSG::ERROR 
          << " GiGaService name '" 
          << m_gigaSvcName << "' points to NULL!" << endreq ;
      return StatusCode::FAILURE ;
    }
  ///
  gigaSvc()->addRef();
  
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard algorithm execution 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaFlushAlgorithm::execute() 
{
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  if( 0 == gigaSvc() ) 
    {
      log << MSG::ERROR 
          << " IGiGaSvc* points to NULL! " << endreq ;
      return StatusCode::FAILURE ;
    }
  
  /// extract the event ( "flush the GiGa" )
  const G4Event* event = 0 ;
  *gigaSvc() >> event      ;
  ///
  
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard algorithm finalization 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaFlushAlgorithm::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  /// release services 
  if( 0 != gigaSvc() ) { gigaSvc()->release() ; m_gigaSvc = 0 ; }
  
  return StatusCode::SUCCESS;
};


// ============================================================================
// The End 
// ============================================================================
