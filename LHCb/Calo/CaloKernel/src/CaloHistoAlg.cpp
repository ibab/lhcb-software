// $Id: CaloHistoAlg.cpp,v 1.1 2004-02-17 11:55:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloHistoAlg.h"
// ============================================================================


// ============================================================================
/** @file 
 *  Implementation file for class CaloHistoAlg
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-02-16
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see GaudiHistoAlg 
 *  @see  GaudiAlgorithm
 *  @see       Algorithm
 *  @param name           name of the algorithm
 *  @param pSvcLocator    pointer to Service Locator 
 */
// ============================================================================
CaloHistoAlg::CaloHistoAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
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
  setProperty      ( "HistoTopDir"  , "Calo/"      ) ;
};
// ============================================================================

// ============================================================================
///  destructor
// ============================================================================
CaloHistoAlg::~CaloHistoAlg() {}; 
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see  GaudiHistoAlg
 *  @see  GaudiAlgorithm
 *  @see       Algorithm
 *  @see      IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHistoAlg::initialize () 
{ return  GaudiHistoAlg::initialize () ; }
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see  GaudiHistoAlg
 *  @see  GaudiAlgorithm
 *  @see       Algorithm
 *  @see      IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHistoAlg::finalize   ()
{ return  GaudiHistoAlg::finalize   () ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
