// $Id: CaloLCorrection.cpp,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/07/03 10:59:10  ibelyaev
//  minor change
//
// Revision 1.4  2002/06/21 11:02:47  ibelyaev
//  update in S-,L- and E-corrections
//
// Revision 1.3  2002/06/19 17:09:46  ibelyaev
//  small update in L-correction
//
// Revision 1.2  2002/06/17 16:02:29  ibelyaev
//  remove the obvious bug in L-correction
//
// Revision 1.1  2002/06/14 17:46:05  ibelyaev
//  new L-correction
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
#include "CaloLCorrection.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : CaloLCorrection
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloLCorrectionFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloLCorrection>         s_factory ;
const        IToolFactory&CaloLCorrectionFactory = s_factory ; 
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
CaloLCorrection::CaloLCorrection
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent  )
  : CaloTool ( type, name , parent ) 
  , m_ec       ( 7.32 * MeV    ) // critical energy ( in MeV )
  , m_x0       ( 17.1 * mm     ) // radiation length  
  , m_tprs     (  2.0          ) // thickness of preshower ( in x0 units) 
  , m_tbar     (  1.7          ) // shower barycenter wrt maximum 
  , m_hypos    (               ) // list of allowed hypos
{
  // interafce 
  declareInterface<ICaloHypoTool> (this);
  // properties 
  declareProperty ( "Ec"   , m_ec   ) ;
  declareProperty ( "X0"   , m_x0   ) ;
  declareProperty ( "TPrs" , m_tprs ) ;
  declareProperty ( "Tbar" , m_tbar ) ;
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
CaloLCorrection::~CaloLCorrection() {}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloLCorrection::finalize   () 
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
StatusCode CaloLCorrection::initialize () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() ) 
    { return Error( "Could not initialize the base class CaloTool" , sc ) ; }
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
StatusCode CaloLCorrection::process    ( CaloHypo* hypo  ) const 
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
StatusCode CaloLCorrection::operator() ( CaloHypo* hypo  ) const
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
  
  const double e = hypo -> position () -> e () ;
  const double x = hypo -> position () -> x () ;
  const double y = hypo -> position () -> y () ;
  const double z = hypo -> position () -> z () ;
  
  double tanTheta = sqrt( x * x + y * y ) / z ;
  double cosTheta = cos( atan( tanTheta ) )   ;
  
  const double Tmax = log( e / m_ec ) - 0.5 ;
  const double TPrS = m_tprs / cosTheta ;
  const double Tbar = ( Tmax - TPrS + m_tbar ) * m_x0 ;
  
  tanTheta      = tanTheta / ( 1.0 + Tbar * cosTheta / z );
  cosTheta      = cos( atan( tanTheta ) ) ;
  double deltaZ = cosTheta * Tbar;
  
  // "correct" parameters 
  hypo -> position() -> setZ( z + deltaZ ) ;
  
  return StatusCode::SUCCESS ;
  
};  
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================

