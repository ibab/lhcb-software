// $Id: GiGaDataStoreAlgorithm.cpp,v 1.1 2002-01-22 18:24:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
#include <functional>
// from Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h"
// local
#include "GiGaDataStoreAlgorithm.h"

// ============================================================================
/** @file GiGaDataStoreAlgorithm.cpp
 * 
 *  Implementation file for class : GiGaDataStoreAlgorithm
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 19/01/2002 
 */
// ============================================================================


// ============================================================================
/** Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<GiGaDataStoreAlgorithm>         s_Factory ;
const        IAlgFactory&GiGaDataStoreAlgorithmFactory = s_Factory ; 

// ============================================================================
/** Standard constructor
 *  @param Name algorithm name 
 *  @param SvcLoc pointer to Service Locator
 */
// ============================================================================
GiGaDataStoreAlgorithm::GiGaDataStoreAlgorithm
( const std::string& Name,
  ISvcLocator*       SvcLoc )
  : Algorithm ( Name , SvcLoc ) 
  , m_names    () 
  , m_services () 
{
  m_names.push_back( "GiGaKineCnvSvc" );
  m_names.push_back( "GiGaHitsCnvSvc" );
  declareProperty( "ConversionServices" , m_names );
};

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaDataStoreAlgorithm::~GiGaDataStoreAlgorithm() 
{
  m_names    .clear () ;
  m_services .clear () ;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDataStoreAlgorithm::finalize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  /// release all services 
  std::for_each( m_services.begin () , 
                 m_services.end   () , 
                 std::mem_fun(&IGiGaCnvSvc::release) ) ;
  /// clear the container 
  m_services.clear() ;
  return StatusCode::SUCCESS;
};

StatusCode GiGaDataStoreAlgorithm::initialize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialize" << endreq;
  
  for( Names::const_iterator item = m_names.begin() ; 
       m_names.end() != item ; ++item ) 
    {
      IGiGaCnvSvc* svc = 0 ;
      StatusCode sc =  serviceLocator()-> service( *item , svc , true );
      if( sc.isFailure() || 0 == svc ) 
        { 
          log << MSG::ERROR 
              << " Could not locate/create service '" + *item + "'" << endreq ;
          /// release all services 
          std::for_each( m_services.begin () , 
                         m_services.end   () , 
                         std::mem_fun(&IGiGaCnvSvc::release) ) ;
          /// clear the container 
          m_services.clear() ;
          return StatusCode::FAILURE ;
        }
      svc->addRef();
      m_services.push_back( svc );
      log << MSG::DEBUG << "==> add the service '" + *item + "'" << endreq;
    };
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard execution method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDataStoreAlgorithm::execute() 
{
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  for( Services::iterator svc = m_services.begin() ;
       m_services.end() != svc ; ++svc )
    {
      StatusCode sc = (*svc)->registerGiGaLeaves();
      if( sc.isFailure() ) 
        {
          log << MSG::ERROR 
              << " Could not register GiGa leaves!" << endreq ;
          return sc ;
        }
    };
  return StatusCode::SUCCESS;
};



// ============================================================================
// The End 
// ============================================================================
