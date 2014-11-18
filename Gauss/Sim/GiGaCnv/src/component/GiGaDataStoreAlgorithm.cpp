// $Id: GiGaDataStoreAlgorithm.cpp,v 1.4 2007-01-12 15:45:55 ranjard Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/20 19:12:00  ibelyaev
//  upgrade for newer GiGa
//
// Revision 1.2  2002/12/07 14:36:26  ibelyaev
//  see $GIGACNVROOT/doc/release.notes
//
// Revision 1.1  2002/01/22 18:24:43  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
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
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
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
DECLARE_ALGORITHM_FACTORY( GiGaDataStoreAlgorithm )


// ============================================================================
/** Standard constructor
 *  @param Name algorithm name 
 *  @param SvcLoc pointer to Service Locator
 */
// ============================================================================
GiGaDataStoreAlgorithm::GiGaDataStoreAlgorithm
( const std::string& Name,
  ISvcLocator*       SvcLoc )
  : GaudiAlgorithm ( Name , SvcLoc ) 
  , m_names    () 
  , m_services () 
{
  m_names.push_back( IGiGaCnvSvcLocation::Kine );
  m_names.push_back( IGiGaCnvSvcLocation::Hits );
  declareProperty( "ConversionServices" , m_names );
}


// ============================================================================
/// destructor
// ============================================================================
GiGaDataStoreAlgorithm::~GiGaDataStoreAlgorithm() {}


// ============================================================================
/** standard inititialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDataStoreAlgorithm::initialize()
{
  // initialize the base class 
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) { return sc ; }
  
  m_services.clear() ;
  for( Names::const_iterator item = m_names.begin() ; 
       m_names.end() != item ; ++item ) 
  {
    IGiGaCnvSvc* cnv = svc<IGiGaCnvSvc>( *item , true );
    m_services.push_back( cnv );
  };
  ///
  return StatusCode::SUCCESS;
}


// ============================================================================
/** standard execution method 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDataStoreAlgorithm::execute() 
{
  
  for( Services::iterator svc = m_services.begin() ;
       m_services.end() != svc ; ++svc )
  {
    if( 0 == (*svc) ) { continue ; }
    StatusCode sc = (*svc)->registerGiGaLeaves();
    if (sc.isFailure() ) 
    { return Error ( " Could not register GiGa leaves!", sc ) ; } 
  }
  return StatusCode::SUCCESS;
}



// ============================================================================
// The End 
// ============================================================================
