// $Id: CaloAlgorithm.cpp,v 1.18 2004-02-17 11:55:55 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define  CALOKERNEL_CALOALGORITHM_CPP 1 
// ============================================================================
// Include files
#include <algorithm>
#include <cstdlib> 
// from GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/ObjectVector.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
// LHCbKernel
#include "CaloKernel/CaloPrint.h"
// CaloKernel 
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloAlgorithm.h"
// local 
 
// ============================================================================
/** @file CaloAlgorithm.cpp
 * 
 *  Implementation file for class : CaloAlgorithm
 * 
 *  @aauthor Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 30/06/2001 
 */
// ============================================================================

// ============================================================================
/** Standard constructor (protected)
 *  @param name           name of the algorithm
 *  @param pSvcLocator    poinetr to Service Locator 
 */
// ============================================================================
CaloAlgorithm::CaloAlgorithm
( const std::string&  name       ,
  ISvcLocator*        pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  /// input/output/detector data
  , m_inputData  ( "" ) ///< no default value
  , m_inputs     (    ) ///< no default value
  , m_outputData ( "" ) ///< no default value
  , m_detData    ( "" ) ///< no default value
{
  ///
  declareProperty  ( "Input"        , m_inputData  ) ;
  declareProperty  ( "Inputs"       , m_inputs     ) ;
  declareProperty  ( "Output"       , m_outputData ) ;
  declareProperty  ( "Detector"     , m_detData    ) ;
  ///
};

// ============================================================================
/** Destructor
 */
// ============================================================================
CaloAlgorithm::~CaloAlgorithm() {};

// ============================================================================
/** standard initialization method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::initialize() 
{ return GaudiAlgorithm::initialize() ; };
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::finalize() 
{ return GaudiAlgorithm::finalize(); };
// ============================================================================

// ============================================================================
/** standard execution method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::execute() 
{ return Error ( "==> One should NEVER see this message!" ) ; } ;
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
