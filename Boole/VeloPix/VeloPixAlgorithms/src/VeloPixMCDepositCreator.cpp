// $Id: VeloPixMCDepositCreator.cpp,v 1.2 2009-12-07 17:37:56 marcin Exp $
// Include files:
// STL
#include <string>
#include <vector>
// Mathcore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// GSL                   
#include "gsl/gsl_math.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/RndmGenerators.h"
// LHCbKernel
#include "Kernel/VeloPixDataFunctor.h"
#include "Kernel/VeloPixChannelID.h"
#include "LHCbMath/LHCbMath.h"
// MCEvent
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Event/MCVeloPixDeposit.h"
// MCInterfaces
#include "MCInterfaces/ISiDepositedCharge.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Boost
#include <boost/foreach.hpp>
#include "boost/assign/list_of.hpp"  
// Local
#include "VeloPixMCDepositCreator.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VeloPixMCDepositCreator
//
// 20/09/2009 : Marcin Kucharczyk
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixMCDepositCreator);

//=============================================================================
// Constructor
//=============================================================================
VeloPixMCDepositCreator::VeloPixMCDepositCreator(const std::string& name, 
                                                 ISvcLocator* pSvcLocator)
  // : GaudiAlgorithm(name, pSvcLocator),
  : GaudiTupleAlg(name, pSvcLocator),
    m_veloPixelDet(0)
{
  declareProperty("InputLocation", m_inputLocation = 
                  "MC/VeloPix/Hits");
  declareProperty("OutputLocation", m_outputLocation = 
                  "MC/VeloPix/Deposits");
  declareProperty("SamplesVector", m_sampleNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
  declareProperty("SpillVector", m_spillNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
  declareProperty("SiteSize", m_siteSize = 0.05 * Gaudi::Units::mm);
  declareProperty("MaxNumSites", m_maxNumSites = 150);
  declareProperty("DepChTool", m_depChTool = "SiGeantDepositedCharge");
  declareProperty("InhomogeneousCharge", m_inhomogenCh = true);
  declareProperty("ChargeUniform", m_chargeUniform = 70.0);
  declareProperty("eVPerElectron", m_eVPerElectron = 3.6);
  declareProperty("DeltaRayMinEnergy", m_deltaRayMinEnergy = 1000.0);
  declareProperty("kT", m_kT = 0.025);
  declareProperty("BiasVoltage", m_biasVoltage = 105.0);
  declareProperty("UseDepTool", m_useDepTool = true);
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixMCDepositCreator::~VeloPixMCDepositCreator(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixMCDepositCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  // Construct input container names
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
  while(iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event" + (*iSpillName) + m_inputLocation);    
    ++iSpillName;
  }
  // Construct output container names
  std::vector<std::string>::const_iterator iSampleName = m_sampleNames.begin();
  while(iSampleName != m_sampleNames.end()) {
    m_outPaths.push_back("/Event" + (*iSampleName) + m_outputLocation);
    ++iSampleName;
  }

  // std::cout << "m_outPaths[]=";
  // for ( iSampleName = m_outPaths.begin(); iSampleName != m_outPaths.end(); ++iSampleName)
  // { std::cout << " " << (*iSampleName); }
  // std::cout << endmsg;

  // Deposited charge 
  m_depositedCharge = tool<ISiDepositedCharge>(m_depChTool,"DepCharge",this);
  // Get detector elements
  m_veloPixelDet = getDet<DeVeloPix>(DeVeloPixLocation::Default);
  // printf("VeloPixMCDepositCreator::initialize()\n");
  // Random number generators
  StatusCode scG = m_gaussDist.initialize(randSvc(),Rndm::Gauss(0.0,1.0));
  StatusCode scU = m_uniformDist.initialize(randSvc(),Rndm::Flat(0.0,1.0));
  if(!(scG && scU)) {
    error() << "Random number initialisation failure" << endmsg;
    return (StatusCode::FAILURE);
  }
  m_diffuseSigma = sqrt(2 * m_kT / m_biasVoltage);

  setHistoTopDir("VeloPix/");

  return StatusCode::SUCCESS;
};

//=========================================================================
//  Main execution
//=========================================================================
StatusCode VeloPixMCDepositCreator::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Output containers put into the store  
  std::vector<LHCb::MCVeloPixDeposits*> depositsVector;
  BOOST_FOREACH(std::string path,m_outPaths) {
    LHCb::MCVeloPixDeposits* depositsContainer = new LHCb::MCVeloPixDeposits();
    depositsContainer->reserve(10000);
    put(depositsContainer,path);
    depositsVector.push_back(depositsContainer);
  }
  // printf("VeloPixMCDepositCreator::execute() => %d spills\n", m_spillPaths.size());
  // Loop over spills
  for(unsigned int iSpill = 0; iSpill < m_spillPaths.size(); ++iSpill) {
    if(exist<LHCb::MCHits>(m_spillPaths[iSpill]) == false) {
      debug() << "Unable to retrieve " + m_spillPaths[iSpill] << endmsg;
    }
    else {
      // printf("VeloPixMCDepositCreator::execute() => process spill %d/%d = %s\n", iSpill, m_spillPaths.size(), m_spillPaths[iSpill].c_str() );
      const LHCb::MCHits* hits = get<LHCb::MCHits>(m_spillPaths[iSpill]);
      createDeposits(hits, depositsVector[iSpill]);
    }
  }
  // Sort by channel
  BOOST_FOREACH(MCVeloPixDeposits* depositsContainer, depositsVector) {
    std::stable_sort(depositsContainer->begin(), depositsContainer->end(),
         VeloPixDataFunctor::Less_by_Channel<const MCVeloPixDeposit*>());
  }
  return StatusCode::SUCCESS;
}


//============================================================================
// Create MCVeloPixDeposits
//============================================================================
void VeloPixMCDepositCreator::createDeposits(
                              const LHCb::MCHits* mcHitContainer, 
                              LHCb::MCVeloPixDeposits*& depositsCont)
{ // printf("VeloPixMCDepositCreator::createDeposits()\n");
  int Count=0;
  // Loop over MChits
  for(LHCb::MCHits::const_iterator iHit = mcHitContainer->begin();    // loop over MC hits
      iHit != mcHitContainer->end(); iHit++) {
    LHCb::MCHit* hit = *iHit;
    // printf(" MCHit: sensDetID=%2d path=%6.1f um dep. energy=%5.1f keV p=%7.1f MeV/c time=%5.3f ns\n",
    //               hit->sensDetID(), 1000*hit->pathLength(), 1000*hit->energy(), hit->p(), hit->time() );
    plot(1000*hit->pathLength(), "PathInSensor",  "VeloPixMCDepositCreator: Path in sensor [um]" , 0.0, 300.0, 60);
    plot(1000*hit->energy(),     "EnergyInSensor","VeloPixMCDepositCreator: Energy deposited in sensor [keV]" , 0.0, 200.0, 40);
    Count+=1;
    // Number of points per pixel to distribute charge on
    int simPoints = simulatedPoints(hit);                              // see into how many parts the path should be divided
    // printf("VeloPixMCDepositCreator::createDeposits() => simPoints=%d\n", simPoints);
    if(simPoints > 0) {
      // Calculate charge for each point + delta ray inhomogeneities
      std::vector<double> sPoints(simPoints);                          // store simulated charge here
      chargeToPoint(hit,sPoints);                                      // simulate charge for every point
      // Diffuse charge from points to pixels
      diffuseCharge(hit,sPoints,depositsCont);                         // diffuse some charge to adjacent pixels
    }
  }
  // printf("VeloPixMCDepositCreator::createDeposits() => %d hits\n", Count);
}

//============================================================================
// Calculate how many points are taken for the simulation in the silicon
//============================================================================
int VeloPixMCDepositCreator::simulatedPoints(LHCb::MCHit* hit)
{
  if(m_isDebug) debug() << " ==> simulatedPoints() " << endmsg;
  double nrPoints = 0.0;
  double path = hit->pathLength();
  nrPoints = ceil(path / m_siteSize);
  if(nrPoints > m_maxNumSites) nrPoints = double(m_maxNumSites);
  // printf(" VeloPixMCDepositCreator::simulatedPoints() => path=%5.3f nrPoints=%3.1f\n", path, nrPoints);
  return int(nrPoints);
}

//============================================================================
// Allocate charge deposit to each point
//============================================================================
void VeloPixMCDepositCreator::chargeToPoint(LHCb::MCHit* hit, 
                                            std::vector<double>& simPoints) 
{
  if(m_isDebug) debug() << " ==> chargeToPoint() " << endmsg;
  double charge;
  if(m_useDepTool) {
    charge = m_depositedCharge->charge(hit);
  } else {
    charge = (hit->energy() / Gaudi::Units::eV) / m_eVPerElectron;
  }
  double chargeE;
  // Some charge allocated by delta ray algorithm
  if(m_inhomogenCh) {
    double thickSi = 
           m_veloPixelDet->sensor(hit->sensDetID())->siliconThickness();
    chargeE = m_chargeUniform * thickSi / Gaudi::Units::micrometer;
    if(chargeE > charge) chargeE = charge;
  } else {
    chargeE = charge;
  }
  // Divide charge equally
  double chargeEN = chargeE / static_cast<double>(simPoints.size());
  // Gaussian fluctuations
  double fluct = 0.0;
  std::vector<double>::iterator iP;
  for(iP = simPoints.begin(); iP != simPoints.end(); ++iP) {
    if(m_inhomogenCh) {
      fluct = m_gaussDist() * sqrt(fabs(chargeEN));
    }
    *iP = chargeEN + fluct;
  }
  // Charge from delta rays
  if(m_inhomogenCh) {
    deltaRayCh(charge-chargeE,0.001*charge,simPoints);
    double totalCh = 0.0;
    for(iP = simPoints.begin(); iP != simPoints.end(); ++iP) {
      totalCh += (*iP);
    }
    if(fabs(totalCh) > 1.e-8) {
      double adjust = charge / totalCh;
      for(iP = simPoints.begin(); iP != simPoints.end(); ++iP) {
      	(*iP) *= adjust;
      }
    }
  }
  // printf("VeloPixMCDepositCreator::chargeToPoint() =>");
  double totalCharge = 0.0;
  for(iP = simPoints.begin(); iP != simPoints.end(); ++iP) {
    totalCharge += (*iP);
    // printf(" %3.1f", (*iP));                                         // print charge simulated for every point
  }
  // printf(" [%3.1f]\n", totalCharge);                                 // print charge for the whole path in silicon
  plot(totalCharge, "DepositPerHit","VeloPixMCDepositCreator: Charge deposited in sensor [e]" , 0.0, 40000.0, 80);
  if(fabs(totalCharge - charge) > 1.e-6) {
    Warning("Normalization problems");
  }
  return;
}

//=============================================================================
// Allocate remaining charge from delta ray distribution
//=============================================================================
void VeloPixMCDepositCreator::deltaRayCh(double charge, double frCh,
			                 std::vector<double>& simPoints) 
{
  if(m_isDebug) debug() << " ==> deltaRayCh() " << endmsg;
  // Upper limit for charge of delta ray
  double maxDRC = charge;
  // Lower limit for charge of delta ray
  double minDRC= m_deltaRayMinEnergy / m_eVPerElectron;
  if(frCh < minDRC * 2.0) frCh = minDRC * 2.0;
  // Amount of charge left to allocate
  while(maxDRC > frCh) {
    // Generate delta ray energy (dN/DE = k * 1 / E^2) for relativistic 
    // incident particle
    // E(r) = 1 / r, where r is uniform in range 1 / minDRC < r < 1 / maxDRC
    // maxDRC bounded by energy left to allocate
    double chargeDR = ranomDRTail(minDRC,maxDRC);
    int ipt = int(LHCb::Math::round(m_uniformDist() * (simPoints.size() - 1)));
    simPoints[ipt] += chargeDR;
    maxDRC -= chargeDR;
  }
  return;
}

//=============================================================================
// Delta ray tail random numbers
//=============================================================================
double VeloPixMCDepositCreator::ranomDRTail(double minDRC, double maxDRC) 
{
  if(m_isDebug) debug() << " ==> randomDRTail() " << endmsg;
  double range = (1.0 / minDRC) - (1.0 / maxDRC);
  double offset = 1.0 / maxDRC;
  double uniform = m_uniformDist() * range + offset;
  double charge = 1.0 / uniform;
  return charge;
}

//=============================================================================
// Diffuse charge from points to pixels & create deposits
//=============================================================================
void VeloPixMCDepositCreator::diffuseCharge(LHCb::MCHit* hit,          // MC hit
                              std::vector<double>& simPoints,          // ionization amounts simulated for this MC hit
                              LHCb::MCVeloPixDeposits*& depositCont)   // storage for pixel deposits
{
  if(m_isDebug) debug() << " ==> diffuseCharge() " << endmsg;
  // Distance between steps on path
  Gaudi::XYZVector path = hit->displacement();                                                 // 3-D distance the particle traveled inside the sensor
  path /= static_cast<double>(simPoints.size() * 2);                                           // path = 3-D step through the ionization points
  Gaudi::XYZPoint pnt = hit->entry() + path;                                                   // first 3-D point where ionization is simulated
  // printf(" => [%+8.3f,%+8.3f,%+9.3f] + %d x [%+6.3f,%+6.3f,%+6.3f]\n",
  //           pnt.x(), pnt.y(), pnt.z(), simPoints.size()-1, path.x(), path.y(), path.z());
  const DeVeloPixSensor* sensor = m_veloPixelDet->sensor(hit->sensDetID());                    // sensor that was hit
  // printf(" => DeVeloPixSensor(%02d): %3.1f um, chipLxW: %4.1fx%4.1f mm, %dx%d pix. pixelHxL: %1.0fx%1.0f um\n",
  //          hit->sensDetID(), 1000*sensor->siliconThickness(),
  //          sensor->chipLength(), sensor->chipWidth(),
  //          sensor->nPixCol(), sensor->nPixRow(),
  //          1000*sensor->hpSize(), 1000*sensor->lpSize());
  double thickSi = sensor->siliconThickness() / Gaudi::Units::micrometer;                      // sensor thickness in um
  thickSi = thickSi / 1000.0;                                                                  // sensor thickness in mm again ?
  double diffuseZ = thickSi;                                                                   // diffusion distance
  // Assume pixels to be at opposite side of Si to entry point
  double dz = diffuseZ / static_cast<double>(simPoints.size() * 2);                            // diffusion distance step for the loop
  diffuseZ -= dz;
  std::vector<double>::iterator iP;
  for(iP = simPoints.begin(); iP != simPoints.end(); ++iP, pnt+=2*path, diffuseZ-=2*dz ) {     // loop over ionization points
    // printf(" simPoint[%d]:\n", iP-simPoints.begin());
    std::pair<double,double> EntryFraction;
    StatusCode channelValid;
    LHCb::VeloPixChannelID entryChannel;
    channelValid = sensor->pointToChannel(pnt,entryChannel,EntryFraction);                     // which pixel is this ?
    if(!channelValid) {
      plot2D(pnt.x(), pnt.y(), "DeadSensorArea", "VeloPixMCDepositCreator: Dead sensor area [mm]",
             -40.0, 40.0, -40.0, 40.0, 160, 160);
      // Warning("pointToChannel() failure");
      // printf(" => pointToChannel() failed\n");
      continue; }
      plot2D(pnt.x(), pnt.y(), "ActiveSensorArea", "VeloPixMCDepositCreator: Active sensor area [mm]",
             -40.0, 40.0, -40.0, 40.0, 160, 160);
    // printf(" => entryChannel @ [%02d:%c, %02d:%03dx%03d]\n",
    //       entryChannel.station(), entryChannel.sidepos()?'R':'L',
    //       entryChannel.chip(), entryChannel.pixel_lp(), entryChannel.pixel_hp() );
    double diffuseSig = m_diffuseSigma * sqrt(thickSi * diffuseZ);
    std::vector<LHCb::VeloPixChannelID> neighbsVec;
    StatusCode channelsValid;
    channelsValid = sensor->pointTo3x3Channels(pnt,neighbsVec);
    if(!channelsValid) { Warning("pointTo3x3Channels failure");
                         // printf(" => pointTo3x3Channels() failed\n");
                         continue; }
    // printf(" => neighbsVec[%d]\n", neighbsVec.size());
    for(std::vector<LHCb::VeloPixChannelID>::iterator                  // loop over center and neib. pixel
        ic = neighbsVec.begin(); ic != neighbsVec.end(); ++ic ) {
      LHCb::VeloPixChannelID channel = *ic;                            // every channel = pixel
      Gaudi::XYZPoint midPoint;
      StatusCode pointValid;
      pointValid = sensor->channelToPoint(channel,midPoint);           // mid-point of given pixel
      if(!pointValid) { Warning("channelToPoint failure"); 
                        // printf(" => sensor->channelToPoint() failed\n");
                        continue; }
      std::pair<double,double> pixSize;
      pixSize = sensor->PixelSize(channel);                            // size of given pixel
      double sizeX = pixSize.first / 2.0;
      double sizeY = pixSize.second / 2.0;
      double xdL = fabs(pnt.x() - midPoint.x()) - sizeX;
      double xdR = fabs(pnt.x() - midPoint.x()) + sizeX;
      double ydL = fabs(pnt.y() - midPoint.y()) - sizeY;
      double ydR = fabs(pnt.y() - midPoint.y()) + sizeY;
      if(entryChannel == channel) {
        double chDepCentr = 0.0;
        chDepCentr = (safe_gsl_sf_erf_Q(xdL / diffuseSig) +
		                  safe_gsl_sf_erf_Q(xdR / diffuseSig)) *
                     (safe_gsl_sf_erf_Q(ydL / diffuseSig) +
		                  safe_gsl_sf_erf_Q(ydR / diffuseSig)) * (*iP);
        MCVeloPixDeposit* newDeposit = 
                          new MCVeloPixDeposit(chDepCentr,channel,hit);
        depositCont->insert(newDeposit);
        // printf("%9.3fe @ [%02d:%c, %02d:%03dx%03d]\n", chDepCentr,
        //        channel.station(), channel.sidepos()?'R':'L',
        //        channel.chip(), channel.pixel_lp(), channel.pixel_hp() );
      } else {
        double chDepNeigh = 0.0;
	      double xdMin = GSL_MIN_DBL(xdL,xdR);
	      double xdMax = GSL_MAX_DBL(xdL,xdR);
	      double ydMin = GSL_MIN_DBL(ydL,ydR);
	      double ydMax = GSL_MAX_DBL(ydL,ydR);
        chDepNeigh = (safe_gsl_sf_erf_Q(xdMin / diffuseSig) -
	                    safe_gsl_sf_erf_Q(xdMax / diffuseSig)) *
                     (safe_gsl_sf_erf_Q(ydMin / diffuseSig) -
		                  safe_gsl_sf_erf_Q(ydMax / diffuseSig)) * (*iP);
        // plot(chDepNeigh, "DepositPerPixel","VeloPixMCDepositCreator: Charge deposited in pixel [e]" , 1.0, 25001.0, 50);
        const double minDeposit = 0.001;                  // [e] avoid storing smaller deposits
        if(chDepNeigh>=minDeposit)                        // added by Pawel Jalocha: to avoid very many deposits of very tiny charge.
        { MCVeloPixDeposit* newDeposit =     
                          new MCVeloPixDeposit(chDepNeigh,channel,hit);
          depositCont->insert(newDeposit);
          // printf("%9.3fe @ [%02d:%c, %02d:%03dx%03d]\n", chDepNeigh,
          //      channel.station(), channel.sidepos()?'R':'L',
          //      channel.chip(), channel.pixel_lp(), channel.pixel_hp() );
        }
      }
    }
    // following two lines moved inside for( ; ; )
    // pnt += 2 * path;
    // diffuseZ -= 2.0 * dz;
  }

  return;
}


//============================================================================
StatusCode VeloPixMCDepositCreator::finalize() {

  return GaudiAlgorithm::finalize();

}
