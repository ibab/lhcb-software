// $Id: CaloDeleteObjectsAlg.cpp,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/04 14:41:34  ibelyaev
//  add new algorithms for recalibration of Ecal
//
// Revision 1.3  2002/06/15 12:53:13  ibelyaev
//  version update
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/DataObject.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
// local
#include "CaloDeleteObjectsAlg.h"

// ============================================================================
/** @file CaloDeleteObjectsAlg.cpp
 * 
 *  Template implementation file for class : CaloDeleteObjectsAlg
 *  @see CaloDeleteObjectsAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloDeleteObjectsAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloDeleteObjectsAlg>         s_Factory ;
const        IAlgFactory&CaloDeleteObjectsAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloDeleteObjectsAlg::CaloDeleteObjectsAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  , m_addresses ()
{
  declareProperty( "Addresses" , m_addresses );
};

// ============================================================================
/** destructor
 */
// ============================================================================
CaloDeleteObjectsAlg::~CaloDeleteObjectsAlg() {};

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDeleteObjectsAlg::initialize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDeleteObjectsAlg::finalize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  /// finalize the base class 
  return CaloAlgorithm::finalize();
};
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloDeleteObjectsAlg::execute() 
{
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // loop over all addresses 
  for( Addresses::const_iterator address = 
         m_addresses.begin() ; m_addresses.end() != address ; ++address )
    {
      // locate the object in TES 
      DataObject* object = get( eventSvc() , *address , object );
      if( 0 == object ) { return StatusCode::FAILURE ; }
      // unregister the object
      StatusCode sc = eventSvc()->unregisterObject( object ) ;
      if( sc.isFailure() ) 
        { return  Error("Could not unregister "+(*address),sc); }
      delete object ;
    }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
