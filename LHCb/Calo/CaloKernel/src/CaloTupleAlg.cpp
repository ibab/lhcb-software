// $Id: CaloTupleAlg.cpp,v 1.1 2004-02-17 11:55:55 ibelyaev Exp $
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
#include "CaloKernel/CaloTupleAlg.h"
// ============================================================================


// ============================================================================
/** @file 
 *  Implementation file for class CaloTupleAlg
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-02-16
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see GaudiTupleAlg 
 *  @see  GaudiAlgorithm
 *  @see       Algorithm
 *  @param name           name of the algorithm
 *  @param pSvcLocator    pointer to Service Locator 
 */
// ============================================================================
CaloTupleAlg::CaloTupleAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
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
  setProperty      ( "NTupleTopDir" , "Calo/"      ) ;
};
// ============================================================================

// ============================================================================
///  destructor
// ============================================================================
CaloTupleAlg::~CaloTupleAlg() {}; 
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see  GaudiTupleAlg
 *  @see  GaudiAlgorithm
 *  @see       Algorithm
 *  @see      IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTupleAlg::initialize () 
{ return  GaudiTupleAlg::initialize () ; }
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see  GaudiTupleAlg
 *  @see  GaudiAlgorithm
 *  @see       Algorithm
 *  @see      IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTupleAlg::finalize   ()
{ return  GaudiTupleAlg::finalize   () ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
