// $Id: STDigitCreator.cpp,v 1.4 2006-02-02 09:06:04 mneedham Exp $

#include "gsl/gsl_math.h"

#include "GaudiKernel/AlgFactory.h"
#
//Random Numbers
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"


#include "Kernel/SystemOfUnits.h"

#include "Event/MCTruth.h"
#include "Event/MCSTDeposit.h"


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
  declareProperty("fracNoiseStrips", m_fracOfNoiseStrips = 1.35e-3);
  declareProperty("tailStart", m_tailStart = 3.0);
  declareProperty("saturation",m_saturation = 150.);
  declareProperty("detType", m_detType = "TT"); 

  m_sectors.reserve(300);

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

  // cache sectors
  m_sectors = m_tracker->sectors();

   // eff tool
  m_effTool = tool<ISTEffCalculator>(m_effToolName,this);

  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName+m_detType);

  // random numbers
  sc = randSvc()->generator(Rndm::Flat(0.,1.),m_uniformDist.pRef());

  // Gaussian
  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussDist.pRef());

  // get a Gauss tail
  sc = randSvc()->generator(Rndm::GaussianTail(m_tailStart,1.),m_gaussTailDist.pRef());

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
  for (unsigned int iNoiseStrip=0; iNoiseStrip<m_numNoiseStrips; ++iNoiseStrip){
    // gen a random wafer
    int iSector = (int)(m_uniformDist->shoot()*m_sectors.size());
    DeSTSector* aSector = m_sectors[iSector];
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
      STDigit* newDigit = new STDigit(GSL_MIN(floor(totalCharge),m_saturation));
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
       STDigit* newDigit = new STDigit(GSL_MIN(floor(iterNoise->first),m_saturation));
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




