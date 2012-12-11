// GSL 
#include "gsl/gsl_math.h"

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// LHCbKernel
#include "Kernel/STChannelID.h"
#include "Kernel/STDataFunctor.h"

#include "LHCbMath/LHCbMath.h"
#include "Kernel/ISiAmplifierResponse.h"
#include "MCInterfaces/ISiDepositedCharge.h"


// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"


// local
#include "ISTChargeSharingTool.h"
#include "MCSTDepositCreator.h"

#include <boost/foreach.hpp>
#include "boost/assign/list_of.hpp"

#include "Kernel/ILHCbMagnetSvc.h"

using namespace LHCb;
using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY( MCSTDepositCreator )


#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#define init_vect1(A) {A}
#define init_vect2(A, B) {A, B}
#define init_vect5(A, B, C, D ,E) {A, B, C, D, E}
#else
#define init_vect1(A) boost::assign::list_of(A)
#define init_vect2(A, B) boost::assign::list_of(A)(B)
#define init_vect5(A, B, C, D, E) boost::assign::list_of(A)(B)(C)(D)(E)
#endif

MCSTDepositCreator::MCSTDepositCreator( const std::string& name, 
                                        ISvcLocator* pSvcLocator ) :
  ST::AlgBase(name, pSvcLocator)
{
  m_spillNames.push_back("/");
  m_spillTimes.push_back(0.*Gaudi::Units::ns);

  declareProperty("TofVector", m_tofVector, "vector of flight times");
  declareProperty("SamplesVector", m_sampleNames = init_vect1("/"));
  declareProperty("SampleTimes", m_sampleTimes = init_vect1(0.));
  declareProperty("SpillVector", m_spillNames = init_vect5("/", "/Prev/", "/PrevPrev/", "/Next/", "/LHCBackground/"));
  declareProperty("SpillTimes", m_spillTimes = init_vect5(0.0, -25.0, -50.0, 25.0, -3.3));
  declareProperty("MinDist", m_minDistance = 5.0e-3*Gaudi::Units::mm);

  declareProperty("ChargeSharerName",m_chargeSharerName ="STChargeSharingTool");
  declareProperty("ChargeSharerTypes",m_chargeSharerTypes );

  declareProperty("DepChargeTool", m_depChargeToolName = "SiDepositedCharge");

  declareProperty("SiteSize", m_siteSize = 0.001*Gaudi::Units::mm);
  declareProperty("MaxNumSites", m_maxNumSites = 150);

  declareProperty("XTalkParamsRightEven", m_xTalkParamsRightEven = 
                  init_vect2(0.0503, 0.001754/picofarad));
  declareProperty("XTalkParamsLeftEven", m_xTalkParamsLeftEven = 
                  init_vect2(0.0185, 0.001625/picofarad));
  declareProperty("XTalkParamsRightOdd", m_xTalkParamsRightOdd = 
                  init_vect2(0.0215, 0.001824/picofarad));
  declareProperty("XTalkParamsLeftOdd", m_xTalkParamsLeftOdd = 
                  init_vect2(0.0494, 0.001416/picofarad));

  declareProperty("Scaling", m_scaling = 1.0);
  declareProperty("ApplyScaling", m_applyScaling = true );
  declareProperty("ResponseTypes", m_beetleResponseTypes);
  declareProperty("useStatusConditions", m_useStatusConditions = true);
  declareProperty("useSensDetID", m_useSensDetID = false);

  declareProperty("pMin", m_pMin = 1e-4 *Gaudi::Units::MeV);


  declareProperty("ApplyLorentzCorrection", m_applyLorentzCorrection = true);
  declareProperty("LorentzFactor", m_lorentzFactor = 0.025/Gaudi::Units::tesla);

  m_inputLocation = MCHitLocation::TT; 
  m_outputLocation = MCSTDepositLocation::TTDeposits;

  addToFlipList(&m_inputLocation);
  addToFlipList(&m_outputLocation);


  setForcedInit();
}
#undef init_vect1
#undef init_vect2
#undef init_vect5

MCSTDepositCreator::~MCSTDepositCreator() 
{
  //destructer
}

StatusCode MCSTDepositCreator::initialize() 
{
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // charge sharing tool
  std::vector<std::string>::const_iterator iterCSType = 
    m_chargeSharerTypes.begin();
  for( ; iterCSType != m_chargeSharerTypes.end(); ++iterCSType ) {
    std::string chargeSharerName = m_chargeSharerName + (*iterCSType);
    ISTChargeSharingTool* chargeSharer = 
      tool<ISTChargeSharingTool>(m_chargeSharerName, chargeSharerName, this );
    m_chargeSharer.push_back( chargeSharer );
  }
  
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

  std::vector<std::string>::const_iterator iSampleName = m_sampleNames.begin() ;
  while (iSampleName != m_sampleNames.end()){
    // path in Transient data store
    m_outPaths.push_back("/Event"+(*iSampleName)+m_outputLocation);
    ++iSampleName;
  }

  if (m_applyLorentzCorrection == true) 
    m_fieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);

  return StatusCode::SUCCESS;
}

StatusCode MCSTDepositCreator::execute() 
{
  // make output containers and put them in the store  
  std::vector<MCSTDeposits*> depositsVec;
  BOOST_FOREACH(std::string path, m_outPaths) {
    MCSTDeposits* depositsCont = new MCSTDeposits();
    depositsCont->reserve(10000);
    put (depositsCont, path);
    depositsVec.push_back(depositsCont);
  } // for each
 
  // loop over spills
  for (unsigned int iSpill = 0; iSpill < m_spillPaths.size(); ++iSpill){

    if (exist<MCHits>(m_spillPaths[iSpill]) == false){   
      // failed to find hits
      if( msgLevel(MSG::DEBUG) )
        debug() << "Unable to retrieve " + m_spillPaths[iSpill] << endmsg;
    }
    else {
      // found spill - create digitizations and add them to deposits container
      const MCHits* hits = get<MCHits>(m_spillPaths[iSpill]);
      createDeposits(hits,m_spillTimes[iSpill],depositsVec);
    }
  } // iSpill

  // sort by channel
  BOOST_FOREACH(MCSTDeposits* depositsCont, depositsVec){ 
    std::stable_sort(depositsCont->begin(), depositsCont->end(),
                     STDataFunctor::Less_by_Channel<const MCSTDeposit*>());
  } // for each

  return StatusCode::SUCCESS;
}
 

void MCSTDepositCreator::createDeposits(const MCHits* mcHitsCont,
                                        const double spillTime, 
                                        std::vector<MCSTDeposits*>& depositCont)
{
  // loop over MChits
  MCHits::const_iterator iterHit = mcHitsCont->begin();
  for (; iterHit!=mcHitsCont->end();++iterHit){    
    MCHit* aHit = *iterHit;

    DeSTSector* aSector = tracker()->findSector(aHit->midPoint());
    if (aSector == 0) {
      if( msgLevel(MSG::DEBUG) )
        debug() << "point " << aHit->midPoint() << endmsg;
      Warning("Failed to find sector", StatusCode::SUCCESS, 1).ignore();
      continue;
    }

    if (hitToDigitize(aHit) == true){           
      
      if (m_useStatusConditions && aSector->sectorStatus() == DeSTSector::Dead )
        continue;

      // find the sensor
      const Gaudi::XYZPoint globalMidPoint = aHit->midPoint();
      DeSTSensor* aSensor = aSector->findSensor(globalMidPoint);
      if (aSensor == 0){
        Warning("Failed to find sensor", StatusCode::SUCCESS, 1).ignore();
        continue;
      } 

      // from now on work in local sensor frame
      Gaudi::XYZPoint entryPoint = aSensor->toLocal( aHit->entry() );
      Gaudi::XYZPoint exitPoint = aSensor->toLocal( aHit->exit() );
      const Gaudi::XYZPoint midPoint = aSensor->toLocal(globalMidPoint);

      if (m_applyLorentzCorrection == true) lorentzShift(aSensor,globalMidPoint,
                                                         entryPoint, exitPoint);
    
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

          STChannelID elemChan = aSector->elementID();

          // loop over strips and simulate capacitive coupling
          unsigned int firstStrip = (stripMap.begin())->first;
          unsigned int lastStrip = (--stripMap.end())->first;
          unsigned int iStrip = firstStrip;
          for ( ; iStrip <= lastStrip; ++iStrip ) {

            // Determine whether Tell1 neighbour(!) channel is even or odd
            bool tell1ChanEven = ((iStrip%2 == 1) == aSensor->xInverted());

            // Determine cross talk level for this readout sector
            double xTalkRight = 0.0;
            double xTalkLeft  = 0.0;
            if( tell1ChanEven ) {
              xTalkRight = m_xTalkParamsRightEven[0] + 
                m_xTalkParamsRightEven[1]*aSector->capacitance();
              xTalkLeft = m_xTalkParamsLeftEven[0] + 
                m_xTalkParamsLeftEven[1]*aSector->capacitance();
            } else {
              xTalkRight = m_xTalkParamsRightOdd[0] + 
                m_xTalkParamsRightOdd[1]*aSector->capacitance();
              xTalkLeft = m_xTalkParamsLeftOdd[0] + 
                m_xTalkParamsLeftOdd[1]*aSector->capacitance();
            }

            // Get the charge of the previous and next strips
            double prevCharge = 0.0;
            if (iStrip != firstStrip) prevCharge = stripMap[iStrip-1];
            double nextCharge = 0.0;
            if (iStrip != lastStrip) nextCharge = stripMap[iStrip+1];
            double leftCharge  = aSensor->xInverted() ? nextCharge : prevCharge;
            double rightCharge = aSensor->xInverted() ? prevCharge : nextCharge;

            // Capacitive coupling
            double weightedCharge = (1.-xTalkLeft -xTalkRight)*stripMap[iStrip]
              + xTalkRight*leftCharge + xTalkLeft*rightCharge ; 

            // Do not store deposits which have too small charge
            if( weightedCharge < 1e-3 ) continue;

            // Scaling for the number of electrons (JvT: do we need this?)
            const double scaling = m_applyScaling ?
              m_scaling * (1.0 + xTalkLeft + xTalkRight ) : 1.0;
            
            // amplifier response - fraction of charge it sees
            for (unsigned int iTime = 0; iTime < m_outPaths.size(); ++iTime) {
              double beetleFrac;          
              double samplingTime = m_sampleTimes[iTime];
              if (iStrip != firstStrip && iStrip != lastStrip){
                beetleFrac =beetleResponse(m_tofVector[elemChan.station()-1]
                                           - aHit->time() - spillTime 
                                           + samplingTime, 
                                           aSector->capacitance(),
                                           SiAmpliferResponseType::signal);
              }
              else {
                beetleFrac =beetleResponse(m_tofVector[elemChan.station()-1]
                                           - aHit->time() - spillTime 
                                           + samplingTime,
                                           aSector->capacitance(),
                                           SiAmpliferResponseType::capCoupling);
              }

              STChannelID aChan = aSector->stripToChan(iStrip);

              if ( m_useStatusConditions == false || 
                   aSector->isOKStrip(aChan) == true) {
                const double electrons = ionization * beetleFrac * scaling
                  * weightedCharge / totWeightedCharge;
                
                const double adcCounts = aSector->toADC(electrons, aChan);
                MCSTDeposit* newDeposit = new MCSTDeposit(adcCounts,aChan,aHit);
                depositCont[iTime]->insert(newDeposit);
              } // ok strip
            } // loop sampling times
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
  if (aHit->p() < m_pMin){
    Warning( "Hit with zero p - not digitized", StatusCode::SUCCESS,1).ignore();
    return false;
  }

  // check if entry and exit point are at the same z-position
  if (fabs(aHit->entry().z() - aHit->exit().z()) < LHCb::Math::lowTolerance) { 
    Warning("Entry and exit at same z - not digitized", 
            StatusCode::SUCCESS, 1).ignore();
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
                                       std::map<unsigned int, double>& stripMap,
                                       double& possibleCollectedCharge) 
{
  // init
  const double chargeOnSite = 1.0/((double)sites.size());
  std::vector<double>::const_iterator iterSite = sites.begin();
  double frac = 0.0;
  unsigned int firstStrip;
  unsigned int secondStrip;

  // Find the appropriate charge sharing tool
  ISTChargeSharingTool* chargeSharer = 0;
  double deltaT = 1E10*m;
  std::vector<ISTChargeSharingTool*>::iterator iter = m_chargeSharer.begin();
  for( ; iter != m_chargeSharer.end(); ++iter ) {
    if( fabs((*iter)->thickness() - aSensor->thickness() ) < deltaT ) {
      deltaT = fabs((*iter)->thickness() - aSensor->thickness()) ;
      chargeSharer = *iter;
    }
  }

  // loop on sites
  while (iterSite != sites.end()){

    // closest strips to site
    firstStrip = aSensor->localUToStrip(*iterSite);
    secondStrip = firstStrip+1;
  
    // do the sharing: first Strip!
    if (aSensor->isStrip(firstStrip)){
      frac = chargeSharer->sharing(fabs(*iterSite-aSensor->localU(firstStrip))
                                   /aSensor->pitch());
      stripMap[firstStrip] += frac*chargeOnSite;
      possibleCollectedCharge += frac*chargeOnSite;
    }

    // second strip - if there is one
    if (aSensor->isStrip(secondStrip)){
      frac = chargeSharer->sharing(fabs(*iterSite-aSensor->localU(secondStrip))
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

void MCSTDepositCreator::lorentzShift(const DeSTSensor* sensor,
                                      const Gaudi::XYZPoint& midPoint,
                                      Gaudi::XYZPoint& entry,  
                                      Gaudi::XYZPoint& exit ) const 
{
  // Get the local By
  Gaudi::XYZVector field = m_fieldSvc->fieldVector( midPoint ) ;
  Gaudi::XYZVector normalY( sensor->globalPoint(0,1,0) - 
                            sensor->globalPoint(0,0,0) );
  double localBy = field.Dot( normalY.Unit() );

  // Calculate the Lorentz shift in local x.
  const double dz = exit.z() - entry.z();
  double dx = fabs(dz) * localBy * m_lorentzFactor;

  // Apply a small tilt to particle trajectory by shifting either the entry or
  // exit point
  if( dz > 0 ) {
    exit.SetX(exit.x() + dx);
  } else {
    entry.SetX(entry.x() + dx);
  }
  
}
