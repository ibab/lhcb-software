// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IService.h"

// ITAlgorithms
#include "STChargeSharingTool.h"
#include "GSLSpline.h"


static ToolFactory<STChargeSharingTool> s_factory;
const IToolFactory& STChargeSharingToolFactory = s_factory;

STChargeSharingTool::STChargeSharingTool(const std::string& type, const std::string& name, const IInterface* parent) :
 GaudiTool(type, name, parent),
 m_responseSpline(0)
{
  // constructer
  declareProperty("sharingFunction",m_sharingFunction);
 declareInterface<ISTChargeSharingTool>(this);
}

STChargeSharingTool::~STChargeSharingTool() {
  //destructer
  if (0 != m_responseSpline ) delete  m_responseSpline;
}

StatusCode STChargeSharingTool::initialize(){

  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }


  unsigned int nBin = m_sharingFunction.size();
  if (nBin == 0){
    return Error("no charge sharing data !" + name(), StatusCode::FAILURE);
  } 

  // bin centers
  m_binCenters.reserve(nBin);  
  double binSize = 1.0/(double)nBin;
  for (unsigned int iBin = 0; iBin<nBin; ++iBin){
    double binCenter = (0.5+(double)iBin)*binSize;
    m_binCenters.push_back(binCenter);
  } //iBin

  // the spline...
  m_responseSpline = new GSLSpline(m_binCenters,m_sharingFunction,gsl_interp_linear);

  return StatusCode::SUCCESS;
}

double STChargeSharingTool::sharing(const double relDist) const{
  return  ((relDist > 0.) && (relDist<1.0)   ? m_responseSpline->value(relDist) : 0  );
}







