// $Id: CaloE1Correction.cpp,v 1.2 2002-07-21 19:25:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/07/21 18:56:17  ibelyaev
//  add new energy correction
//
// Revision 1.1  2002/06/21 11:02:46  ibelyaev
//  update in S-,L- and E-corrections
//
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
#include "CaloE1Correction.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : CaloE1Correction
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloE1CorrectionFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloE1Correction>         s_factory ;
const        IToolFactory&CaloE1CorrectionFactory = s_factory ; 
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
CaloE1Correction::CaloE1Correction
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent  )
  : CaloTool ( type, name , parent ) 
  , m_const0   ()
  , m_const1   ()
  , m_const2   ()
  , m_hypos    (               ) // list of allowed hypos
  , m_area     ()
{
  // interafce 
  declareInterface<ICaloHypoTool> (this);
  // properties 
  declareProperty ( "Calibrations0"   , m_const0 ) ;
  declareProperty ( "Calibrations1"   , m_const1 ) ;
  declareProperty ( "Calibrations2"   , m_const2 ) ;
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
CaloE1Correction::~CaloE1Correction() {}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloE1Correction::finalize   () 
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
StatusCode CaloE1Correction::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() ) 
    { return Error( "Could not initialize the base class CaloTool" , sc ) ; }
  // 
  if( m_const0.empty() ) { return Error("Empty vector of constants0!"); }
  if( m_const1.empty() ) { return Error("Empty vector of constants1!"); }
  if( m_const2.empty() ) { return Error("Empty vector of constants2!"); }
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
StatusCode CaloE1Correction::process    ( CaloHypo* hypo  ) const 
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
StatusCode CaloE1Correction::operator() ( CaloHypo* hypo  ) const
{
  // check arguments 
  if( 0 == hypo ) { return Error("CaloHypo* points to NULL");}
  // check the Hypo
  Hypotheses::const_iterator h = 
    std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() );
  // allowed hypo ?
  if( m_hypos.end() == h ) { return Error("Unknown hypothesis!"); }
  
  // check the position
  if( 0 == hypo->position () ) { return Error("CaloPosition* points to NULL!");}
  
  // get the cluster 
  CaloCluster* cluster = hypo->clusters().front();
  if( 0 == cluster ) 
    { return Error("Cluster is invalid!", StatusCode( 203 ) ) ; }
  
  // get cluster area 
  const unsigned int area = m_area( cluster );
  if( area >= 3 ) 
    { return Error("Invalid aree in Detector!"); }
  
  const Params& pars = 
    ( 0 == area ) ? m_const0 : 
    ( 1 == area ) ? m_const1 : m_const2 ;
  
  
  CaloPosition::Covariance& cov = hypo -> position() -> covariance() ;
  CaloPosition::Parameters& par = hypo -> position() -> parameters() ;
  
  const double energy = par( CaloPosition::E );
  
  par( CaloPosition::E )  = corr      ( energy , pars ) ;
  
  const double scale      = corrPrime ( energy , pars ) ;
  
  cov( CaloPosition::E , CaloPosition::E ) *= scale * scale ;
  cov( CaloPosition::X , CaloPosition::E ) *=         scale ;
  cov( CaloPosition::Y , CaloPosition::E ) *=         scale ;
  
  return StatusCode::SUCCESS ;
  
};  
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================

