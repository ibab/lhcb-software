// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "SiAmplifierResponse.h"

DECLARE_TOOL_FACTORY( SiAmplifierResponse )

SiAmplifierResponse::SiAmplifierResponse( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent ): 
  SiAmplifierResponseBase( type, name, parent )
{
  // constructer
  declareProperty("times",       m_times                                     );
  declareProperty("values",      m_values                                    );  
}

SiAmplifierResponse::~SiAmplifierResponse()
{
  // destructer
}

StatusCode SiAmplifierResponse::initialize()
{
  StatusCode sc = SiAmplifierResponseBase::initialize();
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
  // dump to screen 
  if (m_printToScreen == true) {
    printToScreen();
  }
  if (m_printForRoot == true) {
    printForRoot();
  }

  return sc;
}

