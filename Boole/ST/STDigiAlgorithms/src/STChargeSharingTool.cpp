// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// GSL
#include "gsl/gsl_sf_erf.h"

// Boost
#include <boost/assign/list_of.hpp>

// local
#include "STChargeSharingTool.h"

using namespace GaudiMath;
using namespace GaudiMath::Interpolation;
using namespace Gaudi::Units;

DECLARE_TOOL_FACTORY( STChargeSharingTool )

STChargeSharingTool::STChargeSharingTool( const std::string& type, 
                                          const std::string& name,
                                          const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_responseSpline(0)
{
  // constructer
  std::vector<double> tmp =
    boost::assign::list_of(1.0)(1.0)(0.97)(0.935)(0.90)(0.83)
    (0.73)(0.5)(0.44)(0.34)(0.20)(0.11)(0.065)(0.038)(0.016)
    (0.0)(0.0);
  declareProperty("SharingFunction", m_sharingFunction = tmp);
  declareProperty("UseAnalyticErf",  m_useAnalyticErf = true    );
  declareProperty("ErfWidth",        m_erfWidth       = 0.0224   );
  declareProperty("Thickness",       m_thickness      = 0.500*mm );
  declareInterface<ISTChargeSharingTool>(this);
}

STChargeSharingTool::~STChargeSharingTool()
{
  // destructer
  if (0 != m_responseSpline ) delete m_responseSpline;
}

StatusCode STChargeSharingTool::initialize()
{
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  if( !m_useAnalyticErf ) {
    unsigned int nBin = m_sharingFunction.size();
    if( nBin == 0 ) {
      return Error("no charge sharing data !" + name(), StatusCode::FAILURE);
    }
    
    // bin centers
    std::vector<double> binCenters;
    binCenters.reserve(nBin);  
    double binSize = 1.0/(double)nBin;
    for (unsigned int iBin = 0; iBin<nBin; ++iBin) {
      double binCenter = (0.5+(double)iBin)*binSize;
      binCenters.push_back(binCenter);
    } //iBin

    // Fit the spline.
    m_responseSpline = new SimpleSpline(binCenters, m_sharingFunction, Linear );
  }
  
  return StatusCode::SUCCESS;
}

double STChargeSharingTool::sharing(const double relDist) const
{
  if( relDist < 0.0 || relDist > 1.0 ) return 0.0;
  return (!m_useAnalyticErf) ? m_responseSpline->eval(relDist) :
    0.5*gsl_sf_erfc(sqrt(0.5)*(relDist-0.5)/m_erfWidth) ;
}

double STChargeSharingTool::thickness() const
{
  return m_thickness;
}
