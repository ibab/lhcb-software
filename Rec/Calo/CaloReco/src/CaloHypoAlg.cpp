// $Id: CaloHypoAlg.cpp,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/09/04 14:41:34  ibelyaev
//  add new algorithms for recalibration of Ecal
//
// Revision 1.1  2002/09/02 18:19:16  ibelyaev
//  new generic algorithm for applying CaloHypo tools to CaloHypo objects
//
// Revision 1.3  2002/06/15 12:53:13  ibelyaev
//  version update
//
// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h" 
// CaloEvent/Event
#include "Event/CaloHypo.h"
// local
#include "CaloHypoAlg.h"

// ============================================================================
/** @file CaloHypoAlg.cpp
 * 
 *  Template implementation file for class : CaloHypoAlg
 *  @see CaloHypoAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/09/2002
 */
// ============================================================================

// ============================================================================
/** @var CaloHypoAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypoAlg>         s_Factory ;
const        IAlgFactory&CaloHypoAlgFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloHypoAlg::CaloHypoAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  , m_names () // default empty list of tools  
{
  // list of tools to be applied 
  declareProperty( "Tools" , m_names );
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloHypoAlg::~CaloHypoAlg() {};

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypoAlg::initialize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  for( Names::const_iterator it = m_names.begin() ; 
       m_names.end() != it ; ++it ) 
    {
      ICaloHypoTool* t = tool( *it , t ) ;
      if( 0 == t ) { return Error("Could not locate the tool!"); }
      m_tools.push_back( t );
    };
  
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
StatusCode CaloHypoAlg::finalize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  // finalize all tools 
  std::for_each( m_tools.begin() , 
                 m_tools.end  () , 
                 std::mem_fun(&ICaloHypoTool::release) );
  // clear the container 
  m_tools.clear();
  
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
StatusCode CaloHypoAlg::execute() 
{
  // avoid long name and types 
  typedef CaloHypos  Hypos;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  Hypos* hypos = get( eventSvc() , inputData() , hypos ) ;
  if( 0 == hypos ) { return StatusCode::FAILURE ; }
  
  for( Hypos::const_iterator hypo = hypos->begin() ; 
       hypos->end() != hypo ; ++hypo ) 
    {
      // skip NUULS 
      if( 0 == *hypo ) { continue ; }
      // loop over all tools 

      /**
         std::cout << " hypo before tools " 
         << **hypo 
         << std::endl ;
      */
      
      StatusCode sc = StatusCode::SUCCESS ;
      for( Tools::const_iterator itool = m_tools.begin() ; 
           m_tools.end() != itool && sc.isSuccess() ; ++itool )
        {
          sc = (**itool)( *hypo );
          /**
             std::cout << " hypo after " << (*itool)->name() << " tool: " 
             << **hypo 
             << std::endl ;
          */
        }
      if( sc.isFailure() ) 
        { Error("Error from the Tool! skip hypo!",sc) ; continue ; }
      
      /**
         std::cout << " hypo ALL tools " 
         << **hypo 
         << std::endl ;
      */
    }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
