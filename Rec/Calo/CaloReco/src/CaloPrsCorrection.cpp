// $Id: CaloPrsCorrection.cpp,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/09/04 14:46:16  ibelyaev
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
#include "CaloPrsCorrection.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : CaloPrsCorrection
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloPrsCorrectionFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloPrsCorrection>         s_factory ;
const        IToolFactory&CaloPrsCorrectionFactory = s_factory ; 
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
CaloPrsCorrection::CaloPrsCorrection
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent  )
  : CaloTool     ( type, name , parent ) 
  , m_const0     ()
  , m_const0_cnv ()
  , m_const1     ()
  , m_const1_cnv ()
  , m_const2     ()
  , m_const2_cnv ()
  , m_hypos      (                     ) // list of allowed hypos
  , m_area       ()
  , m_spd        ( DeCalorimeterLocation::Spd )
  , m_prs        ( DeCalorimeterLocation::Prs )
  , m_threshold  ( 2   * MeV  ) 
  , m_spdcut     ( 0.1 * MeV  ) 
{
  // interafce 
  declareInterface<ICaloHypoTool> (this);
  // properties 
  declareProperty   ( "Param0"          , m_const0         ) ;
  declareProperty   ( "Param1"          , m_const1         ) ;
  declareProperty   ( "Param2"          , m_const2         ) ;
  declareProperty   ( "Param0Cnv"       , m_const0_cnv     ) ;
  declareProperty   ( "Param1Cnv"       , m_const1_cnv     ) ;
  declareProperty   ( "Param2Cnv"       , m_const2_cnv     ) ;
  
  // threshold 
  declareProperty   ( "Threshold"       , m_threshold      ) ;
  declareProperty   ( "SpdCut"          , m_spdcut         ) ;
  //
  m_hypos.push_back ( CaloHypotheses::Photon               ) ;
  m_hypos.push_back ( CaloHypotheses::PhotonFromMergedPi0  ) ;
  m_hypos.push_back ( CaloHypotheses::BremmstrahlungPhoton ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloPrsCorrection::~CaloPrsCorrection() {}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPrsCorrection::finalize   () 
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
StatusCode CaloPrsCorrection::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() ) 
    { return Error( "Could not initialize the base class CaloTool" , sc ) ; }
  // 
  if( 4 != m_const0     .size() ) { return Error("Wrong vector size 0"   ); }
  if( 4 != m_const1     .size() ) { return Error("Wrong vector size 1"   ); }
  if( 4 != m_const2     .size() ) { return Error("Wrong vector size 2"   ); }
  if( 4 != m_const0_cnv .size() ) { return Error("Wrong vector size 0Cnv"); }
  if( 4 != m_const1_cnv .size() ) { return Error("Wrong vector size 1Cnv"); }
  if( 4 != m_const2_cnv .size() ) { return Error("Wrong vector size 2Cnv"); }
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
StatusCode CaloPrsCorrection::process    ( CaloHypo* hypo  ) const 
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
StatusCode CaloPrsCorrection::operator() ( CaloHypo* hypo  ) const
{
  // avoid long names 
  typedef const CaloHypo::Digits Digits;
  
  // check arguments 
  if( 0 == hypo ) { return Error("CaloHypo* points to NULL");}
  // check the Hypo
  Hypotheses::const_iterator h = 
    std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() );
  // allowed hypo ?
  if( m_hypos.end() == h ) { return Error("Unknown hypothesis!"); }
  
  // accumulate the preshower information 
  double eprs = 0 ;
  double espd = 0 ;
  const Digits& digits = hypo->digits();
  for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ) 
    { 
      if     ( 0 ==   *d   ) { continue           ; }
      else if( m_prs( *d ) ) { eprs  += (*d)->e() ; } 
      else if( m_spd( *d ) ) { espd  += (*d)->e() ; } 
    }
  // converted photon ?
  const bool cnv = espd > m_spdcut ;
  
  // check the position
  if( 0 == hypo->position () ) { return Error("CaloPosition* points to NULL!");}
  
  // get the cluster 
  CaloCluster* cluster = hypo->clusters().front();
  if( 0 == cluster ) { return Error("Cluster is invalid!", 203 ) ;}
  
  // get cluster area 
  const unsigned int area = m_area( cluster );
  if( area > 2     ) { return Error("Invalid area in Detector!"); }
  
  const Params& pars = 
    0 == area ? 
    ( cnv ? m_const0 : m_const0_cnv ) :
    1 == area ? 
    ( cnv ? m_const1 : m_const1_cnv ) : ( cnv ? m_const2 : m_const2_cnv ) ;
  
  CaloPosition::Covariance& cov = hypo -> position() -> covariance() ;
  CaloPosition::Parameters& par = hypo -> position() -> parameters() ;
  
  const double energy = par( CaloPosition::E );
  
  par( CaloPosition::E )  = corr      ( energy , eprs , pars ) ;
  
  const double scale      = corrPrime ( energy , eprs , pars ) ;
  
  cov( CaloPosition::E , CaloPosition::E ) *= scale * scale ;
  cov( CaloPosition::X , CaloPosition::E ) *=         scale ;
  cov( CaloPosition::Y , CaloPosition::E ) *=         scale ;
  
  return StatusCode::SUCCESS ;
  
};  
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================

