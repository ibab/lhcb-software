// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

// std
#include <algorithm>

// local
#include "SiPMResponse.h"

using namespace boost;

//-----------------------------------------------------------------------------
// Implementation file for class : SiPMResponse
//
//   This class describes the SiPM response to a single pulse
//
// 2013-11-12 : Maurizio Martinelli
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SiPMResponse )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SiPMResponse::SiPMResponse( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<SiPMResponse>(this);
  // properties
  declareProperty("samplingDt",  m_samplingDt, "Sampling time step" );
  declareProperty("times",  m_times, "Times for user-defined function" );
  declareProperty("values", m_values, "Values for user-defined function" );
  declareProperty("splineType", m_splineType = "Cspline", "The spline type" );
}

//=============================================================================
// Destructor
//=============================================================================
SiPMResponse::~SiPMResponse() 
{
  if (0 != m_responseSpline) delete m_responseSpline;
} 
//=============================================================================
// Initialize the pulse shape
//=============================================================================
StatusCode 
SiPMResponse::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  
  // If times and values are empty set default
  if (m_times.size() == 0 && m_values.size() == 0) {
    // prev spill
    std::vector<double> Tprev =  
      boost::assign::list_of
      (3.22188688e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
      (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
      (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
      (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
      (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05);
    // current spill
    std::vector<double> Tcurr =  
      boost::assign::list_of
      (2.88048286e-02)(2.76166647e-02)(2.84853231e-02)(2.86509678e-02)(2.87964850e-02)
      (2.90245415e-02)(2.92078012e-02)(2.93951976e-02)(2.95711670e-02)(2.96709382e-02)
      (2.97279647e-02)(2.96909134e-02)(2.94095934e-02)(2.89146629e-02)(2.80610778e-02)
      (2.67112946e-02)(2.47312816e-02)(2.20768793e-02)(1.86479251e-02)(1.44897527e-02)
      (9.82891757e-03)(5.23039367e-03)(1.56929340e-03)(-2.17283810e-04)(-1.82921036e-05)(6.66654783e-05);
    // next spill
    std::vector<double> Tnext =  
      boost::assign::list_of
      (-0.00136548)(-0.00089388)(-0.0012617)(-0.00137867)(-0.00145512)
      (-0.00160822)(-0.00173438)(-0.00187304)(-0.00201678)(-0.00213902)
      (-0.0022567)(-0.00234672)(-0.0023399)(-0.00226133)(-0.00202669)
      (-0.00154593)(-0.00067318)(0.00060853)(0.00250173)(0.00515046)
      (0.00876429)(0.01327495)(0.01883291)(0.02372474)(0.02788753)(0.0281931);
      // nextnext spill
    std::vector<double> Tnext2 =  
      boost::assign::list_of
      (-0.00034101)(-0.00025578)(-0.00030324)(-0.00031672)(-0.00032477)
      (-0.00034131)( -0.00035811)(-0.00037248)(-0.00038796)(-0.00040745)
      (-0.00042273)(-0.00044454)(-0.00046555)(-0.00048982)(-0.00051403)
      (-0.00054482)(-0.00056937)(-0.00060258)(-0.00064924)(-0.00068913)
      (-0.00070694)(-0.00073142)(-0.00099868)(-0.00083826)(-0.00119224)(-0.00104176);
    // reverse the vectors
    std::reverse(Tprev.begin(), Tprev.end());
    std::reverse(Tcurr.begin(), Tcurr.end());
    std::reverse(Tnext.begin(), Tnext.end());
    std::reverse(Tnext2.begin(), Tnext2.end());
    // set times
    for (int i = -25; i < 76; ++i) m_times.push_back(i);
    // set values
    for (int i = 0; i < 26; ++i) m_values.push_back(Tprev[i]-Tprev[0]);
    for (int i = 1; i < 26; ++i) m_values.push_back(Tcurr[i]-Tcurr[0]+m_values[25]);
    for (int i = 1; i < 26; ++i) m_values.push_back(Tnext[i]-Tnext[0]+m_values[50]);
    for (int i = 1; i < 26; ++i) m_values.push_back(Tnext2[i]-Tnext2[0]+m_values[75]);
  }

  // Check if the data is provided and the times and values are of equal length
  if (m_times.size() == 0) return Error("No data !", StatusCode::FAILURE);
  if (m_times.size() != m_values.size()) {
    return Error("inconsistant data !", StatusCode::FAILURE);
  }
  
  // Store the first and last entry of the vector of times
  m_tMin = m_times.front();
  m_tMax = m_times.back();

  // Set maximum to 1
  double maxVal(-99999.);
  for (std::vector<double>::iterator it = m_values.begin(); 
       it != m_values.end(); ++it)
    if (*it > maxVal) maxVal = *it;

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "SiPMResponse before reweighting:"<<endmsg;
    debug() << "----------------------------" <<endmsg;
    std::vector<double> vals(m_values);
    for (unsigned int i =0; i < m_values.size(); ++i)
      debug() << "\t t = " << m_times[i] << "\t val = " << vals[i] << " / " << maxVal << " = " << vals[i] /  maxVal << endmsg;
  }

  for (unsigned int i =0; i < m_values.size(); ++i)
    m_values[i] = m_values[i] / maxVal;

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "SiPMResponse max = " << maxVal << endmsg;
    debug() << "SiPMResponse after reweighting:" <<endmsg;
    debug() << "----------------------------" <<endmsg;
    for (unsigned int i =0; i < m_values.size(); ++i)
      debug() << "\t t = " << m_times[i] << "\t val = " << m_values[i] << endmsg;
    
  }
  
  // Fit the spline to the data
  m_responseSpline = new GaudiMath::SimpleSpline( m_times, m_values,
						  typeFromString() );
  /*
  // dump to screen 
  if (m_printToScreen == true) {
    printToScreen();
  }
  if (m_printForRoot == true) {
    printForRoot();
  }
  */
  
  return sc;
}

double 
SiPMResponse::response(const double time) const
{
  return ((time>m_tMin)&&(time<m_tMax) ? m_responseSpline->eval(time) : 0.0);
}

GaudiMath::Interpolation::Type 
SiPMResponse::typeFromString() const
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

void 
SiPMResponse::sample(std::vector<double>& times, std::vector<double>& values) const
{
  double t = m_tMin;
  while(t < m_tMax) {
  times.push_back(t);
  values.push_back(response(t));
  t += m_samplingDt; 
  } // loop times
}

//=============================================================================
