// $Id: CaloECorrection.cpp,v 1.1 2002-06-21 11:02:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// Event 
#include "Event/CaloHypo.h"
// local
#include "CaloECorrection.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : CaloECorrection
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloECorrectionFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloECorrection>         s_factory ;
const        IToolFactory&CaloECorrectionFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool 
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance 
 *  @param parent  pointer to parent object (algorithm, service or tool)
 */
// ============================================================================
CaloECorrection::CaloECorrection
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent  )
  : CaloTool ( type, name , parent ) 
  , m_const    ()
  , m_hypos    (               ) // list of allowed hypos
  , m_area     ()
{
  // interafce 
  declareInterface<ICaloHypoTool> (this);
  // properties 
  declareProperty ( "Calibrations"   , m_const) ;
  //
  m_hypos.push_back( CaloHypotheses::Photon               ) ;
  m_hypos.push_back( CaloHypotheses::PhotonFromMergedPi0  ) ;
  m_hypos.push_back( CaloHypotheses::BremmstrahlungPhoton ) ;
  m_hypos.push_back( CaloHypotheses::Positron             ) ;
  m_hypos.push_back( CaloHypotheses::Electron             ) ;
  m_hypos.push_back( CaloHypotheses::EmCharged            ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloECorrection::~CaloECorrection() {}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloECorrection::finalize   () 
{
  // release calorimeter 
  setDet( (const DeCalorimeter*) 0 );
  // finalize the base class 
  return CaloTool::finalize ();
};
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloECorrection::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() ) 
    { return Error( "Could not initialize the base class CaloTool" , sc ) ; }
  // 
  if( m_const.empty() ) { return Error("Emptry vector of constants!"); }
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloECorrection::process    ( CaloHypo* hypo  ) const 
{
  return (*this) ( hypo ) ;
};
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloECorrection::operator() ( CaloHypo* hypo  ) const
{
  // check arguments 
  if( 0 == hypo ) { return Error("CaloHypo* points to NULL");}
  // check the Hypo
  Hypotheses::const_iterator h = 
    std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() );
  // allowed hypo ?
  if( m_hypos.end() == h ) { return Error("Unknown hypothesis!"); }
  
  // check the position 
  if( 0 == hypo->position() ) { return Error("CaloPosition* points to NULL!");}

  // get the cluster 
  CaloCluster* cluster = hypo->clusters().front();
  if( 0 == cluster ) 
    { return Error("Cluster is invalid!", StatusCode( 203 ) ) ; }
  
  // get cluster area 
  const unsigned int area = m_area( cluster );
  if( area >= m_const.size() ) 
    { return Error("Invalid aree in Detector!"); }
  
  const double correction = m_const[area];
  
  CaloPosition::Covariance& cov = hypo -> position() -> covariance() ;
  CaloPosition::Parameters& par = hypo -> position() -> parameters() ;
  
  par(                   CaloPosition::E ) *=              correction ;
  
  cov( CaloPosition::E , CaloPosition::E ) *= correction * correction ;
  cov( CaloPosition::X , CaloPosition::E ) *=              correction ;
  cov( CaloPosition::Y , CaloPosition::E ) *=              correction ;
  
  return StatusCode::SUCCESS ;
  
};  
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================

