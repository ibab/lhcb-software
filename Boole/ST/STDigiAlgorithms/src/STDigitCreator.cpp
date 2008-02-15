// $Id: STDigitCreator.cpp,v 1.1.1.1 2008-02-15 13:18:48 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// GSL
#include "gsl/gsl_sf_erf.h"

// Event
#include "Event/MCTruth.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// LHCbKernel
#include "Kernel/ISTSignalToNoiseTool.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/STDetSwitch.h"

// local
#include "STDigitCreator.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigitCreator );

STDigitCreator::STDigitCreator( const std::string& name,
                                ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tracker(0)
{
  //constructer
  declareProperty("EffToolName", m_effToolName="STEffCalculator");
  declareProperty("SigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("InputLocation", m_inputLocation=MCSTDigitLocation::TTDigits);
  declareProperty("OutputLocation", m_outputLocation=STDigitLocation::TTDigits);
  declareProperty("TailStart", m_tailStart = 3.0);
  declareProperty("Saturation", m_saturation = 127.);
  declareProperty("DetType", m_detType = "TT");
  declareProperty("allStrips", m_allStrips = false);
  declareProperty("useStatusConditions", m_useStatusConditions = false);

}

STDigitCreator::~STDigitCreator()
{
  //destructer
}

StatusCode STDigitCreator::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  STDetSwitch::flip(m_detType,m_inputLocation);
  STDetSwitch::flip(m_detType,m_outputLocation);


  m_sigNoiseTool = tool<ISTSignalToNoiseTool>( m_sigNoiseToolName,
                                               m_sigNoiseToolName+m_detType);

  // random numbers generators (flat, gaussian and gaussian tail)
  sc = randSvc()->generator(Rndm::Flat(0.,1.),m_uniformDist.pRef());
  sc = randSvc()->generator(Rndm::Gauss(0.,1.),m_gaussDist.pRef());
  sc = randSvc()->generator(Rndm::GaussianTail(m_tailStart,1.),
                            m_gaussTailDist.pRef());

  // cache the number of noise strips
  if (!m_allStrips){
    double fracOfNoiseStrips = 0.5*gsl_sf_erfc(m_tailStart/sqrt(2.0));
    m_numNoiseStrips = (int)(fracOfNoiseStrips*m_tracker->nStrip());
  }
  else {
    m_numNoiseStrips = m_tracker->nStrip();
  }
  return sc;
}

StatusCode STDigitCreator::execute()
{
  // Retrieve MCSTDigits
  MCSTDigits* mcDigitCont = get<MCSTDigits>(m_inputLocation);

  // create STDigits
  STDigits* digitsCont = new STDigits();
  createDigits(mcDigitCont,digitsCont);

  // cache first and last digit
  m_lastIter = digitsCont->end();
  m_cachedIter = digitsCont->begin();

  // generate random noise
  std::vector<digitPair> noiseCont;
  genRanNoiseStrips(noiseCont);

  // merge containers
  mergeContainers(noiseCont,digitsCont);

  // resort by channel
  std::stable_sort( digitsCont->begin(), digitsCont->end(),
                    STDataFunctor::Less_by_Channel<const STDigit*>() );

  // Ensure that there is neighbours for all strips
  addNeighbours(digitsCont);

  // and finally resort
  std::stable_sort( digitsCont->begin(), digitsCont->end(),
                    STDataFunctor::Less_by_Channel<const STDigit*>() );

  // register STDigits in the transient data store
  put(digitsCont,m_outputLocation);


  // Make the link between digits and mcdigits
  StatusCode sc = setMCTruth( digitsCont , mcDigitCont );
  if( sc.isFailure()) return Error("Failed to set MCTruth info", sc) ;

  return sc;
}

void STDigitCreator::genRanNoiseStrips(std::vector<digitPair>& noiseCont) const
{
  // generate random noise strips
  noiseCont.reserve(m_numNoiseStrips);
  unsigned int nSector = m_tracker->sectors().size();

  // create noise strips in loop
  const DeSTDetector::Sectors& tSectors = m_tracker->sectors();
  if (!m_allStrips){
    for (unsigned int iNoiseStrip=0u; iNoiseStrip<m_numNoiseStrips; ++iNoiseStrip){
      // generate a random readout sector
      int iSector = (int)(m_uniformDist->shoot()*nSector);
      DeSTSector* aSector = tSectors[iSector];
      int iStrip = (int)(m_uniformDist->shoot()*aSector->nStrip())+1;
      STChannelID aChan = aSector->stripToChan(iStrip); 

      if (m_useStatusConditions == false ||  aSector->isOKStrip(aChan) == true){
        // Generate a ADC value following a gaussian tail distribution
        double ranNoise = m_sigNoiseTool->noiseInADC(aSector) *
        m_gaussTailDist->shoot();
        noiseCont.push_back(std::make_pair(ranNoise,aChan));
      } // alive strip
    } // iNoiseStrip
  } 
  else {
    DeSTDetector::Sectors::const_iterator iterS = tSectors.begin(); 
    for ( ; iterS != tSectors.end(); ++iterS){
      for (unsigned int iStrip =1u;  iStrip <= (*iterS)->nStrip();  ++iStrip){

        STChannelID aChan = (*iterS)->stripToChan(iStrip);
        double ranNoise = m_sigNoiseTool->noiseInADC(*iterS) * m_gaussDist->shoot();
        noiseCont.push_back(std::make_pair(ranNoise,aChan));
      } // iStrip
    } // for
  }

  // Sort the noise digits by STChannelID
  std::sort( noiseCont.begin(), noiseCont.end(), Less_by_Channel() );
}

void STDigitCreator::createDigits(MCSTDigits* mcDigitCont, STDigits* digitsCont)
{
  // add gaussian noise to real hit channels + allow xxx% dead channels
  digitsCont->reserve( mcDigitCont->size() + m_numNoiseStrips );
  MCSTDigits::const_iterator iterMC = mcDigitCont->begin();
  for ( ; iterMC != mcDigitCont->end(); ++iterMC ) {

    // charge including noise
    const SmartRefVector<MCSTDeposit> depositCont = (*iterMC)->mcDeposit();
    double totalCharge =
       std::accumulate(depositCont.begin(),depositCont.end(), 0.,
                       STDataFunctor::Accumulate_Charge<const MCSTDeposit*>());
    totalCharge += (m_gaussDist->shoot() *
                    m_sigNoiseTool->noiseInADC((*iterMC)->channelID()) );

    // make digit and add to container.
    STDigit* newDigit = new STDigit( adcValue( totalCharge ) );
    digitsCont->insert(newDigit,(*iterMC)->channelID());
  } // iterDigit
}

void STDigitCreator::mergeContainers( const std::vector<digitPair>& noiseCont,
                                      STDigits* digitsCont )
{
  // merge the two containers
  STChannelID prevChan(0u,0u,0u,0u,0u,0u);
  std::vector<digitPair>::const_iterator iterNoise = noiseCont.begin();
  while (iterNoise != noiseCont.end()) {
    // if strip was not hit: add noise
    if ((0 == findDigit(iterNoise->second)) && (prevChan != iterNoise->second)){
      STDigit* newDigit = new STDigit( adcValue(iterNoise->first) );
      digitsCont->insert(newDigit,iterNoise->second);
    }   // findDigit
    prevChan = iterNoise->second;
    ++iterNoise;
  } //iterNoise
}

STDigit* STDigitCreator::findDigit(const STChannelID& aChan)
{
  STDigit* digit = 0;

  while ((m_cachedIter != m_lastIter)&&((*m_cachedIter)->channelID() < aChan)){
    ++m_cachedIter;
  } // m_cachedIter

  if ((m_cachedIter != m_lastIter)&&((*m_cachedIter)->channelID() == aChan)){
    digit = *m_cachedIter;
  }

  return digit;
}

void STDigitCreator::addNeighbours(STDigits* digitsCont) const
{
  STDigits::iterator curDigit = digitsCont->begin();
  std::vector<digitPair> tmpCont;
  for( ; curDigit != digitsCont->end(); ++curDigit ) {

    // Get left neighbour
    const DeSTSector* aSector = m_tracker->findSector((*curDigit)->channelID());
    STChannelID leftChan = aSector->nextLeft((*curDigit)->channelID());

    // Don't add left neighbour if this neighbour is already hit
    STChannelID prevDigitChan = 0u;
    if (curDigit != digitsCont->begin()){
      STDigits::iterator prevDigit = curDigit;
      --prevDigit;
      prevDigitChan = (*prevDigit)->channelID();
    }
    if ( (leftChan != prevDigitChan) && (leftChan != 0u) ) {
      tmpCont.push_back(std::make_pair(genInverseTail(),leftChan));
    }
  
    // Get right neighbour
    STChannelID rightChan = aSector->nextRight((*curDigit)->channelID());

    // Don't add right neighbour if this neighbour is already hit
    STChannelID nextDigitChan = 0u;
    STDigits::iterator nextDigit = curDigit;
    ++nextDigit;
    if ((nextDigit != digitsCont->end())){
      nextDigitChan = (*nextDigit)->channelID();
    }
    if ( (rightChan != nextDigitChan) && (rightChan != 0u) ){
      tmpCont.push_back(std::make_pair(genInverseTail(),rightChan));
    }
    
  } //iterDigit

  std::vector<digitPair>::iterator iterP = tmpCont.begin();
  for ( ; iterP != tmpCont.end(); ++iterP){
    if (!digitsCont->object(iterP->second)){
      // do better sometimes we can make twice ie we start with 101
      DeSTSector* aSector = m_tracker->findSector(iterP->second);
      if (m_useStatusConditions == false|| aSector->isOKStrip(iterP->second) == true){
        STDigit* aDigit = new STDigit( adcValue(iterP->first) );
        digitsCont->insert(aDigit,iterP->second);
      } //ok strip
    }
  } //iterP
}

double STDigitCreator::genInverseTail() const
{
  double testVal;
  do {
    testVal = m_gaussDist->shoot();
  } while(testVal > m_tailStart);
  return testVal;
}
