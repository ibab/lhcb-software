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

#include "Kernel/STDataFunctor.h"


// local
#include "STDigitCreator.h"
#include "ISTCMSimTool.h"
#include "ISTPedestalSimTool.h"


using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigitCreator )

STDigitCreator::STDigitCreator( const std::string& name,
                                ISvcLocator* pSvcLocator ) :
  ST::AlgBase(name, pSvcLocator)
{
  //constructer

  declareSTConfigProperty("InputLocation", m_inputLocation ,MCSTDigitLocation::TTDigits);
  declareSTConfigProperty("OutputLocation", m_outputLocation, STDigitLocation::TTDigits);
  declareProperty("TailStart", m_tailStart = 2.5);
  declareProperty("Saturation", m_saturation = 127.);
 
  declareProperty("addPedestal", m_addPedestal = false );
  declareProperty("pedestalToolName", m_pedestalToolName = "STPedestalSimTool" );

  declareProperty("addCommonMode", m_addCommonMode = false );
  declareProperty("cmToolName", m_cmToolName = "STCMSimTool" );

  declareProperty("allStrips", m_allStrips = false);
  declareProperty("useStatusConditions", m_useStatusConditions = true);

  setForcedInit();
}

STDigitCreator::~STDigitCreator()
{
  //destructer
}

StatusCode STDigitCreator::initialize()
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  if (m_addPedestal) 
    m_pedestalTool = tool<ISTPedestalSimTool>( m_pedestalToolName,  m_pedestalToolName,  this);

  if (m_addCommonMode) 
    m_cmTool = tool<ISTCMSimTool>( m_cmToolName,  m_cmToolName,  this);

  
  // random numbers generators (flat, gaussian and gaussian tail)
  m_uniformDist = randSvc()->generator(Rndm::Flat(0.,1.));
  m_gaussDist = randSvc()->generator(Rndm::Gauss(0.,1.));
  m_gaussTailDist = randSvc()->generator(Rndm::GaussianTail(m_tailStart,1.));

  // cache the number of noise strips
  if (!m_allStrips){
    double fracOfNoiseStrips = 0.5*gsl_sf_erfc(m_tailStart/sqrt(2.0));
    m_numNoiseStrips = (int)(fracOfNoiseStrips*tracker()->nStrip());
  } else {
    m_numNoiseStrips = tracker()->nStrip();
  }
  return sc;
}

StatusCode STDigitCreator::execute()
{
  // Retrieve MCSTDigits
  const MCSTDigits* mcDigitCont = get<MCSTDigits>(m_inputLocation);

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
  unsigned int nSector = tracker()->sectors().size();

  // create noise strips in loop
  const DeSTDetector::Sectors& tSectors = tracker()->sectors();
  if (!m_allStrips){
    for (unsigned int iNoiseStrip=0u; iNoiseStrip<m_numNoiseStrips; ++iNoiseStrip){
      // generate a random readout sector
      int iSector = (int)(m_uniformDist->shoot()*nSector);
      DeSTSector* aSector = tSectors[iSector];
      int iStrip = (int)(m_uniformDist->shoot()*aSector->nStrip())+1;
      STChannelID aChan = aSector->stripToChan(iStrip); 

      if (m_useStatusConditions == false ||  aSector->isOKStrip(aChan) == true){
        // Generate a ADC value following a gaussian tail distribution
        double ranNoise = aSector->noise(aChan) *
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
        double ranNoise = (*iterS)->noise(aChan) * m_gaussDist->shoot();
        noiseCont.push_back(std::make_pair(ranNoise,aChan));
      } // iStrip
    } // for
  }

  // Sort the noise digits by STChannelID
  std::stable_sort( noiseCont.begin(), noiseCont.end(), Less_by_Channel() );
}

void STDigitCreator::createDigits(const MCSTDigits* mcDigitCont, STDigits* digitsCont)
{
  // add gaussian noise to real hit channels + allow xxx% dead channels
  digitsCont->reserve( mcDigitCont->size() + m_numNoiseStrips );
  MCSTDigits::const_iterator iterMC = mcDigitCont->begin();
  for ( ; iterMC != mcDigitCont->end(); ++iterMC ) {

    // charge including noise
    const DeSTSector* sector = findSector((*iterMC)->channelID());
    const SmartRefVector<MCSTDeposit> depositCont = (*iterMC)->mcDeposit();
    double totalCharge =
       std::accumulate(depositCont.begin(),depositCont.end(), 0.,
                       STDataFunctor::Accumulate_Charge<const MCSTDeposit*>());
    totalCharge += (m_gaussDist->shoot() *
                    sector->noise((*iterMC)->channelID()) );

    // sim pedestal
    if (m_addPedestal) {
      totalCharge += m_pedestalTool->pedestal((*iterMC)->channelID());
    }

    // sim cm noise
    if (m_addCommonMode){
      totalCharge += m_cmTool->noise((*iterMC)->channelID());
    }

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

      double totalCharge = iterNoise->first;
      // sim pedestal
      if (m_addPedestal) {
         totalCharge += m_pedestalTool->pedestal(iterNoise->second);
      }
      // sim cm noise
      if (m_addCommonMode){
        totalCharge += m_cmTool->noise(iterNoise->second);
      }

      STDigit* newDigit = new STDigit( adcValue(totalCharge));
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
    const DeSTSector* aSector =tracker()->findSector((*curDigit)->channelID());
    STChannelID leftChan = aSector->nextLeft((*curDigit)->channelID());

    // Don't add left neighbour if this neighbour is already hit
    STChannelID prevDigitChan(0u);
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
    STChannelID nextDigitChan(0u);
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
      DeSTSector* aSector = tracker()->findSector(iterP->second);
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
