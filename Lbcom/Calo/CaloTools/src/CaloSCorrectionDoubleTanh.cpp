// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloKernel/CaloTool.h"

#include "math.h"

#include "CaloSCorrectionDoubleTanh.h"

// ============================================================================
/** @file CaloSCorrectionDoubleTanh.cpp
 *
 *  Implementation file for class : CaloSCorrectionDoubleTanh
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   20/06/2002
 */
// ============================================================================

// ============================================================================
/** @var CaloSCorrectionDoubleTanhFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSCorrectionDoubleTanh>         s_factory ;
const        IToolFactory&CaloSCorrectionDoubleTanhFactory = s_factory ;
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
CaloSCorrectionDoubleTanh::CaloSCorrectionDoubleTanh(const std::string& type, 
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
CaloSCorrectionDoubleTanh::~CaloSCorrectionDoubleTanh() {}
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrectionDoubleTanh::initialize()
{
 /// initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) {
    Error("Could not initialize the base class ",sc);
    return StatusCode::FAILURE;
  }
   MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "intialize() has been called" << endreq;
  if (3!=m_Coeff.size()) {
    log << MSG::INFO << "number of coeff=" << m_Coeff.size() 
        << "!!!" << endreq;
    return StatusCode::FAILURE;
  }
  unsigned int i=0;
  for (i=0;i<m_Coeff.size();i++) {
    log << MSG::INFO << "coeff[" << i << "]=" << m_Coeff[i] << endreq;
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
StatusCode CaloSCorrectionDoubleTanh::finalize()
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
StatusCode CaloSCorrectionDoubleTanh::calculate(double min, 
                                                double middle,
                                                double max,
                                                double e,
                                                double x,
                                                double y,
                                                double& result) {
  double bary = (max-min)/(min+middle+max);
  result = m_Coeff[0]*tanh(m_Coeff[1]*bary)
    +(1.-m_Coeff[0])*tanh(m_Coeff[2]*bary);
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "calculate() has been called"
      << " arg:" << " min:" << min 
      << " middle:" << middle
      << " max:" << max
      << " e:" << e
      << " x:" << x
      << " y:" << y
      << endreq;
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
StatusCode CaloSCorrectionDoubleTanh::calculateprime(double min, 
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

