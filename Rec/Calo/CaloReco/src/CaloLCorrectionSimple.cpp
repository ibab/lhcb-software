// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"

#include "CaloKernel/CaloTool.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// Event
#include "Event/CaloHypo.h"
// local
#include "CaloLCorrectionSimple.h"

// ============================================================================
/** @file CaloLCorrectionSimple.cpp
 *
 *  Implementation file for class : CaloLCorrectionSimple
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   20/06/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloLCorrectionSimpleFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloLCorrectionSimple>         s_factory ;
const        IToolFactory&CaloLCorrectionSimpleFactory = s_factory ;
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
CaloLCorrectionSimple::CaloLCorrectionSimple(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : CaloTool( type, name, parent )
  , m_Zref(12480.)
  , m_Zss(171.1)
  , m_Zfactor(9.72) {
  declareInterface<ICaloHypoTool>(this);
  declareProperty("Zref",m_Zref);
  declareProperty("Zss",m_Zss);
  declareProperty("Zfactor",m_Zfactor);
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloLCorrectionSimple::~CaloLCorrectionSimple() {}
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode CaloLCorrectionSimple::initialize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "intialize() has been called" << endreq;
  /// initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) {
    Error("Could not initialize the base class ",sc);
    return StatusCode::FAILURE;
  }
  msg << MSG::INFO
      << " Zref=" << m_Zref
      << " Zss=" << m_Zss
      << " Zfactor=" << m_Zfactor
      << endreq;

  return StatusCode::SUCCESS;
}

// ============================================================================
/** standard finalization method
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode CaloLCorrectionSimple::finalize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "finalize() has been called" << endreq;
  /// finalize the  the base class
  return CaloTool::finalize ();
}
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code
 */
// ============================================================================
StatusCode CaloLCorrectionSimple::process    ( CaloHypo* hypo  ) const
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
StatusCode CaloLCorrectionSimple::operator() ( CaloHypo* hypo  ) const
{
  // check arguments
  if( 0 == hypo ) { return Error("CaloHypo* points to NULL");}
  // check the position
  if( 0 == hypo->position() ) { return Error("CaloPosition* points to NULL!");}
  // retrieve energy and position
  const double e = hypo -> position () -> e () /1000. ;
  if (e<=0.) {return StatusCode::FAILURE;}
  const double x = hypo -> position () -> x () ;
  if (fabs(x)<=0.) {return StatusCode::FAILURE;}
  const double y = hypo -> position () -> y () ;
  if (fabs(y)<=0.) {return StatusCode::FAILURE;}
  // compute trigonometric things...
  //double result = m_Zref+cos(atan(sqrt(x*x+y*y)/(12490.)))*(m_Zss+m_Zfactor*log(e));
  double result = m_Zref+12490./sqrt(x*x+y*y+12490.*12490.)*(m_Zss+m_Zfactor*log(e));
  // talk to user
  MsgStream msg(msgSvc(),name());
  msg << MSG::VERBOSE << "calculate() has been called"
         << " arg:" << " e:" << e
         << " x:" << x << " y:" << y << endreq;
  msg  << MSG::VERBOSE
          << " result:" << result
          << endreq;
  // set "correct" parameters
  hypo -> position() -> setZ( result );
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

