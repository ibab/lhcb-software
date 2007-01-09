// $Id: SiAmplifierResponse.cpp,v 1.3 2007-01-09 14:57:21 jvantilb Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "SiAmplifierResponse.h"

DECLARE_TOOL_FACTORY( SiAmplifierResponse );

SiAmplifierResponse::SiAmplifierResponse( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent ): 
  GaudiTool( type, name, parent ),
  m_responseSpline(0)
{
  // constructer
  declareProperty("times",       m_times                                     );
  declareProperty("values",      m_values                                    );
  declareProperty("splineType",  m_splineType  = "Cspline"                   );
  declareProperty("type",        m_type        = "signal"                    );
  declareProperty("vfs",         m_vfs         = 400                         );
  declareProperty("capacitance", m_capacitance = 18 * Gaudi::Units::picofarad);
  
  // need a line here to get the interface
  declareInterface<ISiAmplifierResponse>(this); 
}

SiAmplifierResponse::~SiAmplifierResponse()
{
  // destructer
  if (0 != m_responseSpline) delete m_responseSpline;
}

StatusCode SiAmplifierResponse::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Check if the data is provided and the times and values are of equal length
  if (m_times.size() == 0) return Error("No data !", StatusCode::FAILURE);
  if (m_times.size() != m_values.size()) {
    return Error("inconsistant data !", StatusCode::FAILURE);
  }

  // Store the first and last entry of the vector of times
  m_tMin = m_times.front();
  m_tMax = m_times.back();

  // Fit the spline to the data
  m_responseSpline = new GaudiMath::SimpleSpline( m_times, m_values,
                                                  typeFromString() );

  // Fill the ISiAmplifierResponse::Info object. Used to find out for which 
  // parameters the spline curve is valid.
  m_info.capacitance = m_capacitance;
  m_info.vfs = m_vfs;
  m_info.type = m_type;

  return sc;
}

/// calculate amplifier response
double SiAmplifierResponse::response(const double time) const
{
  return ((time>m_tMin)&&(time<m_tMax) ? m_responseSpline->eval(time) : 0.0);
}

/// information on the validity
ISiAmplifierResponse::Info SiAmplifierResponse::validity() const
{
  return m_info;
}

GaudiMath::Interpolation::Type SiAmplifierResponse::typeFromString() const
{  
  GaudiMath::Interpolation::Type aType;
  using namespace GaudiMath::Interpolation;
  if      (m_splineType == "Cspline")          aType = Cspline;
  else if (m_splineType == "Linear")           aType = Linear;
  else if (m_splineType == "Polynomial")       aType = Polynomial;
  else if (m_splineType == "Akima")            aType = Akima;
  else if (m_splineType == "Akima_Periodic")   aType = Akima_Periodic;
  else if (m_splineType == "Cspline_Periodic") aType = Cspline_Periodic;
  else aType = Cspline; // default to cubic

  return aType;
}
