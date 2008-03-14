// $Id: MCSTDepositCreator.cpp,v 1.3 2008-03-14 18:27:25 mneedham Exp $

// GSL 
#include "gsl/gsl_math.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// LHCbKernel
#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/STDetSwitch.h"
#include "LHCbMath/LHCbMath.h"
#include "Kernel/ISiAmplifierResponse.h"
#include "MCInterfaces/ISiDepositedCharge.h"
#include "Kernel/ISTSignalToNoiseTool.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"


// local
#include "ISTChargeSharingTool.h"
#include "MCSTDepositCreator.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCSTDepositCreator );

MCSTDepositCreator::MCSTDepositCreator( const std::string& name, 
                                        ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator)
{
  m_spillNames.push_back("/");
  m_spillTimes.push_back(0.*Gaudi::Units::ns);

  declareProperty("TofVector", m_tofVector, "vector of flight times");
  declareProperty("SpillVector", m_spillNames);
  declareProperty("SpillTimes", m_spillTimes);
  declareProperty("MinDist", m_minDistance = 10.0e-3*Gaudi::Units::mm);

  declareProperty("ChargeSharerName",m_chargeSharerName ="STChargeSharingTool");
  declareProperty("DepChargeTool", m_depChargeToolName = "SiDepositedCharge");

  declareProperty("SiteSize", m_siteSize = 0.02*Gaudi::Units::mm);
  declareProperty("MaxNumSites", m_maxNumSites = 150);

  declareProperty("XTalkParams", m_xTalkParams);

  m_xTalkParams.push_back(0.08);
  m_xTalkParams.push_back(0.092/(55*Gaudi::Units::picofarad));

  declareProperty("DetType", m_detType = "TT"); 

  declareProperty("SigNoiseTool", m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("Scaling", m_scaling = 1.0);
  declareProperty("ResponseTypes", m_beetleResponseTypes);
  declareProperty("useStatusConditions", m_useStatusConditions = false);
  declareProperty("useSensDetID", m_useSensDetID = false);


  m_inputLocation = MCHitLocation::TT; 
  m_outputLocation = MCSTDepositLocation::TTDeposits;

 
}

MCSTDepositCreator::~MCSTDepositCreator() 
{
  //destructer
}

StatusCode MCSTDepositCreator::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
  STDetSwitch::flip(m_detType,m_outputLocation);
  STDetSwitch::flip(m_detType,m_inputLocation);

  // signal to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, 
                                              m_sigNoiseToolName+m_detType);

  // charge sharing tool
  m_chargeSharer = tool<ISTChargeSharingTool>(m_chargeSharerName, 
                                              m_chargeSharerName,this);

  // deposited charge 
  m_depositedCharge = tool<ISiDepositedCharge>(m_depChargeToolName,
                                               "DepCharge",this);
  
  // make beetle response functions
  std::vector<std::string>::const_iterator iterType = 
    m_beetleResponseTypes.begin();
  for (; iterType != m_beetleResponseTypes.end() ; ++iterType ){
    std::string name = "Response"+(*iterType);
    ISiAmplifierResponse* aResponse = 
      tool<ISiAmplifierResponse>("SiAmplifierResponse",name,this);
    m_amplifierResponse.push_back(aResponse); 
  }

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin() ;
  while (iSpillName != m_spillNames.end()){
    // path in Transient data store
    m_spillPaths.push_back( "/Event"+(*iSpillName)+m_inputLocation );    
    ++iSpillName;
  }

  return StatusCode::SUCCESS;
}

StatusCode MCSTDepositCreator::execute() 
{
  // execute   
  MCSTDeposits* depositsCont = new MCSTDeposits();
  depositsCont->reserve(10000);

  // loop over spills
  for (unsigned int iSpill = 0; iSpill < m_spillPaths.size(); ++iSpill){

    if (exist<MCHits>(m_spillPaths[iSpill]) == false){   
      // failed to find hits
      debug() << "Unable to retrieve " + m_spillPaths[iSpill] << endmsg;
    }
    else {
      // found spill - create digitizations and add them to deposits container
      const MCHits* hits = get<MCHits>(m_spillPaths[iSpill]);
      createDeposits(hits,m_spillTimes[iSpill],depositsCont);
    }
  } // iSpill

  // sort by channel
  std::stable_sort(depositsCont->begin(), depositsCont->end(),
                   STDataFunctor::Less_by_Channel<const MCSTDeposit*>());

  // register container in store
  put(depositsCont,m_outputLocation);

  return StatusCode::SUCCESS;
}
 

void MCSTDepositCreator::createDeposits( const MCHits* mcHitsCont, 
                                         const double spillTime, 
                                         MCSTDeposits* depositCont )
{
  // loop over MChits
  MCHits::const_iterator iterHit = mcHitsCont->begin();
  for (; iterHit!=mcHitsCont->end();++iterHit){    
    MCHit* aHit = *iterHit;

    DeSTSector* aSector = 0;    
    // if (m_useSensDetID == true){
    //  aSector = m_tracker->findSector(aHit->sensDetID());
    // }
    //else {
     aSector = m_tracker->findSector(aHit->midPoint());
      //std::cout << "Find point " << std::endl;
    //}

    if (aSector == 0) {
      //std::cout << STChannelID(aHit->sensDetID()) << std::endl;
      warning() << "point " << aHit->midPoint() << endreq;
      Warning("Failed to find sector", StatusCode::SUCCESS, 1);
      continue;
    }

    if (hitToDigitize(aHit) == true){           
      
      if (m_useStatusConditions && aSector->sectorStatus() == DeSTSector::Dead ) continue;

      // global to local transformation

      // find the sensor
      DeSTSensor* aSensor = aSector->findSensor(aHit->midPoint());
      if (aSensor == 0){
        Warning("Failed to find sensor", StatusCode::SUCCESS, 1);
        continue;
      } 

      const Gaudi::XYZPoint entryPoint = aSensor->toLocal(aHit->entry());
      const Gaudi::XYZPoint exitPoint = aSensor->toLocal(aHit->exit());
      const Gaudi::XYZPoint midPoint = aSensor->toLocal(aHit->midPoint());
        
      if (aSensor->localInActive(midPoint) == true) {

        // Calculate the deposited charge on strip
        const double ionization = m_depositedCharge->charge(aHit);
        	    
        // distribute charge to n sites
        std::vector<double> chargeSites;
        distributeCharge(entryPoint.x(),exitPoint.x(),chargeSites);

        // doing charge sharing + go to strips
        std::map<unsigned int,double> stripMap;
        double totWeightedCharge = 0.0;
        chargeSharing(chargeSites,aSensor,stripMap, totWeightedCharge);
      
        if (totWeightedCharge > 1e-3 ){

          // Determine cross talk level for this readout sector
          const double xTalkLevel = m_xTalkParams[0] + 
            m_xTalkParams[1]*aSector->capacitance();
          const double scaling = m_scaling * (1.0+(2.0*xTalkLevel));
          STChannelID elemChan = aSector->elementID();

          // loop over strips and simulate capacitive coupling
          unsigned int firstStrip = (stripMap.begin())->first;
          unsigned int lastStrip = (--stripMap.end())->first;
          unsigned int iStrip = firstStrip;
          for ( ; iStrip <= lastStrip; ++iStrip ) {

            // Get the charge of the previous and next strips
            double prevCharge = 0.0;
            if (iStrip != firstStrip) prevCharge = stripMap[iStrip-1];
            double nextCharge = 0.0;
            if (iStrip != lastStrip) nextCharge = stripMap[iStrip+1];
           
            // Capacitive coupling
            double weightedCharge = (1.- 2.0*xTalkLevel)*stripMap[iStrip]
              + xTalkLevel*(nextCharge+prevCharge); 
            
            // amplifier response - fraction of charge it sees
            double beetleFraction;          
            if (iStrip != firstStrip && iStrip != lastStrip){
              beetleFraction = beetleResponse(m_tofVector[elemChan.station()-1]
                                              -aHit->time()-spillTime,
                                              aSector->capacitance(),
                                              SiAmpliferResponseType::signal);
            }
            else {
              beetleFraction = beetleResponse(m_tofVector[elemChan.station()-1]
                                              -aHit->time()-spillTime,
                                              aSector->capacitance(),
                                           SiAmpliferResponseType::capCoupling);
            }

            STChannelID aChan = aSector->stripToChan(iStrip);

            if ( m_useStatusConditions == false || aSector->isOKStrip(aChan) == true){
            
              const double electrons = ionization*beetleFraction*scaling*weightedCharge
                                 /totWeightedCharge;

              const double adcCounts = m_sigNoiseTool->convertToADC(electrons);
            
              MCSTDeposit* newDeposit = new MCSTDeposit(adcCounts,aChan,aHit); 
              depositCont->insert(newDeposit);
	    } // ok strip
          } // loop strip
        } // if has some charge
      }  // in active area
    } // hitToDigitize
  } // iterHit

}


bool MCSTDepositCreator::hitToDigitize(const MCHit* aHit) const
{
  // check whether it is reasonable to digitize this hit

  // check if it goes through some Si ....
  if (aHit->pathLength() < m_minDistance ) return false;

  // some hits have a zero p...
  if (aHit->p() < 1e-3){
    Warning( "Hit with zero p - not digitized", StatusCode::SUCCESS, 1 );
    return false;
  }

  // check if entry and exit point are at the same z-position
  if (fabs(aHit->entry().z() - aHit->exit().z()) < LHCbMath::lowTolerance) { 
    Warning("Entry and exit at same z - not digitized", StatusCode::SUCCESS, 1);
    return false;
  }

  return true;
}

void MCSTDepositCreator::distributeCharge(const double entryU,
                                          const double exitU, 
                                          std::vector<double>& sites) const
{
  // distribute charge homogeneously at n Sites
  const double deltaU = fabs(exitU - entryU);
  int nSites;
  if ( deltaU <= m_siteSize ) {
    nSites = 1;
  }
  else {
    nSites = GSL_MIN((int)floor(deltaU/m_siteSize) + 1, m_maxNumSites);
  }
  sites.reserve(nSites);

  double startU = entryU;
  double stopU = exitU;
  if (startU>stopU){
    std::swap(startU,stopU);
  }

  const double siteSize = deltaU/nSites;
  int iSite;
  for (iSite = 0; iSite < nSites; ++iSite){
    sites.push_back(startU+((double)iSite+0.5)*siteSize);
  } // iSite
 
}

void MCSTDepositCreator::chargeSharing(const std::vector<double>& sites,
                                       const DeSTSensor* aSensor,
                                       std::map<unsigned int,
                                       double>& stripMap, 
                                       double& possibleCollectedCharge) const 
{
  // init
  const double chargeOnSite = 1.0/((double)sites.size());
  std::vector<double>::const_iterator iterSite = sites.begin();
  double frac = 0.0;
  unsigned int firstStrip;
  unsigned int secondStrip;

  // loop on sites
  while (iterSite != sites.end()){

    // closest strips to site
    firstStrip = aSensor->localUToStrip(*iterSite);
    secondStrip = firstStrip+1;
  
    // do the sharing: first Strip!
    if (aSensor->isStrip(firstStrip)){
      frac = m_chargeSharer->sharing(fabs(*iterSite-aSensor->localU(firstStrip))
                                     /aSensor->pitch());
      stripMap[firstStrip] += frac*chargeOnSite;
      possibleCollectedCharge += frac*chargeOnSite;
    }

    // second strip - if there is one
    if (aSensor->isStrip(secondStrip)){
      frac =m_chargeSharer->sharing(fabs(*iterSite-aSensor->localU(secondStrip))
                                    /aSensor->pitch());
      stripMap[secondStrip] += frac*chargeOnSite;
      possibleCollectedCharge += frac*chargeOnSite;
    }

    ++iterSite;
  } 

  // add entry for strip before first and after last
  std::map<unsigned int,double>::iterator startIter = stripMap.begin();
  std::map<unsigned int,double>::iterator lastIter = stripMap.end();
  --lastIter;

  firstStrip = startIter->first;
  if (aSensor->isStrip(firstStrip-1)){
    stripMap[firstStrip-1] += 0.0;
  }

  const unsigned int lastStrip = lastIter->first;
  if (aSensor->isStrip(lastStrip+1)){
    stripMap[lastStrip+1] += 0.0;
  }
}

double MCSTDepositCreator::beetleResponse(const double time, 
                                          const double capacitance,
                                          const std::string& type)
{ 
  // choose the best spline for our needs...
  ISiAmplifierResponse* bResponse = 0;
  double testCap = 9999.0*Gaudi::Units::picofarad;
  std::vector<ISiAmplifierResponse*>::iterator iter=m_amplifierResponse.begin();
  for ( ; iter != m_amplifierResponse.end(); ++iter) {
    ISiAmplifierResponse::Info properties = (*iter)->validity();
    if ((fabs(properties.capacitance -  capacitance) < testCap ) &&
       (properties.type == type)) {
      testCap = fabs(properties.capacitance - capacitance);
      bResponse = *iter;
    } 
  } // iter
  if (bResponse == 0) warning() << "Failed to match amplifier response"
                                << endmsg;

  return (bResponse !=0 ? bResponse->response(time): 0);
}
