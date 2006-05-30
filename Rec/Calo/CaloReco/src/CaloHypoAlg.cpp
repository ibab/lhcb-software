// $Id: CaloHypoAlg.cpp,v 1.5 2006-05-30 09:42:02 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/11/07 12:12:42  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.3  2004/03/17 16:32:21  ibelyaev
//  add new (04) Photon calibrations from Olivier Deschamps
//
// Revision 1.2  2004/02/17 12:08:07  ibelyaev
//  update for new CaloKernel and CaloInterfaces
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
  : GaudiAlgorithm ( name , svcloc ) 
    , m_names () // default empty list of tools  
    , m_inputData()
{
  // list of tools to be applied 
  declareProperty( "Tools" , m_names );
  declareProperty( "InputData" , m_inputData);
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
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("CouGld not initialize the base class CaloAlgorithm",sc);}
  
  for( Names::const_iterator it = m_names.begin() ; 
       m_names.end() != it ; ++it ) 
  {
    ICaloHypoTool* t = tool<ICaloHypoTool>( *it ) ;
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
  // clear the container 
  m_tools.clear();  
  /// finalize the base class 
  return GaudiAlgorithm::finalize();
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
  typedef LHCb::CaloHypos  Hypos;
  
  Hypos* hypos = get<Hypos>( m_inputData ) ;
  if( 0 == hypos ) { return StatusCode::FAILURE ; }
  
  for( Hypos::const_iterator hypo = hypos->begin() ; 
       hypos->end() != hypo ; ++hypo ) 
  {
    // skip NUULS 
    if( 0 == *hypo ) { continue ; }
    // loop over all tools
    StatusCode sc = StatusCode::SUCCESS ;
    for( Tools::const_iterator itool = m_tools.begin() ; 
         m_tools.end() != itool && sc.isSuccess() ; ++itool )
    { sc = (**itool)( *hypo ); }
    if( sc.isFailure() ) 
    { Error("Error from the Tool! skip hypo!",sc) ; continue ; }  
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

