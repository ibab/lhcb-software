// Standard
#include <algorithm>
#include <numeric>
#include <math.h>

//GSL 
#include "gsl/gsl_math.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/STDigit.h"
#include "Kernel/STChannelID.h"
#include "Kernel/ISTSignalToNoiseTool.h"

#include "STCommonModeSim.h"

#include "GaudiKernel/RndmGenerators.h"

#include "STDet/DeSTSector.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STCommonModeSim );

STCommonModeSim::STCommonModeSim(const std::string& name, ISvcLocator* pSvcLocator) :
  ST::AlgBase(name, pSvcLocator)
{
  // constructer

  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("outlierCut", m_outlierCut = 3.0);
  declareProperty("cmNoiseLeve",  m_commonModeNoiseLevel = 4.0);
  declareSTConfigProperty("inputLocation", m_inputLocation ,  STDigitLocation::TTDigits);
  declareSTConfigProperty("outputLocation", m_outputLocation ,  STDigitLocation::TTDigits +"Corrected");
 
}

STCommonModeSim::~STCommonModeSim() {
  //destructer
}

StatusCode STCommonModeSim::initialize() {

  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }


  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName+detType());

  // Gaussian
  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussDist.pRef());

  return StatusCode::SUCCESS;
}

StatusCode STCommonModeSim::execute() {

  m_cachedSector = 0;

  // retrieve Digitizations
  STDigits* inputCont = get<STDigits>(m_inputLocation);

  STDigits* outputCont = new STDigits();
  outputCont->reserve(inputCont->size());

  // collect all digits in a sector...
  STDigits::const_iterator iterDigit = inputCont->begin();
  while (iterDigit != inputCont->end()){

   // collect hits
   STDigits::const_iterator stop = inputCont->end();
   STDigits::const_iterator endPort = collectByPort(iterDigit, stop);    

   // apply correction
   correctForCM(iterDigit,endPort, outputCont);

   iterDigit = endPort; 
  } // iterDigit

  put(outputCont,m_outputLocation);
    
 
  return StatusCode::SUCCESS;
}

STDigits::const_iterator STCommonModeSim::collectByPort(STDigits::const_iterator& ,
							STDigits::const_iterator& end){
  return end;
}

void STCommonModeSim::correctForCM(STDigits::const_iterator& start,
				   STDigits::const_iterator& end,
                                   STDigits* outputCont){

  if (m_cachedSector->contains((*start)->channelID())){
    m_cmNoise = m_gaussDist->shoot() * m_commonModeNoiseLevel;
    m_cachedSector = findSector((*start)->channelID());
  }


  processPort(start,end,outputCont);
 
}

void STCommonModeSim::processPort(STDigits::const_iterator& start,
				  STDigits::const_iterator& end, 
                                  STDigits* outputCont){


  unsigned int nMissed = LHCbConstants::nStripsInPort - (end - start);
  double noiseCounts = m_sigNoiseTool->noiseInADC((*start)->channelID());
  double ranNoise = (nMissed* m_cmNoise) + m_gaussDist->shoot()*noiseCounts *sqrt(nMissed);

  // make tmp list of charges
  std::vector<double> startCharge;
  for (STDigits::const_iterator iterDigit = start; iterDigit != end; ++iterDigit){
    double tCharge = rescale((*iterDigit)->depositedCharge() + m_cmNoise);
    startCharge.push_back(tCharge);
  }
 
  double mean = std::accumulate(startCharge.begin(), startCharge.end(), 0.) + ranNoise;

  // remove outliers....
  std::vector<double> baseLine;
  for (std::vector<double>::iterator iter = startCharge.begin(); 
       iter != startCharge.end(); ++iter){
    double correctedValue = *iter - mean;
    if ( correctedValue/noiseCounts > m_outlierCut) startCharge.push_back(correctedValue);
  }

  // cm correction
  double cmCorrection = m_cmNoise - std::accumulate(baseLine.begin(), baseLine.end(), 0.);

  // feedback correction to the input values...
  for (STDigits::const_iterator iterDigit2 = start; iterDigit2 != end; ++iterDigit2){
    STDigit* aDigit = new STDigit(GSL_MIN(cmCorrection+(*iterDigit2)->depositedCharge(),127));
    outputCont->insert(aDigit, (*iterDigit2)->channelID());
  }

}

double STCommonModeSim::rescale(const double value) const{
  double tVal = GSL_MIN(255,value + m_pedestal);
  return GSL_MAX(0., tVal) - m_pedestal;
}











