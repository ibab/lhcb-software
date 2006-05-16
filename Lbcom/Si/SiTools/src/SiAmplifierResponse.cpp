// $Id: SiAmplifierResponse.cpp,v 1.1.1.1 2006-05-16 08:33:54 mneedham Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "SiAmplifierResponse.h"

#include "Kernel/SystemOfUnits.h"


static const ToolFactory<SiAmplifierResponse>  s_factory;
const IToolFactory& SiAmplifierResponseFactory = s_factory;

/// constructer
SiAmplifierResponse::SiAmplifierResponse(const std::string& type, 
                                         const std::string& name, 
                                        const IInterface* parent): 
  GaudiTool( type, name, parent ),
  m_responseSpline(0)
{
  declareProperty("times",m_times);
  declareProperty("values",m_values);

  declareProperty("splineType", m_splineType = "Cspline");

  declareProperty("type", m_type = "signal");
  declareProperty("vfs", m_vfs  = 400);
  declareProperty("capacitance",m_capacitance  = 18 * picofarad);

  
  // need a line here to get the interface correct !!!!
  declareInterface<ISiAmplifierResponse>(this); 
}

/// initialize
SiAmplifierResponse::~SiAmplifierResponse() {
  /// destructer
 if (0 != m_responseSpline) delete m_responseSpline;
}

/// initialize
StatusCode SiAmplifierResponse::initialize(){

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

  m_responseSpline = new GaudiMath::SimpleSpline(m_times,m_values,typeFromString());
  m_info.capacitance = m_capacitance;
  m_info.vfs = m_vfs;
  m_info.type = m_type;

  return StatusCode::SUCCESS;
}

/// calc amplifier response
double SiAmplifierResponse::response(const double time) const{
  return ((time>m_tMin)&&(time<m_tMax)  ?  m_responseSpline->eval(time) : 0.0);
}

/// information on the validity
ISiAmplifierResponse::Info SiAmplifierResponse::validity() const{
  return m_info;
}

GaudiMath::Interpolation::Type SiAmplifierResponse::typeFromString() const{
  
  GaudiMath::Interpolation::Type aType;
  if (m_splineType == "Cspline"){
     aType = GaudiMath::Interpolation::Cspline;
  }
  else if (m_splineType == "Linear"){
    aType = GaudiMath::Interpolation::Linear;
  }
  else if (m_splineType == "Polynomial"){   
    aType = GaudiMath::Interpolation::Polynomial;
  }
  else if (m_splineType == "Akima"){
    aType = GaudiMath::Interpolation::Akima;
  }
  else if (m_splineType == "Akima_Periodic"){
    aType = GaudiMath::Interpolation::Akima_Periodic;
  }
  else if (m_splineType == "Cspline_Periodic"){
    aType = GaudiMath::Interpolation::Cspline_Periodic;
  }
  else {
    // default to cubic
    aType = GaudiMath::Interpolation::Cspline;
  }

  return aType;
}






