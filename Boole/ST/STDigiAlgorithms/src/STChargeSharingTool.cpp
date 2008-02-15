// $Id: STChargeSharingTool.cpp,v 1.1 2008-02-15 14:49:05 cattanem Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "STChargeSharingTool.h"

using namespace GaudiMath;
using namespace GaudiMath::Interpolation;

DECLARE_TOOL_FACTORY( STChargeSharingTool );

STChargeSharingTool::STChargeSharingTool( const std::string& type, 
                                          const std::string& name,
                                          const IInterface* parent ) :
  GaudiTool(type, name, parent),
  m_responseSpline(0)
{
  // constructer
  declareProperty("SharingFunction", m_sharingFunction );
  declareInterface<ISTChargeSharingTool>(this);
}

STChargeSharingTool::~STChargeSharingTool()
{
  // destructer
  if (0 != m_responseSpline ) delete m_responseSpline;
}

StatusCode STChargeSharingTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  unsigned int nBin = m_sharingFunction.size();
  if (nBin == 0){
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
  m_responseSpline = new SimpleSpline( binCenters, m_sharingFunction, Linear );

  return StatusCode::SUCCESS;
}

double STChargeSharingTool::sharing(const double relDist) const
{
  return ((relDist>0.) && (relDist<1.0) ? m_responseSpline->eval(relDist) : 0);
}
