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
  , m_Ec(7.3)
  , m_X0(17.1)
  , m_Zecal(12490.)
  , m_LogCste(-0.8)
  , m_Coeff() {
  declareInterface<ICaloHypoTool>(this);
  declareProperty("Ec",m_Ec);
  declareProperty("X0",m_X0);
  declareProperty("Zecal",m_Zecal);
  declareProperty("LogCste",m_LogCste);
  declareProperty("Coeff",m_Coeff);
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
  MsgStream logmsg(msgSvc(), name());
  logmsg << MSG::VERBOSE << "intialize() has been called" << endreq;
  /// initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) {
    Error("Could not initialize the base class ",sc);
    return StatusCode::FAILURE;
  }
  if (2!=m_Coeff.size()) {
    logmsg << MSG::INFO << "number of coeff=" << m_Coeff.size() 
           << "!!!" << endreq;
    return StatusCode::FAILURE;
  }
  unsigned int i=0;
  for (i=0;i<m_Coeff.size();i++) {
    logmsg << MSG::INFO << "coeff[" << i << "]=" << m_Coeff[i] << endreq;
  }
  logmsg << MSG::INFO << "Ec=" << m_Ec 
         << " X0=" << m_X0
         << " Zecal=" << m_Zecal
         << " LogCste=" << m_LogCste << endreq;

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
  MsgStream logmsg(msgSvc(), name());
  logmsg << MSG::VERBOSE << "finalize() has been called" << endreq;
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
  const double e = hypo -> position () -> e () ;
  const double x = hypo -> position () -> x () ;
  const double y = hypo -> position () -> y () ;
  // compute trigonometric things...
  double theta=atan(sqrt(x*x+y*y)/m_Zecal);
  double result = m_Zecal+
    ((log(e/m_Ec)+m_LogCste+m_Coeff[0])*m_X0*m_Coeff[1])
    *cos(theta);
  // talk to user
  MsgStream logmsg(msgSvc(),name());
  logmsg << MSG::VERBOSE << "calculate() has been called"
         << " arg:" << " e:" << e
         << " x:" << x << " y:" << y << endreq;
  logmsg  << MSG::VERBOSE 
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

