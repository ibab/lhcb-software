// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloKernel/CaloTool.h"

#include "math.h"

#include "CaloSCorrectionArcSinh.h"

// ============================================================================
/** @file CaloSCorrectionArcSinh.cpp
 *
 *  Implementation file for class : CaloSCorrectionArcSinh
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   20/06/2002
 */
// ============================================================================

// ============================================================================
/** @var CaloSCorrectionArcSinhFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSCorrectionArcSinh>         s_factory ;
const        IToolFactory&CaloSCorrectionArcSinhFactory = s_factory ;
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
CaloSCorrectionArcSinh::CaloSCorrectionArcSinh(const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent) 
  : CaloTool( type, name, parent ) 
  , m_Coeff() {
  declareInterface<ICaloSCorrectionFunction>(this);
  declareProperty("Coeff",m_Coeff);
}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloSCorrectionArcSinh::~CaloSCorrectionArcSinh() {}
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrectionArcSinh::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "intialize() has been called" << endreq;
  if (1!=m_Coeff.size()) {
    log << MSG::INFO << "number of coeff=" << m_Coeff.size() 
        << "!!!" << endreq;
    return StatusCode::FAILURE;
  }
  uint i=0;
  for (i=0;i<m_Coeff.size();i++) {
    log << MSG::INFO << "coeff[" << i << "]=" << m_Coeff[i] << endreq;
  }
  /// initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) {
    Error("Could not initialize the base class ",sc);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrectionArcSinh::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "finalize() has been called" << endreq;
  /// finalize the  the base class
  return CaloTool::finalize ();
}
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloSCorrectionFunction
 *  @param  energies to be processed
 *  @param  seed position as option
 *  @param  result value
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloSCorrectionArcSinh::calculate(double min, 
                                             double middle,
                                             double max,
                                             double e,
                                             double x,
                                             double y,
                                             double& result) {
  double bary = (max-min)/(min+middle+max);
  result = m_Coeff[0]*asinh(2.*bary*sinh(1./m_Coeff[0]))/2.;
  MsgStream log(msgSvc(),name());
  log << MSG::VERBOSE << "calculate() has been called"
      << " arg:" << " min:" << min 
      << " middle:" << middle
      << " max:" << max
      << " e:" << e
      << " x:" << x
      << " y:" << y
      << endreq;
  log << MSG::VERBOSE << "bary=" << bary << endreq;
  log << MSG::VERBOSE << "result=" << result << endreq;
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** The main error processing method
 *  @see ICaloSCorrectionFunction
 *  @param  energies to be processed
 *  @param  seed position as option
 *  @param  result value
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloSCorrectionArcSinh::calculateprime(double min, 
                                                  double middle,
                                                  double max,
                                                  double e,
                                                  double x,
                                                  double y,
                                                  double& result) {
  result = 0.;
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "calculateprime() has been called"
      << " arg:" << " min:" << min 
      << " middle:" << middle
      << " max:" << max
      << " e:" << e
      << " x:" << x
      << " y:" << y
      << endreq;
  return StatusCode::SUCCESS;
}
