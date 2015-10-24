// Standard
#include "LHCbMath/LHCbMath.h"
#include <algorithm>
#include <numeric>
#include <math.h>

//GSL 
#include "gsl/gsl_math.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "LHCbMath/LHCbMath.h"


#include "Event/STDigit.h"
#include "Kernel/STChannelID.h"


#include "STCommonModeSim.h"

#include "GaudiKernel/RndmGenerators.h"

#include "STDet/DeSTSector.h"

#include "ISTCMSimTool.h"
#include "ISTPedestalSimTool.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STCommonModeSim )

STCommonModeSim::STCommonModeSim(const std::string& name, ISvcLocator* pSvcLocator) :
  ST::AlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("forceOptions", m_forceOptions = false);
  declareProperty("outlierCut", m_outlierCut = 5.0);
  declareSTConfigProperty( "conditionLocation", 
			   m_conditionLocation  , "/dd/Conditions/ReadoutConf/TT/ClusteringThresholds" );


  declareProperty("pedestalToolName", m_pedestalToolName = "STPedestalSimTool" );
  declareProperty("cmToolName", m_cmToolName = "STCMSimTool" );
  declareSTConfigProperty("inputLocation", m_inputLocation ,  STDigitLocation::TTDigits);
  declareSTConfigProperty("outputLocation", m_outputLocation ,  STDigitLocation::TTDigits +"CMCorrected");
 
}

STCommonModeSim::~STCommonModeSim() {
  //destructer
}

StatusCode STCommonModeSim::initialize() {

  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_pedestalTool = tool<ISTPedestalSimTool>( m_pedestalToolName,  m_pedestalToolName,  this);
  m_cmTool = tool<ISTCMSimTool>( m_cmToolName,  m_cmToolName,  this);

  // Gaussian
  m_gaussDist = randSvc()->generator(Rndm::Gauss(0.,1.));

  // get the cuts from condition if present
  if (existDet<Condition>(m_conditionLocation) == false || 
      m_forceOptions == true ){
    info() <<  "Default to jobOptions for cuts" <<endmsg;
  }
  else {
    registerCondition(m_conditionLocation,
                    &STCommonModeSim::loadCutsFromConditions );
    sc = runUpdate();
    if (sc.isFailure()) return Error("Failed to update conditions", sc);
  }

  return StatusCode::SUCCESS;
}

StatusCode STCommonModeSim::execute() {


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
   processPort(iterDigit,endPort, outputCont);

   iterDigit = endPort; 
  } // iterDigit

  put(outputCont,m_outputLocation);
    
 
  return StatusCode::SUCCESS;
}

STDigits::const_iterator STCommonModeSim::collectByPort(STDigits::const_iterator& start,
							STDigits::const_iterator& end){

  STDigits::const_iterator iter = start; ++iter;
  const STChannelID firstChan = (*start)->channelID() ;
  const unsigned int port = firstChan.strip() / LHCbConstants::nStripsInPort;
  const unsigned int sector = firstChan.uniqueSector();
  bool samePort = true;
  while (iter != end && samePort == true){
    if ((*iter)->channelID().uniqueSector() != sector ) {
      samePort = false;
    } 
    else {
      const unsigned int testport = (*iter)->channelID().strip() / LHCbConstants::nStripsInPort;
      if ( port != testport) samePort = false; 
   }
  } // sector

  return iter;
}

void STCommonModeSim::processPort(STDigits::const_iterator& start,
				  STDigits::const_iterator& end, 
                                  STDigits* outputCont){

  const DeSTSector* sector = findSector((*start)->channelID());
  const double noiseCounts = sector->sectorNoise();
  const double cmNoise = m_cmTool->noise((*start)->channelID());
  unsigned int nMissed = LHCbConstants::nStripsInPort - (end - start);
  double ranNoise = cmNoise + m_gaussDist->shoot()*noiseCounts / sqrt((double)nMissed);

  // make list of pedestal corrected adc scaled to 7 bit
  typedef std::pair<LHCb::STChannelID,double> digitCharge; 
  std::vector<digitCharge> startCharge; 
  std::vector<double> portVec(LHCbConstants::nStripsInPort,  ranNoise);
  std::vector<double> threshold(LHCbConstants::nStripsInPort,0); 
  for (STDigits::const_iterator iterDigit = start; iterDigit != end; ++iterDigit){
    int pedestal = LHCb::Math::round(m_pedestalTool->pedestal((*iterDigit)->channelID()));
    double tCharge = GSL_MIN((*iterDigit)->channelID() - pedestal , 127);
    startCharge.push_back(std::make_pair((*iterDigit)->channelID(), tCharge));
    unsigned int pos = (*iterDigit)->channelID().strip() -1 %  LHCbConstants::nStripsInPort  ;
    portVec[pos] = tCharge;
    threshold[pos] = m_outlierCut*sector->noise((*iterDigit)->channelID());
  }
 
  // first mean
  double mean = std::accumulate(portVec.begin(), portVec.end(), 0.)/LHCbConstants::nStripsInPort ;

  // mask outliers
  std::vector<unsigned int> mask(LHCbConstants::nStripsInPort,0);
  for (unsigned int i2 = 0; i2 < LHCbConstants::nStripsInPort; ++i2){
    double testVal = portVec[i2] - mean;
    if (testVal > threshold[i2]){
      mask[i2] = 1;
      if (i2 != 0) mask[i2-1] = 1;
      if (i2 != LHCbConstants::nStripsInPort ) mask[i2+1] = 1;
    }
  } // i

  // second mean
  double tCharge = 0.0;
  for (unsigned int i = 0; i < LHCbConstants::nStripsInPort; ++i){
    if (mask[i] != 1) tCharge = portVec[i]; 
  }
  mean = tCharge / LHCbConstants::nStripsInPort ;

  // feedback correction to the input values...
  for (std::vector<digitCharge>::const_iterator iter = startCharge.begin(); iter != startCharge.end(); ++iter){
    STDigit* aDigit = new STDigit(GSL_MIN(LHCb::Math::round(iter->second-mean),127));
    outputCont->insert(aDigit, iter->first);
  }

}

StatusCode STCommonModeSim::loadCutsFromConditions(){


  // load conditions
  Condition* cInfo = getDet<Condition>(m_conditionLocation);
  info() << "Loading cuts tagged as " << cInfo->param<std::string>("tag") << endmsg;

  m_outlierCut = cInfo->param<double>("CMThreshold");

  return StatusCode::SUCCESS;
}







