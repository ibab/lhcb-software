// $Id: GiGaFlushAlgorithm.cpp,v 1.2 2004-02-22 16:52:39 ibelyaev Exp $
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
#include "GiGa/DumpG4Event.h"
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
  : GaudiAlgorithm ( Name , SvcLoc ) 
  , m_gigaSvcName ( "GiGa" ) 
  , m_gigaSvc     ( 0         ) 
{ 
  declareProperty( "GiGa" , m_gigaSvcName ) ; 
};

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
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName , true ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @return status code
 */
// ============================================================================
StatusCode GiGaFlushAlgorithm::execute() 
{
  
  if ( 0 == gigaSvc() ) 
  { m_gigaSvc = svc<IGiGaSvc>( m_gigaSvcName , true ) ; }
  
  if ( 0 == gigaSvc() ) 
  { return Error ( " execute(): IGiGaSvc* points to NULL" ) ;}
  
  // extract the event ( "flush the GiGa" )
  const G4Event* event = 0 ;
  *gigaSvc()  >> event     ;
  
  if ( msgLevel( MSG::INFO ) ) 
  { 
    info() << " Dump G4 event object " << endreq ;
    if( info().isActive() ) 
    { GiGaUtil::DumpG4Event ( info().stream() , event  ) ; }
    info() << endreq ;
  };

  return StatusCode::SUCCESS;
};


// ============================================================================
// The End 
// ============================================================================
