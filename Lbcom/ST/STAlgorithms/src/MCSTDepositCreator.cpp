// Standard
#include <algorithm>
#include <math.h>

//GSL 
#include "gsl/gsl_math.h"

// Booost
#include <boost/lexical_cast.hpp>

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// CLHEP
#include "Kernel/SystemOfUnits.h"

#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/LHCbMath.h"

#include "MCSTDepositCreator.h"
#include "Kernel/STDetSwitch.h"

#include "ISTChargeSharingTool.h"
#include "ISTAmplifierResponse.h"
#include "ISTDepositedCharge.h"
#include "Kernel/ISTSignalToNoiseTool.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

using namespace LHCb;

static const AlgFactory<MCSTDepositCreator>  s_factory;
const IAlgFactory& MCSTDepositCreatorFactory = s_factory;

MCSTDepositCreator::MCSTDepositCreator(const std::string& name, ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructer

  m_SpillVector.push_back("/");
  m_spillTimes.push_back(0.*ns);

  declareProperty("tofVector",m_TOFVector);
  declareProperty("spillVector", m_SpillVector);
  declareProperty("spillTimes", m_spillTimes);
  declareProperty("minDist", m_minDistance = 10.0e-3*mm);

  declareProperty("chargeSharerName",m_chargeSharerName = "STChargeSharingTool");
  declareProperty("depChargeTool", m_depChargeToolName = "STDepositedCharge");

  declareProperty("siteSize",m_siteSize = 0.02*mm);
  declareProperty("maxNumSites",m_maxNumSites = 150);

  declareProperty("xTalkParams",m_xTalkParams);

  m_xTalkParams.push_back(0.08);
  m_xTalkParams.push_back(0.092/(55*picofarad));

  declareProperty("detType", m_detType = "TT"); 

  declareProperty("sigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("responseTypes", m_beetleResponseTypes);

  declareProperty("scaling", m_scaling = 1.0);

  m_inputLocation = MCHitLocation::TT; 
  m_outputLocation = MCSTDepositLocation::TTDeposits;
 
}

MCSTDepositCreator::~MCSTDepositCreator() {
  //destructer
}

StatusCode MCSTDepositCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
  STDetSwitch::flip(m_detType,m_outputLocation);
  STDetSwitch::flip(m_detType,m_inputLocation);

  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, m_sigNoiseToolName+m_detType);

  // 
  m_chargeSharer = tool<ISTChargeSharingTool>(m_chargeSharerName, m_chargeSharerName,this);

  // deposited charge 
  m_depositedCharge = tool<ISTDepositedCharge>(m_depChargeToolName,"depCharge",this);
  
  // make beetle response functions
  std::vector<std::string>::const_iterator iterType = m_beetleResponseTypes.begin();
  for (; iterType != m_beetleResponseTypes.end() ; ++iterType){
    std::string name = "response"+(*iterType);
    ISTAmplifierResponse* aResponse = tool<ISTAmplifierResponse>("STAmplifierResponse",name,this);
    m_AmplifierResponse.push_back(aResponse); 
  }

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_SpillVector.begin() ;
  while (iSpillName!=m_SpillVector.end()){
    // path in Transient data store
    std::string mcHitPath = "/Event"+(*iSpillName)+m_inputLocation;
    m_spillNames.push_back(mcHitPath);    
    ++iSpillName;
  } // iterSpillName

  return StatusCode::SUCCESS;
}

StatusCode MCSTDepositCreator::execute() {

  // execute   
  MCSTDeposits* depositsCont = new MCSTDeposits();
  depositsCont->reserve(10000);

  // loop over spills
  for (unsigned int iSpill = 0; iSpill < m_spillNames.size(); ++iSpill){

    if (exist<MCHits>(m_spillNames[iSpill]) == false){   
      // failed to find hits
      debug() << "Unable to retrieve "+m_spillNames[iSpill] << endreq;
    }
    else {
      // found spill - create digitizations and add them to deposits container
      MCHits* hits = get<MCHits>(m_spillNames[iSpill]);
      this->createDeposits(hits,m_spillTimes[iSpill],depositsCont);
    }

  } // iSpill

  // sort by channel
  std::stable_sort(depositsCont->begin(),
            depositsCont->end(),
            STDataFunctor::Less_by_Channel<const MCSTDeposit*>());

  // register container in store
  put(depositsCont,m_outputLocation);

  return StatusCode::SUCCESS;
}
 

StatusCode MCSTDepositCreator::createDeposits(const MCHits* mcHitsCont, 
                                       const double spillTime, 
                                       MCSTDeposits* depositCont) {
  // loop tracking hits

  MCHits::const_iterator iterHit = mcHitsCont->begin();
  for (; iterHit!=mcHitsCont->end();++iterHit){

    MCHit* aHit = *iterHit;

    DeSTSector* aSector = m_tracker->findSector(aHit->midPoint());
    if ((0 != aSector)&&(this->hitToDigitize(aHit))){           

      // global to local transformation
      Gaudi::XYZPoint entryPoint = aSector->toLocal(aHit->entry());
      Gaudi::XYZPoint exitPoint = aSector->toLocal(aHit->exit());
      Gaudi::XYZPoint midPoint = aSector->toLocal(aHit->midPoint());
        
      if (aSector->localInActive(midPoint) == true){

	// deposited charge on strip calc the ionization
        double ionization = m_depositedCharge->charge(aHit);
        	    
	// distribute charge to n sites
	std::vector<double> chargeSites;

        this->distributeCharge(entryPoint.x(),exitPoint.x(),chargeSites);

        // doing charge sharing + go to strips
	std::map<unsigned int,double> stripMap;
        chargeSharing(chargeSites,aSector,stripMap);

	// correct normalization of charge
        double totWeightedCharge = chargeOnStrips(stripMap);
      
        if (totWeightedCharge > 1e-3 ){

   	  // capacitive coupling
          std::map<unsigned int,double>::iterator firstIter = stripMap.begin();
          std::map<unsigned int,double>::iterator lastIter = stripMap.end();
          lastIter--;
          unsigned int firstStrip = firstIter->first;
          unsigned int lastStrip = lastIter->first;

          unsigned int iStrip;

          const double xTalkLevel = m_xTalkParams[0] + m_xTalkParams[1]*aSector->capacitance();
	  const double scaling = m_scaling * (1.0+(2.0*xTalkLevel));

          STChannelID elemChan = aSector->elementID();
          for (iStrip = firstStrip; iStrip <= lastStrip; ++iStrip){

            double prevCharge = 0.0;
            if (iStrip != firstStrip){
              prevCharge = stripMap[iStrip-1];
	    }

            double nextCharge = 0.0;
            if (iStrip != lastStrip){
              nextCharge = stripMap[iStrip+1];
	    }
           
            double weightedCharge = ((1.-(2.0*xTalkLevel))*stripMap[iStrip])
  	                           + (xTalkLevel*(nextCharge+prevCharge)); 
  
            // amplifier response - fraction of charge it sees
            double beetleFraction;          
            if (iStrip != firstStrip && iStrip != lastStrip){
              beetleFraction = beetleResponse(m_TOFVector[elemChan.station()-1]  
                                           -aHit->time()-spillTime, aSector->capacitance(),
                                           STAmpliferResponseType::signal);
	    }
            else {
              beetleFraction = beetleResponse(m_TOFVector[elemChan.station()-1]  
                                              -aHit->time()-spillTime, aSector->capacitance()
                                              ,STAmpliferResponseType::capCoupling);
	    }

            STChannelID aChan(DeSTDetLocation::detType(m_detType),
                              elemChan.station(), elemChan.layer(), 
                              elemChan.detRegion(), elemChan.sector(), iStrip);

            double electrons = ionization*beetleFraction*scaling*weightedCharge/totWeightedCharge;

            double adcCounts = m_sigNoiseTool->convertToADC(electrons);

	    MCSTDeposit* newDeposit = new MCSTDeposit(adcCounts,aChan,aHit); 
	    depositCont->insert(newDeposit);
	  } // loop strip
        } // if has some charge
      }  // in active area
    } // hitToDigitize
  } // iterHit

  return StatusCode::SUCCESS;
}


bool MCSTDepositCreator::hitToDigitize(const MCHit* aHit) const{

  // check whether it is reasonable to digitize this hit

  // check if it goes through some Si ....
  if (aHit->pathLength() < m_minDistance ) return false; 


  // some hits have a zero p...
  if (aHit->p() < 1e-3){
    Warning( "Hit with zero p - not digitized", StatusCode::SUCCESS, 1 );
    return false;
  }

  // check hit too steep - can't digitize yet....
  if (fabs(aHit->entry().z() - aHit->exit().z()) < LHCbMath::lowTolerance) { 
    Warning( "Too steep hit - not digitized", StatusCode::SUCCESS, 1 );
    return false;
  }

  return true;
}

void MCSTDepositCreator::distributeCharge(const double entryU, const double exitU, std::vector<double>& sites) const{

  // distribute charge homogeneously at n Sites
  double deltaU = fabs(exitU - entryU);
  int nSites;
  if (deltaU<=m_siteSize){
    nSites = 1;
  }
  else {
    nSites = GSL_MIN((int)floor(fabs(exitU - entryU)/m_siteSize) + 1,
                      m_maxNumSites);
  }

  sites.reserve(nSites);

  double startU = entryU;
  double stopU = exitU;
  if (startU>stopU){
    std::swap(startU,stopU);
  }

  double siteSize = deltaU/nSites;
  int iSite;
  for (iSite  = 0; iSite < nSites; ++iSite){
    sites.push_back(startU+((double)iSite+0.5)*siteSize);
  } // iSite
 
}

void MCSTDepositCreator::chargeSharing(const std::vector<double>& sites ,
                                       const DeSTSector* aSector,
                                       std::map<unsigned int,double>& stripMap) const{

  // init
  double chargeOnSite = 1.0/((double)sites.size());
  std::vector<double>::const_iterator iterSite = sites.begin();
  double frac = 0.0;
  unsigned int firstStrip;
  unsigned int secondStrip;

  // loop on sites
  while (iterSite != sites.end()){

    // closest strips to site
    firstStrip = aSector->localUToStrip(*iterSite);
    secondStrip = firstStrip+1;
  
    // do the sharing  first Strip!
    if (aSector->isStrip(firstStrip)){
      frac = m_chargeSharer->sharing(fabs(*iterSite-aSector->localU(firstStrip))/aSector->pitch());
      stripMap[firstStrip] += frac*chargeOnSite;
    }

    // second strip - if there is one
    if (aSector->isStrip(secondStrip)){
      frac = m_chargeSharer->sharing(fabs(*iterSite-aSector->localU(secondStrip))/aSector->pitch());
      stripMap[secondStrip] += frac*chargeOnSite;
    }

    ++iterSite;
  } // itersites

  // add entry for strip before first and after last
  std::map<unsigned int,double>::iterator startIter = stripMap.begin();
  std::map<unsigned int,double>::iterator lastIter = stripMap.end();
  lastIter--;

  firstStrip = startIter->first;
  if (aSector->isStrip(firstStrip-1)){
    stripMap[firstStrip-1] += 0.0;
  }

  unsigned int lastStrip = lastIter->first;
  if (aSector->isStrip(lastStrip+1)){
    stripMap[lastStrip+1] += 0.0;
  }
}

double MCSTDepositCreator::chargeOnStrips(const std::map<unsigned int,double>& stripMap) const{

  double totCharge = 0.0;
  std::map<unsigned int,double>::const_iterator iterMap =  stripMap.begin();
  while (iterMap != stripMap.end()){
    totCharge += iterMap->second;
    ++iterMap;
  } // iterMap

  return totCharge;
}

double MCSTDepositCreator::beetleResponse(const double time, 
                                          const double capacitance,
                                          const std::string& type) {
  
  // choose the best spline for our needs...
  ISTAmplifierResponse* bResponse = 0;
  double testCap = 9999.0*picofarad;
  std::vector<ISTAmplifierResponse*>::iterator iter = m_AmplifierResponse.begin();
  for (; iter != m_AmplifierResponse.end(); ++iter){
    ISTAmplifierResponse::Info properties = (*iter)->validity();
    if ((fabs(properties.capacitance -  capacitance) < testCap ) &&
       (properties.type == type)) {
      testCap = fabs(properties.capacitance -  capacitance);
      bResponse = *iter;
    } 
  } // iter
  if (bResponse == 0) warning() << "Failed to match amplifier response " << endmsg;

  return (bResponse !=0 ? bResponse->response(time): 0);
}









