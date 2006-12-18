// $Id: STAmplifierResponse.cpp,v 1.4 2006-12-18 10:09:54 cattanem Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "STAmplifierResponse.h"

#include "GSLSpline.h"

#include "GaudiKernel/SystemOfUnits.h"

DECLARE_TOOL_FACTORY( STAmplifierResponse );

/// constructer
STAmplifierResponse::STAmplifierResponse(const std::string& type, 
                                         const std::string& name, 
                                        const IInterface* parent): 
  GaudiTool( type, name, parent ),
  m_responseSpline(0)
{
  declareProperty("times",m_times);
  declareProperty("values",m_values);

  declareProperty("type", m_type = "signal");
  declareProperty("vfs", m_vfs  = 400);
  declareProperty("capacitance",m_capacitance  = 18 * Gaudi::Units::picofarad);
  
  // need a line here to get the interface correct !!!!
  declareInterface<ISTAmplifierResponse>(this); 
}

/// initialize
STAmplifierResponse::~STAmplifierResponse() {
  /// destructer
 if (0 != m_responseSpline) delete m_responseSpline;
}

/// initialize
StatusCode STAmplifierResponse::initialize(){

  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  if (m_times.size() == 0){
    return Error(" no data !", StatusCode::FAILURE);
  }

  if (m_times.size() != m_values.size()){
    return Error("inconsistant data !", StatusCode::FAILURE);
  }
                                                                              
  m_tMin = m_times.front();
  m_tMax = m_times.back();
  m_responseSpline = new GSLSpline(m_times,m_values);
  
  m_info.capacitance = m_capacitance;
  m_info.vfs = m_vfs;
  m_info.type = m_type;

  return StatusCode::SUCCESS;
}

/// calc amplifier response
double STAmplifierResponse::response(const double time) const{
  return ((time>m_tMin)&&(time<m_tMax)  ?  m_responseSpline->value(time) : 0.0);
}

/// information on the validity
ISTAmplifierResponse::Info STAmplifierResponse::validity() const{
  return m_info;
}









