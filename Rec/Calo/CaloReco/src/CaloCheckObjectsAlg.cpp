// $Id: CaloCheckObjectsAlg.cpp,v 1.1 2004-09-05 15:25:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloCheckObjectsAlg.h"
// ============================================================================

/** @file 
 *  Implementation file for class : CaloCheckObjectsAlg
 *
 *  @date 2004-09-05 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<CaloCheckObjectsAlg>         s_Factory ;
const        IAlgFactory&CaloCheckObjectsAlgFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm instance name 
 *  @param psvc pointer to Service Locator 
 */
// ============================================================================
CaloCheckObjectsAlg::CaloCheckObjectsAlg
( const std::string& name,
  ISvcLocator* pSvcLocator)
  : CaloAlgorithm ( name , pSvcLocator )
{};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloCheckObjectsAlg::~CaloCheckObjectsAlg() {}; 
// ============================================================================


// ============================================================================
/// execution of the algorithm
// ============================================================================
StatusCode CaloCheckObjectsAlg::execute() 
{
  
  if ( !inputData().empty() ) 
  {
    // bool OK = exist<DataObject> ( inputData() ) ;
    bool OK = SmartDataPtr<DataObject> ( evtSvc() , inputData() ) ;
    if ( !OK ) 
    { Warning ( "The inputData '" + inputData() + "' does not exists" ) ; }
  }
  
  for( CaloAlgorithm::Inputs::const_iterator it = inputs().begin() ;
       inputs().end() != it ; ++it ) 
  {
    const std::string& address = *it ;
    // bool OK = exist<DataObject> ( address  ) ;
    bool OK = SmartDataPtr<DataObject> ( evtSvc() , address ) ;
    if ( !OK ) 
    { Warning ( "The inputs '" + address + "' does not exists" ) ; }
  }
  
  return StatusCode::SUCCESS;
};

// ============================================================================
// The END 
// ============================================================================
