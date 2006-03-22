// $Id: STDigitCreator.cpp,v 1.11 2006-03-22 13:56:47 mneedham Exp $

#include "gsl/gsl_math.h"

#include "GaudiKernel/AlgFactory.h"
#
//Random Numbers
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Kernel
#include "Kernel/SystemOfUnits.h"

// Event
#include "Event/MCTruth.h"
#include "Event/MCSTDeposit.h"

// GSL
#include "gsl/gsl_sf_erf.h"

// STAlgorithms
#include "ISTEffCalculator.h"
#include "Kernel/ISTSignalToNoiseTool.h"
#include "Kernel/STDataFunctor.h"
#include "STDigitCreator.h"
#include "Kernel/STDetSwitch.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include <algorithm>

#include "Kernel/LHCbMath.h"

using namespace LHCb;


static const AlgFactory<STDigitCreator>  s_factory;
const IAlgFactory& STDigitCreatorFactory = s_factory;

STDigitCreator::STDigitCreator( const std::string& name, 
                                ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tracker(0)
{
  //constructer
  declareProperty("effToolName", m_effToolName="STEffCalculator");
  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("inputLocation", m_inputLocation = MCSTDigitLocation::TTDigits);
  declareProperty("outputLocation", m_outputLocation = STDigitLocation::TTDigits); 
  declareProperty("tailStart", m_tailStart = 3.0);
  declareProperty("saturation",m_saturation = 127.);
  declareProperty("detType", m_detType = "TT"); 

}

STDigitCreator::~STDigitCreator() {
  //destructer
}

StatusCode STDigitCreator::initialize() {
 
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
   
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
 
  STDetSwitch::flip(m_detType,m_inputLocation);
  STDetSwitch::flip(m_detType,m_outputLocation);

   // eff tool
  m_effTool = tool<ISTEffCalculator>(m_effToolName,this);

  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName+m_detType);

  // random numbers
  sc = randSvc()->generator(Rndm::Flat(0.,1.),m_uniformDist.pRef());

  // Gaussian
  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussDist.pRef());

  // get a Gauss tail
  sc = randSvc()->generator(Rndm::GaussianTail(m_tailStart,1.),m_gaussTailDist.pRef());

  m_fracOfNoiseStrips = 0.5*gsl_sf_erfc(m_tailStart/sqrt(2.0));
  m_numNoiseStrips = (int)(m_fracOfNoiseStrips*m_tracker->nStrip());

  return StatusCode::SUCCESS;
}

StatusCode STDigitCreator::execute() {

 

  // Retrieve digit 
  MCSTDigits* mcDigitCont  = get<MCSTDigits>(m_inputLocation);
  
  // cache mcdigits
  this->initCache(mcDigitCont);

  // create digits
  STDigits* digitsCont = new STDigits();
  createDigits(mcDigitCont,digitsCont);
    
  // generate random noise
  std::vector<digitPair> noiseCont;
  genRanNoiseStrips(noiseCont);
  
  // merge containers
  mergeContainers(noiseCont,digitsCont);

  // resort by channel
  std::stable_sort(digitsCont->begin(),
                   digitsCont->end(),
                   STDataFunctor::Less_by_Channel<const STDigit*>());

  // Ensure that there is neighbours for all strips
  addNeighbours(digitsCont);

  // and finally resort
  std::stable_sort(digitsCont->begin(),
                   digitsCont->end(),
                   STDataFunctor::Less_by_Channel<const STDigit*>());

  put(digitsCont,m_outputLocation);
    
  StatusCode sc = setMCTruth( digitsCont , mcDigitCont );
  if( sc.isFailure()){
    return Error("Failed to set MCTruth info", sc) ;
  }
 
  return StatusCode::SUCCESS;
}

void STDigitCreator::initCache(MCSTDigits* digits){

  m_lastIter = digits->end();
  m_cachedIter = digits->begin();
}


void STDigitCreator::genRanNoiseStrips(std::vector<digitPair>& noiseCont) const{

  // random noise strips
  noiseCont.reserve(m_numNoiseStrips);
  unsigned int nSector = m_tracker->sectors().size();
  const DeSTDetector::Sectors& tSectors = m_tracker->sectors();
  for (unsigned int iNoiseStrip=0; iNoiseStrip<m_numNoiseStrips; ++iNoiseStrip){
    // gen a random wafer
    int iSector = (int)(m_uniformDist->shoot()*nSector);
    DeSTSector* aSector = tSectors[iSector];
    int iStrip = (int)(m_uniformDist->shoot()*aSector->nStrip())+1;
    STChannelID aChan = STChannelID(aSector->elementID().type(),
                                    aSector->elementID().station(),
                                    aSector->elementID().layer(),
                                    aSector->elementID().detRegion(),
                                    aSector->elementID().sector(),
                                    iStrip);
   
    double ranNoise = m_sigNoiseTool->noiseInADC(aSector) * m_gaussTailDist->shoot();
    noiseCont.push_back(std::make_pair(ranNoise,aChan));
  } // iNoiseStrip    
    
  std::sort(noiseCont.begin(),noiseCont.end(),Less_by_Channel());
  
}  

void STDigitCreator::createDigits(MCSTDigits* mcDigitCont,
                                     STDigits* digitsCont){

  // add gaussian noise to real hit channels + allow xxx% dead channels
  digitsCont->reserve(mcDigitCont->size()+m_numNoiseStrips);
  for ( MCSTDigits::const_iterator iterMC = mcDigitCont->begin();
       iterMC !=mcDigitCont->end();++iterMC){

    if (m_effTool->accept() == true){

      // charge including noise
      const SmartRefVector<MCSTDeposit> depositCont = (*iterMC)->mcDeposit();
      double totalCharge = std::accumulate(depositCont.begin(),
                      depositCont.end(),0.,
                      STDataFunctor::Accumulate_Charge<const MCSTDeposit*>());
      totalCharge += (m_gaussDist->shoot()*m_sigNoiseTool->noiseInADC((*iterMC)->channelID()));

      // make digit and add to container.....    
      STDigit* newDigit = new STDigit(GSL_MIN(LHCbMath::round(totalCharge),m_saturation));
      digitsCont->insert(newDigit,(*iterMC)->channelID());
    } // isOK
  } // iterDigit
}

void STDigitCreator::mergeContainers(const std::vector<digitPair>& noiseCont, 
                     STDigits* digitsCont){

  // trick bit merge the two containers (sort of...)
  STChannelID prevChan(0u,0u,0u,0u,0u,0u);
  std::vector<digitPair>::const_iterator iterNoise =  noiseCont.begin();
  while (iterNoise != noiseCont.end()){
    if ((0 == findDigit(iterNoise->second))&&
     (prevChan != iterNoise->second)){
     // strip was not hit add noise
     if ( m_effTool->accept() == true){     
       STDigit* newDigit = new STDigit(GSL_MIN(LHCbMath::round(iterNoise->first),m_saturation));
       digitsCont->insert(newDigit,iterNoise->second);
      } // alive
    }   // findDigit  
    prevChan = iterNoise->second;       
    ++iterNoise;
  } //iterNoise
}

MCSTDigit* STDigitCreator::findDigit(const STChannelID& aChan){

  MCSTDigit* mcDigit = 0;
 
  while ((m_cachedIter != m_lastIter)&&((*m_cachedIter)->channelID() < aChan)){
    ++m_cachedIter;
  } // m_cachedIter
 
  if ((m_cachedIter != m_lastIter)
     &&((*m_cachedIter)->channelID() == aChan)){
    mcDigit = *m_cachedIter;
  } 
 
  return mcDigit;
}

void STDigitCreator::addNeighbours(STDigits* digitsCont) const{

  STDigits::iterator curDigit = digitsCont->begin();
  std::vector<digitPair> tmpCont;
  for( ; curDigit != digitsCont->end(); ++curDigit){ 
    if (curDigit != digitsCont->begin()){
      STDigits::iterator prevDigit = curDigit;
      --prevDigit; 
      STChannelID leftChan = m_tracker->nextLeft((*curDigit)->channelID());
      if (( (*prevDigit)->channelID() != leftChan) &&(leftChan != 0u)){
         tmpCont.push_back(std::make_pair(genInverseTail(),leftChan));
      } // prev test
    }

    STDigits::iterator nextDigit = curDigit;
    ++nextDigit;
    if ((nextDigit != digitsCont->end())){
      STChannelID rightChan = m_tracker->nextRight((*curDigit)->channelID());
      if ( ((*nextDigit)->channelID() != rightChan) && ((rightChan != 0u))){
         tmpCont.push_back(std::make_pair(genInverseTail(),rightChan));
      } // prev test
    }

  } //iterDigit 

  for ( std::vector<digitPair>::iterator iterP = tmpCont.begin(); iterP != tmpCont.end(); ++iterP){   
    if (!digitsCont->object(iterP->second)){
      // do better sometimes we can make twice ie we start with 101
      STDigit* aDigit = new STDigit(GSL_MIN(LHCbMath::round(iterP->first),m_saturation));
      digitsCont->insert(aDigit,iterP->second);
    }
  } //iterP

}

double STDigitCreator::genInverseTail() const{
  double testVal;
  do {
    testVal = m_gaussDist->shoot();
  } while(testVal > m_tailStart);
  return testVal;
}
