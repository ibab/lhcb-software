// Include files
// STL
#include <string>
#include <vector>

// Mathcore
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

// from LHCbKernel
#include "LHCbMath/LHCbMath.h"
#include "Kernel/VeloEventFunctor.h"
#include "Kernel/ISiAmplifierResponse.h"
#include "IRadDamageTool.h"

// from MCEvent
#include "Event/MCHit.h"
#include "Event/MCVeloFE.h"
#include "Event/MCParticle.h"

// from MCInterfaces
#include "MCInterfaces/ISiDepositedCharge.h"

// VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "VeloSim.h"
#include "VeloChargeThreshold.h"

//------------------------------------------------------------
// Implementation file for class : VeloSim
//
// 16/01/2002 : Chris Parkes, update Tomasz Szumlak
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloSim )


//===========================================================================
// Standard creator, initializes variables
//===========================================================================
VeloSim::VeloSim( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_MCHitContainerToLink(NULL),
    m_veloDet ( 0 ),
    m_FEs(NULL),
    m_FEs_coupling(NULL),
    m_thresholdADC(0.),
    m_threshold(0.),
    m_thresholdADCSingle(0.),
    m_thresholdSingle(0.),
    m_noiseTailProb(0.),
    m_noiseTailProbSingle(0.),
    m_baseDiffuseSigma(0.),
    m_SiTimeTool(NULL),
    m_depositedCharge(NULL),
    m_totalFEs( 0 ),
    m_killedFEsRandom( 0 ),
    m_killedFEsBadStrips( 0 ),
    m_radTool(NULL),
    m_isDebug(false),
    m_isVerbose(false)
{
  declareProperty("InputContainers", m_inputContainers );
  declareProperty("InputContainerToLink", m_MCHitContainerToLinkName );
  declareProperty("InputTimeOffsets", m_inputTimeOffsets );
  declareProperty("OutputContainers", m_outputContainers );
  declareProperty("SimNoisePileUp", m_simNoisePileUp = false);
  declareProperty("InhomogeneousCharge", m_inhomogeneousCharge = true );
  declareProperty("Coupling", m_coupling = true );
  declareProperty("NoiseSim", m_noiseSim = true );
  declareProperty("PedestalSim", m_pedestalSim = true );
  declareProperty("CMSim", m_CMSim = true );
  declareProperty("StripInefficiency", m_stripInefficiency = 0.0 );
  declareProperty("ThresholdADC", m_thresholdADC = 4.0 );  
  declareProperty("ThresholdADCSingle", m_thresholdADCSingle = 10.0 );  
  declareProperty("kT", m_kT = 0.025 );
  declareProperty("BiasVoltage", m_biasVoltage = 150. );
  declareProperty("eVPerElectron", m_eVPerElectron = 3.6 );
  declareProperty("SimulationPointsPerStrip", m_simulationPointsPerStrip = 3 );
  declareProperty("ChargeUniform", m_chargeUniform = 70. );
  declareProperty("DeltaRayMinEnergy", m_deltaRayMinEnergy = 1000. );
  declareProperty("CapacitiveCoupling", m_capacitiveCoupling = 0.01 );
  declareProperty("AverageStripNoise", m_averageStripNoise = 1.85 );
  // In VeloTell1DataProcessor  ADC in e- = (ADC_max)/(e_max)
  // 76950/128 = 601
  // tuned to 2009 data
  declareProperty("ElectronsPerADC", m_electronsPerADC = 601.);
  declareProperty("OffPeakSamplingTime", m_offPeakSamplingTime = 0. );
  declareProperty("MakeNonZeroSuppressedData",
		  m_makeNonZeroSuppressedData=false );
  declareProperty("PedestalConst", m_pedestalConst=512);
  declareProperty("PedestalVariation", m_pedestalVariation=0.05);
  declareProperty("DepChargeTool", 
		  m_depChargeToolType = "SiDepositedCharge");
  declareProperty("UseDepTool", m_useDepTool = true);
  declareProperty("SiAmplifierResponse",     
		  m_SiTimeToolType = "SiAmplifierResponse");
  declareProperty("PulseShapePeakTime",m_pulseShapePeakTime=30.7848);
  declareProperty("NoiseScale" , m_noiseScale =1.);
  declareProperty("FluctuationsScale" , m_scaleFluctuations = 1. );

  declareProperty("RadDamageToolName", m_radToolType="VeloRadDamageTool" );

  declareProperty("IntraSensorCrossTalk",m_intraSensorCrossTalk=true);
  declareProperty("M2SinglePoint",m_m2SinglePoint=true);

  Rndm::Numbers m_gaussDist;
  Rndm::Numbers m_uniformDist;
}

//===========================================================================
// Destructor
//===========================================================================
VeloSim::~VeloSim() {}
//===========================================================================
// Initialisation. Check parameters
//===========================================================================
StatusCode VeloSim::initialize() {
  //
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_isDebug   = msgLevel( MSG::DEBUG   );
  m_isVerbose = msgLevel( MSG::VERBOSE );

  if(m_isDebug) debug() << "==> Initialise" << endmsg;

  if( m_inputContainers.size() != m_inputTimeOffsets.size() ){
    Error("Size of InputContainters and InputTimeOffsets  do not match");
    return StatusCode::FAILURE;
  }

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  m_baseDiffuseSigma=sqrt(2*m_kT/m_biasVoltage);

  // random number initialisation
  StatusCode sc1=m_gaussDist.initialize( randSvc(), Rndm::Gauss(0.,1.0));
  StatusCode sc2=m_uniformDist.initialize( randSvc(), Rndm::Flat(0.,1.0));
  if(!( sc1 && sc2  )){
    error() << "Random number init failure" << endmsg;
    return (StatusCode::FAILURE);
  }

  // Front end time tool
  m_SiTimeTool = tool<ISiAmplifierResponse>(m_SiTimeToolType,
					    "SiTimeTool",this);

  // deposited charge 
  m_depositedCharge = tool<ISiDepositedCharge>(m_depChargeToolType,
					       "DepCharge",this);

  // convert threshold into electrons
  m_threshold = m_thresholdADC*m_electronsPerADC;
  m_thresholdSingle = m_thresholdADCSingle*m_electronsPerADC;
  // precalulate the probability of adding noise to a strip
  m_noiseTailProb = safe_gsl_sf_erf_Q(m_threshold/ 
		      (m_averageStripNoise*m_noiseScale*m_electronsPerADC));
  info() <<"Probability to add noise to empty strips " << m_noiseTailProb
	 << endmsg;
  // precalulate the probability of adding noise to an isolated strip
  m_noiseTailProbSingle = 
    safe_gsl_sf_erf_Q(m_thresholdSingle/ 
                      (m_averageStripNoise*m_noiseScale*m_electronsPerADC));
  info() <<"Probability to add noise to isolated empty strips " 
         << m_noiseTailProbSingle << endmsg;
  std::vector<DeVeloSensor*>::const_iterator sens = 
    m_veloDet->rPhiSensorsBegin();
  int maxStrips= (*sens)->numberOfStrips();
  double hitNoiseAverage= 2.*m_noiseTailProb*maxStrips;
  StatusCode sc3 = m_poissonDist.initialize(randSvc(), 
					    Rndm::Poisson(hitNoiseAverage));
  if(!sc3){
    error() << "Random number init failure" << endmsg;
    return sc3;
  }

  m_radTool =   // make the radiation tools required
    tool<IRadDamageTool>(m_radToolType,"RadDamage",this);

  return StatusCode::SUCCESS;
}
//===========================================================================
// Main execution
//===========================================================================
StatusCode VeloSim::execute() {

  if(m_isDebug) debug() << "==> Execute" << endmsg;

  // get the pointer to the container of MCHits to make MC linker table to
  m_MCHitContainerToLink = get<LHCb::MCHits>(m_MCHitContainerToLinkName);
  if(m_isVerbose) verbose() << "m_MCHitContainerToLinkName " 
			    << m_MCHitContainerToLinkName
			    << " m_MCHitContainerToLink " 
			    << m_MCHitContainerToLink
			    << " " << m_MCHitContainerToLink->name() 
			    << endmsg;

  m_FEs = new LHCb::MCVeloFEs();

  std::vector<std::string>::const_iterator iCont;
  for ( iCont = m_inputContainers.begin() ; 
	iCont != m_inputContainers.end() ; ++ iCont ){
    unsigned int timeIndex = iCont - m_inputContainers.begin();
    
    LHCb::MCHits * veloHits=getIfExists<LHCb::MCHits>( *iCont );
    if( NULL == veloHits ){
      if(m_isDebug) debug() << "Could not find container " 
			    << *iCont << endmsg;
      continue;
    }
    
    if(m_isDebug) debug()<< "Input " << *iCont
			 << " has " << veloHits->size() 
			 << " MCHits" << endmsg;

    StatusCode sc = simulation(veloHits,m_inputTimeOffsets[timeIndex]);
    if(!sc) return(sc);
  }
  if(m_isDebug) debug() << "let's go to the output data" << endmsg;
  StatusCode sc = storeOutputData(); // add MCFEs to TDS
  //
  return (sc);
}
//============================================================================
StatusCode VeloSim::simulation(LHCb::MCHits * veloHits, double timeOffset) {
  // perform simulation

  // simulate signals in strips from GEANT hits of current event
  chargeSim(veloHits,timeOffset);
  // charge sharing from capacitive coupling of strips
  if (m_coupling) coupling();
  // add noise
  if (m_noiseSim) noiseSim();
  // add pedestals
  if(m_makeNonZeroSuppressedData){
    if (m_pedestalSim) pedestalSim();
    // common mode - not yet implemented
    if (m_CMSim) CMSim();
  }
  // dead strips / channels (always call as dead strips now set in db)
  deadStrips();
  // remove any unwanted elements and sort
  finalProcess();
  //
  return StatusCode::SUCCESS;
}
//=======================================================================
// loop through hits allocating the charge to strips
//=======================================================================
void VeloSim::chargeSim(LHCb::MCHits * hits, double timeOffset) {
  //
  if(m_isDebug) debug() << " ==> chargeSim() " <<endmsg;
  if(m_isVerbose) verbose()
    << "Number of hits to simulate=" << hits->size() << endmsg;
  //loop over input hits
  for(LHCb::MCHits::const_iterator hitIt = hits->begin();
      hits->end() != hitIt ; hitIt++ ){
    LHCb::MCHit* hit = (*hitIt);
    if(!checkConditions(hit)){
      if(m_isVerbose) verbose()<< " the sensor is not read-out" <<endmsg;
      continue;
    }
    // calculate a set of points in the silicon
    //with which the simulation will work
    int NPoints = simPoints(hit);
    if(m_isVerbose) verbose()<< "Simulating " << NPoints
			     << " points in Si for this hit" <<endmsg;
    if (NPoints>0){
      // calculate charge to assign to each point
      // taking account of delta ray inhomogeneities
      std::vector<double> sPoints(NPoints);
      chargePerPoint(*hitIt,sPoints,timeOffset);
      // diffuse charge from points to strips
      diffusion(*hitIt,sPoints);
    }
  }
  if(m_isDebug) debug()<< "Number of MCVeloFEs " << m_FEs->size() <<endmsg;
  //
  return;
}

//=======================================================================
// calculate how many points in the silicon the simulation will be performed at
//=======================================================================
int VeloSim::simPoints(LHCb::MCHit* hit){
  //
  if(m_isDebug) debug()<< " ==> simPoints() " <<endmsg;
  //
  double EntryFraction=0.,ExitFraction=0.;
  double pitch=0.;
  StatusCode EntryValid, ExitValid;
  if(m_isVerbose) verbose()<< "hit entry: " << hit->entry() <<endmsg;
  LHCb::VeloChannelID entryChan, exitChan;
  const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
  double NPoints=0.;
  if(!sens) {
    Error("Invalid sensor from hit");
    return 0;
  }
  EntryValid=sens->pointToChannel(hit->entry(),entryChan,EntryFraction,
				  pitch);
  ExitValid=sens->pointToChannel(hit->exit(),
				 exitChan,ExitFraction,pitch);
  
  if(m_isVerbose) verbose()<< "calculate number of points to simulate in Si"
			   <<endmsg;
  if(m_isVerbose) verbose()
    << "entry/exit points " << entryChan.sensor() << " " << entryChan.strip()
    <<  " + " << EntryFraction << " / " <<  exitChan.sensor()
    << " " << exitChan.strip()  << " + " << ExitFraction <<endmsg;
  if (!EntryValid) if(m_isVerbose) verbose() << " invalid entry point"
					     <<endmsg;
  if (!ExitValid) if(m_isVerbose) verbose() << " invalid exit point"
					    <<endmsg;
  //
  if (EntryValid&&ExitValid){
    // both entry and exit are at valid strip numbers,
    // calculate how many full strips apart
    int INeighb;
    StatusCode sc=sens->channelDistance(entryChan,exitChan,INeighb);
    if (sc) {
      NPoints = fabs(float(INeighb)-(EntryFraction-ExitFraction));
      if(m_isVerbose) verbose()
	<< "Integer number of strips apart " << INeighb
	<< " floating number " << NPoints <<endmsg;
    }else{
      if(m_isVerbose) verbose()
	<< "Entry and Exit Strips not in same sector" <<endmsg;
    }
  }else{
    // either entry or exit or both are invalid, ignore this hit
    NPoints=0.;
    //
    if((EntryValid!=ExitValid)){
      if(m_isVerbose) verbose()
	<< "simPoints: only one of entry and exit point of hit are in "
	<< "silicon - hit ignored " <<endmsg;
    }
  }
  //
  return (int(ceil(NPoints)*m_simulationPointsPerStrip));
}

//=======================================================================
// allocate charge to points
//=======================================================================
void VeloSim::chargePerPoint(LHCb::MCHit* hit,
                             std::vector<double>& Spoints, double timeOffset){
  //
  if(m_isDebug) debug()<< " ==> chargePerPoint() " <<endmsg;
  // total charge in electrons
  double charge;
  if( m_useDepTool ) {
    charge = m_depositedCharge->charge(hit);
  }else{
    charge = (hit->energy()/Gaudi::Units::eV)/m_eVPerElectron;
  }

  // sim radiation damage
  charge *= m_radTool->chargeFrac(*hit);

  if(m_isVerbose) verbose()
    << "Number of electron-hole pairs: " << charge <<endmsg;
  
  charge*=chargeTimeFactor(hit->time(),timeOffset,hit->entry().z());
  if(m_isVerbose) verbose() << "Charge after time correction of " 
			    << timeOffset << " is " << charge << endmsg;

  if(m_intraSensorCrossTalk && 
     m_m2SinglePoint){ // do correction for midpoint only
    LHCb::VeloChannelID chM2;
    const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
    double fracM2Loss =0., fracMain = 1.;
    StatusCode sc = m_radTool->m2CouplingFrac(hit->midPoint(), sens, 
					      chM2, fracM2Loss, fracMain);
    if(sc && fracM2Loss > 0.01 ){ // less than 1% do not bother
      if( m_isVerbose ) {
        verbose() << "Add "  << fracM2Loss << " of charge to M2 line "
                  << chM2 
		  << " remove " << fracMain << " from main strip" << endmsg;
      }
      LHCb::MCVeloFE* myFE = findOrInsertFE(chM2); // no MC link for coupling
      fillFE(myFE,charge*fracM2Loss); // update an unlinked FE
      charge *= (1.-fracMain); // remove charge from main hit point
    }
  }

  // charge to divide equally
  double chargeEqual;
  if(m_inhomogeneousCharge){
    // some of charge allocated by delta ray algorithm
    double thickness=m_veloDet->sensor(hit->sensDetID())->siliconThickness();
    chargeEqual=m_chargeUniform*thickness/Gaudi::Units::micrometer;
    chargeEqual*=chargeTimeFactor(hit->time(),timeOffset,hit->entry().z());
    if(m_isVerbose) verbose() 
      << "ChargeEqual equal after time correction of " << timeOffset
      << " is " << chargeEqual << endmsg;
    if(chargeEqual>charge)  chargeEqual=charge;
  }else{
    // all of charge allocated equally to points
    chargeEqual=charge;
  }
  if(m_isVerbose) verbose()
    << "total charge " << charge
    << " charge for equal allocation " << chargeEqual <<endmsg;
  // divide equally
  double chargeEqualN=chargeEqual/static_cast<double>(Spoints.size());
  std::vector<double>::iterator iPoint;
  for (iPoint = Spoints.begin(); iPoint != Spoints.end(); ++iPoint){
    // gaussian fluctuations
    if (m_inhomogeneousCharge) {
      // formula is normal distribution * sqrt(charge on point) 
      double fluctuate = m_gaussDist()*sqrt(fabs(chargeEqualN));
      // to scale this set m_scaleFluctions > 1
      fluctuate *= m_scaleFluctuations;
      *iPoint = chargeEqualN+fluctuate;
    }
    if(m_isVerbose) verbose()<< "charge for pt" << iPoint-Spoints.begin() 
			     << " is " << *iPoint << endmsg;
  }
  // inhomogeneous charge dist from delta rays
  if(m_inhomogeneousCharge){
    deltaRayCharge(charge-chargeEqual, 0.001*charge, Spoints);
    // ensure total charge is allocated
    double total=0.;
    for (iPoint = Spoints.begin(); iPoint != Spoints.end(); ++iPoint){
      total += (*iPoint);
    }
    if(m_isVerbose) verbose()<< "charge distributed: " << total <<endmsg;
    // normalize the charge each time (assume bigger than 1 electron!)
    if(fabs(total) > 1.e-8){
      double adjust=charge/total;
      for (iPoint = Spoints.begin(); iPoint != Spoints.end(); ++iPoint){
	(*iPoint) *= adjust;
      }
    }
  }
  double totalCharge=0.;
  for (iPoint = Spoints.begin(); iPoint != Spoints.end(); ++iPoint){
    totalCharge+= (*iPoint);
  }
  if(m_isVerbose) verbose()
    << "total charge after correction: " << totalCharge <<endmsg;
  // if distributed charge not equal to charge taken from
  // a hit issue a warning
  if(fabs(totalCharge-charge)>1.e-6){
    Warning("Normalization problems, see debug for information!");
    if(m_isDebug){
      debug()<< "total charge after correction: " << totalCharge <<endmsg;
      debug()<< "total charge from a hit:" << charge <<endmsg;
    }
  }
  //
  return;
}
//=======================================================================
// allocate remaining charge from delta ray distribution
//=======================================================================
void VeloSim::deltaRayCharge(double charge, double tol,
			     std::vector<double>& Spoints){
  //
  if(m_isDebug) debug()<< " ==> deltaRayCharge() " <<endmsg;
  //
  double Tmax= charge;// upper limit on charge of delta ray
  double Tmin= m_deltaRayMinEnergy/m_eVPerElectron;
  // lower limit on charge of delta ray
  if (tol<Tmin*2.) tol=Tmin*2.;
  // amount of charge left to allocate
  while (Tmax>tol){
    // generate delta ray energy
    // dN/DE=k*1/E^2 for relativistic incident particle
    // E(r)=1/r, where r is uniform in range 1/Tmin < r < 1/Tmax
    // but Tmax bounded by energy left to allocate, so following is
    // not truly correct
    double deltaCharge=ran_inv_E2(Tmin,Tmax);
    // choose pt at random to add delta ray
    int ipt=int(LHCb::Math::round(m_uniformDist()*(Spoints.size()-1)));
    //
    if(m_isVerbose) verbose()<< " delta ray charge added to point " << ipt
			     << "/" << Spoints.size() <<endmsg;
    //
    Spoints[ipt]+=deltaCharge;
    Tmax-=deltaCharge;
  }
  return;
}
//=======================================================================
// allocate the charge to the collection strips
//=======================================================================
void VeloSim::diffusion(LHCb::MCHit* hit,std::vector<double>& Spoints){
  //
  if(m_isDebug) debug()<< " ==> diffusion " <<endmsg;
  //
  Gaudi::XYZVector path=hit->displacement();
  path/=static_cast<double>(Spoints.size()*2);// distance between steps on path
  Gaudi::XYZPoint point= hit->entry() + path;
  //
  const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
  double thickness = sens->siliconThickness()/Gaudi::Units::micrometer;
  double ZDiffuse = thickness;
  // assume strips are at opposite side of Si to entry point
  // distance between steps on path
  double dz=ZDiffuse/static_cast<double>(Spoints.size()*2); 
  ZDiffuse-=dz;
  //
  std::vector<double>::iterator iPoint;
  //loop over points on path
  for (iPoint = Spoints.begin() ; iPoint != Spoints.end() ; ++iPoint){ 
    double fraction,pitch;
    //
    if(m_isVerbose) verbose()<< " ipt " << iPoint - Spoints.begin() 
			     << " point " << point <<endmsg;
    //calculate point on path
    LHCb::VeloChannelID entryChan;
    StatusCode valid=sens->pointToChannel(point,entryChan,fraction,pitch);
    if(!valid){
      // a point may be invalid, despite entry and exit points being valid,
      // as it lies in say a bias rail dead area.
      // charge from this point is considered lost
      if(m_isVerbose) verbose()<< " point is not in active silicon " << point
			       << " entry " << hit->entry() << " exit "
			       <<  hit->exit() << " detID " << hit->sensDetID() <<endmsg;
      continue;
    }
    //
    if(m_isVerbose) verbose()<< "chan " << entryChan.strip() << " fraction "
			     << fraction << " pitch " << pitch << " valid "
			     << valid <<endmsg;

    if(m_intraSensorCrossTalk && 
       !m_m2SinglePoint){ // do correction for every point
      LHCb::VeloChannelID chM2;
      double fracM2Loss =0., fracMain = 1.;
      StatusCode sc = m_radTool->m2CouplingFrac(hit->midPoint(), sens, 
                                                chM2, fracM2Loss, fracMain);
      if(sc && fracM2Loss > 0.01 ){ // less than 1% do not bother
        if( m_isVerbose ) {
          verbose() << "Add "  << fracM2Loss << " of charge to M2 line "
                    << chM2 
                    << " remove " << fracMain << " from main strip" << endmsg;
        }
        LHCb::MCVeloFE* myFE = findOrInsertFE(chM2); // no MC link for coupling
        fillFE(myFE,(*iPoint)*fracM2Loss); // update an unlinked FE
        (*iPoint) *= (1.-fracMain); // remove charge from main hit point
      }
    }

    //
    const int neighbs=1; // only consider +/- this many neighbours
    double chargeFraction[2*neighbs+1];
    double totalFraction=0.;
    // loop over neighbours per point
    int iNg;
    for(iNg=-neighbs; iNg<=+neighbs; iNg++){
      double diffuseDist1=((iNg-0.5)-fraction)*pitch/
	Gaudi::Units::micrometer;
      double diffuseDist2=((iNg+0.5)-fraction)*pitch/
	Gaudi::Units::micrometer;
      if(m_isVerbose) verbose()<< "dif1: " << diffuseDist1 << ", dif2: "
			       << diffuseDist2 <<endmsg;

      double diffuseSigma=m_baseDiffuseSigma*sqrt(thickness*ZDiffuse);
      //
      if(m_isVerbose) verbose()
	<< "diffuseDist1 " << diffuseDist1
	<<   " diffuseDist2 " << diffuseDist2 << " diffuseSigma "
	<< diffuseSigma << " base " << m_baseDiffuseSigma
	<< " zdiff " << ZDiffuse <<endmsg;
      //
      double prob1= safe_gsl_sf_erf_Q(diffuseDist1/diffuseSigma);
      double prob2= safe_gsl_sf_erf_Q(diffuseDist2/diffuseSigma);
      //
      if(m_isVerbose) verbose() << " prob1+2 " <<  prob1 
				<< " " << prob2 <<endmsg;
      //
      int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
      chargeFraction[i]=fabs(prob1-prob2);
      totalFraction+= fabs(prob1-prob2);
      //
      if(m_isVerbose) verbose()<< " iNg " << iNg << " cfrac "
			       << chargeFraction[i]  << " tot " 
			       << totalFraction <<endmsg;
    }
    // renormalise allocated fractions to 1., and update strip signals
    for(iNg=-neighbs; iNg<=+neighbs; iNg++ ){
      if( totalFraction < 1e-8 ) continue; // skip if very small fraction
      int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
      //
      if(m_isDebug) debug()<< i << " iNg " << iNg 
			   << " ipt " << iPoint - Spoints.begin()
			   << " " <<endmsg;
      //
      double charge=(*iPoint)*(chargeFraction[i]/totalFraction);
      //
      if(m_isDebug) debug()<< i << " ipt " 
			   << iPoint - Spoints.begin() << " charge "
			   << charge <<endmsg;
      if (charge>m_threshold*0.1){
	// ignore if below 10% of threshold
	// calculate index of this strip
	LHCb::VeloChannelID stripKey;
	valid=sens->neighbour(entryChan,iNg,stripKey);
	//
	if(m_isDebug) debug()<< " neighbour " << entryChan.strip() << " "
			     << stripKey.strip() << " iNg " << iNg <<endmsg;
	// update charge and MCHit list
	if (valid){
	  LHCb::MCVeloFE* myFE = findOrInsertFE(stripKey);
	  if( hit->parent() == m_MCHitContainerToLink ){
	    if( m_isVerbose ) verbose() << "MCHit to link"  << endmsg;
	    fillFE(myFE,hit,charge); // update and add MC link
	  }else{
	    if( m_isVerbose ) verbose() << "Non-linked MCHit"  << endmsg;
            fillFE(myFE,charge); // update an unlinked FE
          }
	}
      }
    } // neighbours loop
    point+=2*path; // update to look at next point on path
    ZDiffuse-=2.*dz;

  } // loop over points
  //
  return;
}
//=======================================================================
// update signal and list of MCHits
//=======================================================================
void VeloSim::fillFE(LHCb::MCVeloFE* myFE,
                     LHCb::MCHit* hit, double charge){
  //
  if(m_isDebug) debug()<< " ==> fillFE() " <<endmsg;
  //
  myFE->setAddedSignal(myFE->addedSignal()+charge);
  if( hit ){
    // add link to MC hit / update with weight
    if(m_isVerbose) verbose() << "fillFE " << myFE->key() << endmsg;
    //
    int size=myFE->NumberOfMCHits();
    int i=0;
    LHCb::MCHit* hitChk=NULL;
    while(hit!=hitChk && i<size){
      //
      if(m_isVerbose) verbose()
	<< "hit number " << i << " / " << size
	<< " charge " << charge << " hit " << hit->index() <<endmsg;
      hitChk=myFE->mcHit(i);
      i++;
    };
    i--;
    if(hit==hitChk){
      double sig=myFE->mcHitCharge(i);
      //
      if(m_isVerbose) verbose() << "hit exists has signal " << sig <<endmsg;
      //
      sig+=charge;
      myFE->setMCHitCharge(i,sig);
      //
      if(m_isVerbose) verbose()<< " new signal value " << sig << " for hit "
			       << hit->index() << " hit check " 
			       << hitChk->index() << endmsg;
    }
    else{
      if(m_isVerbose) verbose()<< "hit added" << endmsg;
      myFE->addToMCHits(hit,charge);
    }
  }
  return;
}

//=======================================================================
// add a % of signal in strip to the two neighbouring strips
// it is assumed that this is a small % and hence it doesn't matter
// in what order this procedure is applied to the strip list.
//=======================================================================
void VeloSim::coupling(){
  //
  if(m_isDebug) debug()<< " ==> coupling() " <<endmsg;
  // sort FEs into order of ascending sensor + strip
  std::stable_sort(m_FEs->begin(),m_FEs->end(),
                   VeloEventFunctor::Less_by_key<const LHCb::MCVeloFE*>());
  // make new container for any added strips
  m_FEs_coupling=new LHCb::MCVeloFEs();
  //
  for(LHCb::MCVeloFEs::iterator FEIt = m_FEs->begin() ;
      m_FEs->end() != FEIt ; FEIt++ ){
    // calculate signal to couple to neighbouring strips
    double coupledSignal=(*FEIt)->addedSignal()*m_capacitiveCoupling;
    //
    if(m_isVerbose) verbose()<< "coupledSignal " << coupledSignal
			     << " orig " << (*FEIt)->addedSignal() <<endmsg;
    // subtract coupled signal from this strip
    (*FEIt)->setAddedSignal((*FEIt)->addedSignal()-2.*coupledSignal);
    //
    if(m_isVerbose) verbose()<< " subtracted " 
			     << (*FEIt)->addedSignal() <<endmsg;
    // add to previous strip (if doesn't exist then create)
    // apply charge threshold to determine if worth creating
    bool create = (coupledSignal > m_threshold*0.1);
    bool valid;
    LHCb::MCVeloFE* prevStrip=findOrInsertPrevStrip(FEIt,valid,create);
    if (valid) fillFE(prevStrip,coupledSignal);
    //
    if(m_isVerbose) {
      verbose() << " base " << (*FEIt)->strip() << " "
		<< (*FEIt)->sensor() << endmsg;
      if(valid) verbose()<< " prev " << prevStrip->strip()
			 << " " << prevStrip->sensor() << endmsg;
    }
    // add to next strip
    LHCb::MCVeloFE* nextStrip=findOrInsertNextStrip(FEIt,valid,create);
    //
    if(m_isVerbose) verbose()
      << " create " << create << " valid " << valid <<endmsg;
    //
    if (valid) fillFE(nextStrip,coupledSignal);
    //
    if(m_isVerbose) verbose()<< " base " << (*FEIt)->strip() << " "
			     << (*FEIt)->sensor() << endmsg;
    if (valid && m_isVerbose) verbose()
      << " next " << nextStrip->strip() << " " << nextStrip->sensor() <<endmsg;
  } // end of loop over hits
  // add any newly created FEs
  if(m_isDebug) debug()<< "FEs created by coupling routine "
		       << m_FEs_coupling->size() <<endmsg;
  for(LHCb::MCVeloFEs::iterator coupIt=m_FEs_coupling->begin();
      coupIt<m_FEs_coupling->end(); coupIt++){
    LHCb::MCVeloFE* myFE=m_FEs->object((*coupIt)->key());
    if( 0 != myFE ) {
      myFE->setAddedSignal( myFE->addedSignal() + (*coupIt)->addedSignal() );
      //
      if(m_isVerbose) verbose()
	<< " -- Existing FE. " << (*coupIt)->sensor() << ", "
	<< (*coupIt)->strip() << " Update with coupling FE" <<endmsg;
    }else{
      //
      if(m_isVerbose) verbose()
	<< " -- Add coupling FE " << (*coupIt)->sensor() << ", "
	<< (*coupIt)->strip() << endmsg;
      //
      m_FEs->insert(*coupIt);
    }
  }
  delete m_FEs_coupling;
  //
  if(m_isDebug) debug()<< "Number of FEs after coupling simulation "
		       << m_FEs->size() << endmsg;
  //
  return;
}
//=======================================================================
// From an originally sorted list, find the strip with the previous key,
// or create a new one.
//=======================================================================
LHCb::MCVeloFE* VeloSim::findOrInsertPrevStrip(
					       LHCb::MCVeloFEs::iterator FEIt, bool& valid, bool& create){
  //
  if(m_isDebug) debug()<< " ==> findOrInsertPrevStrip() " <<endmsg;
  // try previous entry in container
  LHCb::MCVeloFE* prevStrip=(*FEIt);
  if (FEIt!=m_FEs->begin()){
    FEIt--;
    prevStrip=(*(FEIt));
    FEIt++;
  }
  // check this
  int checkDistance;
  const DeVeloSensor* sens=m_veloDet->sensor((*FEIt)->key().sensor());
  StatusCode sc=sens->channelDistance((*FEIt)->key(),prevStrip->key(),
				      checkDistance);
  valid = sc.isSuccess();
  bool exists=(-1 == checkDistance && valid);
  if(exists) return prevStrip;
  // check if just added this strip in other container
  if (m_FEs_coupling->size()!=0){
    LHCb::MCVeloFEs::iterator last=m_FEs_coupling->end(); last--;
    prevStrip=(*last);
  }
  // check this
  sc=sens->channelDistance((*FEIt)->key(),prevStrip->key(),
                           checkDistance);
  valid = sc.isSuccess();
  exists=(-1 == checkDistance && valid);
  if(exists) return prevStrip;
  // doesn't exist so insert a new strip (iff create is true)
  if (create){
    LHCb::VeloChannelID stripKey;
    sc=sens->neighbour((*FEIt)->key(),-1,stripKey);
    if(sc.isSuccess()){
      //== Protect if key already exists ==
      prevStrip = m_FEs_coupling->object(stripKey);
      if ( 0 != prevStrip ) return prevStrip;
      //
      if(m_isVerbose) verbose()<< " create strip" << stripKey.strip() << " "
			       << stripKey.sensor() <<endmsg;
      //
      prevStrip = new LHCb::MCVeloFE(stripKey);
      m_FEs_coupling->insert(prevStrip);
      valid=true;
    }else{
      valid=false;
      prevStrip=NULL;
    }
  }else{
    valid=false;
    prevStrip=NULL;
  }
  //
  return (prevStrip);
}
//=======================================================================
//
//=======================================================================
LHCb::MCVeloFE* VeloSim::findOrInsertNextStrip(
					       LHCb::MCVeloFEs::iterator FEIt, bool& valid, bool& create){
  // From an originally sorted list, find the strip with the previous key,
  // or create a new one.
  if(m_isDebug) debug()<< " ==> findOrInsertNextStrip() " <<endmsg;
  // try next entry in container
  LHCb::MCVeloFE* nextStrip=*FEIt;
  LHCb::MCVeloFEs::iterator last = m_FEs->end(); last--;
  if (FEIt!=last){
    FEIt++;
    nextStrip=(*(FEIt));
    FEIt--;
  }
  // check this
  const DeVeloSensor* sens=m_veloDet->sensor((*FEIt)->key().sensor());
  int checkDistance;
  StatusCode sc=sens->channelDistance((*FEIt)->key(),nextStrip->key(),
				      checkDistance);
  valid = sc.isSuccess();
  bool exists=(1 == checkDistance && valid);
  if(exists) return nextStrip;
  // doesn't exist so insert a new strip (iff create is true)
  if (create){
    LHCb::VeloChannelID stripKey;
    sc=sens->neighbour((*FEIt)->key(),+1,stripKey);
    if(sc.isSuccess()){
      //== Protect if key already exists ==
      nextStrip = m_FEs_coupling->object(stripKey);
      if ( 0 != nextStrip ) return nextStrip;
      //
      if(m_isVerbose) verbose()<< " create strip" << stripKey.strip() << " "
			       << stripKey.sensor() <<endmsg;
      //
      nextStrip = new LHCb::MCVeloFE(stripKey);
      m_FEs_coupling->insert(nextStrip);
      valid=true;
    }else{
      valid=false;
      nextStrip=NULL;
    }
  }else{
    valid=false;
    nextStrip=NULL;
  }
  //
  return nextStrip;
}
//=======================================================================
// add pedestal
//=======================================================================
void VeloSim::pedestalSim(){
  if(m_isDebug) debug()<< " ==> pedestalSim() " <<endmsg;
  // add pedestals to all created FEs
  LHCb::MCVeloFEs::iterator FEIt;
  double pedestalValue=0.;
  //
  for(FEIt=m_FEs->begin(); m_FEs->end()!=FEIt; FEIt++){
    // change adc counts to electrons
    pedestalValue=m_uniformDist()*m_pedestalConst*m_pedestalVariation;
    pedestalValue+=m_pedestalConst;
    pedestalValue*=m_electronsPerADC;
    (*FEIt)->setAddedPedestal(pedestalValue);
  }
  //
  return;
}
//=======================================================================
//
//=======================================================================
void VeloSim::noiseSim(){
  //
  if(m_isDebug) debug()<< " ==> noiseSim() " <<endmsg;
  //
  // consider noise contributions due to
  // 1) strip capacitance and 2) leakage current.

  // 1) readout chip noise
  // const term and term prop to strip capacitance.

  // 2) leakage current
  // shot noise prop. to sqrt(Ileak).
  // radn induced leakage current prop to fluence and strip volume.
  // fluence prop to 1/r^2, strip area prop to r^2. hence term const with r.
  //
  // summary - sigma of noise from constant + term prop to strip cap.

  // loop through already allocated hits adding noise (if none already added)
  // values measured from NZS data to match real detector
  for(LHCb::MCVeloFEs::iterator FEIt = m_FEs->begin() ;
      m_FEs->end() != FEIt ; FEIt++ ){
    if((*FEIt)->addedNoise()==0){
      const DeVeloSensor* sens=m_veloDet->sensor((*FEIt)->sensor());
      double sNoise = sens->stripNoise((*FEIt)->strip());
      double noise=0.;
      if(sNoise > 0.) noise = noiseValue(sNoise); // NaN protection for zero noise
      (*FEIt)->setAddedNoise(noise);
      //
      if(m_isVerbose) verbose()<< " noise added to existing strip "
			       << (*FEIt)->addedNoise()<< endmsg;
    }
  }
  // allocate noise (above threshold) to channels that don't currently
  // have signal
  std::vector<DeVeloSensor*>::const_iterator sensBegin;
  std::vector<DeVeloSensor*>::const_iterator sensEnd;

  if (!m_simNoisePileUp) { // simulation of main VELO
    sensBegin = m_veloDet->rPhiSensorsBegin();
    sensEnd   = m_veloDet->rPhiSensorsEnd();
  } else { // simulation of PileUp Sensors 
    sensBegin = m_veloDet->pileUpSensorsBegin();
    sensEnd   = m_veloDet->pileUpSensorsEnd();
  }
  for(std::vector<DeVeloSensor*>::const_iterator iSens = sensBegin;
      iSens != sensEnd;
      ++iSens) {
    const DeVeloSensor* sens = *iSens;
    if(sens->isReadOut()){
      // use average noise of sensor, should be adequate if variation in
      // noise not too large.
      // number of hits to add noise to (i.e. fraction above threshold)
      // add both large +ve and -ve noise.
      int hitNoiseTotal=-999;
      int maxStrips= sens->numberOfStrips();
      if(!m_makeNonZeroSuppressedData){
        hitNoiseTotal = safe_int_poissonDist();
      }else{
        hitNoiseTotal=sens->numberOfStrips();
      }
      //
      unsigned int sensorNo=sens->sensorNumber();
      if(m_isVerbose) verbose()
	<< "Number of strips to add noise to " << hitNoiseTotal
	<< " sensor Number " << sensorNo
	<< " maxStrips " << maxStrips
	<< " sigma of noise " 
	<< m_averageStripNoise*m_noiseScale*m_electronsPerADC
	<< " threshold " << m_threshold
	<< " tail probability " 
	<< m_noiseTailProb
	<< endmsg;
      //
      for(int noiseHit=0; noiseHit<hitNoiseTotal; noiseHit++){
        if(!m_makeNonZeroSuppressedData){
          // choose random hit to add noise to
          // get strip number
          int stripArrayIndex= int(LHCb::Math::round(m_uniformDist()*
                                                     (maxStrips-1)));          
          // optimisation : only generate with m_thresholdADC if next to an 
          // existing strip, otherwise use m_thresholdADCSingle if isolated
	  // as an isolated strip below that can not start a cluster
          double threshold = m_threshold;
          if( !m_FEs->object(LHCb::VeloChannelID(sensorNo,stripArrayIndex)) &&
              !m_FEs->object(LHCb::VeloChannelID(sensorNo,stripArrayIndex+1)) &&
              !m_FEs->object(LHCb::VeloChannelID(sensorNo,stripArrayIndex-1))){
            if( m_uniformDist() > m_noiseTailProbSingle/m_noiseTailProb ) continue;
            threshold = m_thresholdSingle;
          }
          if(!sens->OKStrip(stripArrayIndex))  continue; //unconnected strip
          LHCb::VeloChannelID stripKey(sensorNo,stripArrayIndex);
          // find strip in list.
          LHCb::MCVeloFE* myFE = findOrInsertFE(stripKey);
          if (myFE->addedNoise()==0){
            double noise=
	      noiseValueTail(sens->stripNoise(stripArrayIndex)*m_noiseScale,
			     threshold);
            myFE->setAddedNoise(noise);
            //
            if(m_isVerbose) verbose()<< "hit from tail of noise created "
				     << myFE->addedNoise() << endmsg;
          }
        }else{
          // generate noise for all strips, beside those ones with 
	  // already added noise
          LHCb::VeloChannelID stripKey(sensorNo, noiseHit);
          LHCb::MCVeloFE* myFE=findOrInsertFE(stripKey);
          if(myFE->addedNoise()==0){
            double noise=noiseValue(sens->stripNoise(noiseHit));
            myFE->setAddedNoise(noise);
            if(m_isVerbose) verbose()<< "hit from noise created " 
				     << myFE->addedNoise() <<endmsg;
          }
        }
      }
    }
  }
  //
  return;
}
//=========================================================================
// generate some noise
//=========================================================================
double VeloSim::noiseValue(double noiseSigma){
  //
  if(m_isDebug) debug()<< " ==> noiseValue() " <<endmsg;
  //
  double noise=m_gaussDist()*noiseSigma*m_electronsPerADC;
  return noise;
}
//=========================================================================
// generate some noise from tail of distribution
//=========================================================================
 double VeloSim::noiseValueTail(double noiseSigma,double threshold){
  //
  if(m_isDebug) debug()<< "noiseValueTail() " <<endmsg;
  //-----------------------------------------------------
  //Seems too slow to create and destroy random generators
  // make a tempoary random number generator to get tail above threshold
  //  Rndm::Numbers ranGaussTail(randSvc(), 
  //			     Rndm::GaussianTail(m_threshold,
  //						noiseSigma*m_electronsPerADC));
  //double noise=ranGaussTail();
  //-----------------------------------------------------
  
  double noise = ran_gaussian_tail(threshold,noiseSigma*m_electronsPerADC);  
  double sign=m_uniformDist();
  if (sign > 0.5) noise*=-1.; // noise negative or positive
  return noise;
}
//=======================================================================
// common mode - not yet implemented
//=======================================================================
void VeloSim::CMSim(){
  return;
}
//=======================================================================
// dead strips
//=======================================================================
void VeloSim::deadStrips(){
  //
  if(m_isDebug) debug()<< " ==> deadStrips() " <<endmsg;
  // Add some strip inefficiency
  // channels are given zero signal, and hence will be removed by the
  // threshold cut in final process.
  // e.g. set stripInefficiency to 0.01 for 1% dead channels
  for(LHCb::MCVeloFEs::iterator itF1 = m_FEs->begin(); m_FEs->end() != itF1;
      itF1++){
    ++m_totalFEs;
    const DeVeloSensor* sensor=m_veloDet->sensor((*itF1)->sensor());
    if(sensor->OKStrip((*itF1)->strip())){ 
      // no db problems, check random inefficiency
      double cut =  m_uniformDist();
      if ( m_stripInefficiency > cut ) {
	++m_killedFEsRandom;
	(*itF1)->setAddedSignal( 0. );
      }
    }else{
      // assume thresholds set to have no response in TELL1
      if(m_isVerbose) verbose() 
	<< "Removed signal and noise from not OK strip "
	<< format("[%3i,%4i]",(*itF1)->sensor(),(*itF1)->strip())
	<< endmsg;
      (*itF1)->setAddedSignal( 0. );
      (*itF1)->setAddedNoise( 0. );
      ++m_killedFEsBadStrips;
    }
  }
  //
  return;
}
//=======================================================================
// find a strip in list of FEs, or if it does not currently exist create it
//=======================================================================
LHCb::MCVeloFE* VeloSim::findOrInsertFE(LHCb::VeloChannelID& stripKey){
  //
  if(m_isDebug) debug()<< " ==> findOrInsertFE() " <<endmsg;
  //
  LHCb::MCVeloFE* myFE = m_FEs->object(stripKey);
  if (myFE==NULL) {
    // this strip has not been used before, so create
    myFE = new LHCb::MCVeloFE(stripKey);
    //
    if(m_isVerbose) verbose()<< " -- Add FE " << stripKey.sensor() << ","
			     << stripKey.strip() << endmsg;
    //
    if(m_isDebug) debug()<< "size of FEs: " << m_FEs->size() <<endmsg;
    m_FEs->insert(myFE);
    if(m_isDebug) debug()<< "size of FEs: " << m_FEs->size() <<endmsg;

  }
  //
  return (myFE);
}
//=======================================================================
// remove any MCFEs with charge below abs(threshold)
//=========================================================================
void VeloSim::finalProcess(){
  //
  if(m_isDebug) debug()<< " ==> finalProcess() " <<endmsg;
  // cannot do this by remove_if, erase as storing/erasing pointers.
  // instead sort whole container and erase.
  // if want to produce the non-zero suppressed data set the flag
  if(!m_makeNonZeroSuppressedData){
    std::sort(m_FEs->begin(), m_FEs->end(),
              VeloEventFunctor::Less_by_charge<const LHCb::MCVeloFE*>());
    std::reverse(m_FEs->begin(),m_FEs->end());
    LHCb::MCVeloFEs::iterator
      it1=std::find_if(m_FEs->begin(), m_FEs->end(),
		       VeloChargeThreshold(m_threshold));
    LHCb::MCVeloFEs::iterator it2=m_FEs->end();
    m_FEs->erase(it1, it2);
    // sort FEs into order of ascending sensor + strip
  }
  // sort according to sensor/strip number
  std::sort(m_FEs->begin(),m_FEs->end(),
            VeloEventFunctor::Less_by_key<const LHCb::MCVeloFE*>());
  //
  return;
}
//=======================================================================
// store MCFEs
//=======================================================================
StatusCode VeloSim::storeOutputData(){
  //
  if(m_isDebug) debug()<< " ==> storeOutputData() " <<endmsg;
  
  std::vector<std::string>::const_iterator iCont;
  for ( iCont = m_outputContainers.begin(); 
	iCont != m_outputContainers.end(); ++iCont ){

    if(m_isDebug) debug() 
      << "Writing " << m_FEs->size() << " MCVeloFEs to " << *iCont << endmsg;

    LHCb::MCVeloFEs *outputCont = getIfExists<LHCb::MCVeloFEs>(*iCont);
    if( NULL != outputCont ){
      if(m_isVerbose) verbose() 
        << "Size of " << *iCont << " before update " << outputCont->size()
        << endmsg;      	
      LHCb::MCVeloFEs::const_iterator feIt;
      for(feIt=m_FEs->begin(); feIt!=m_FEs->end(); ++feIt){
        outputCont->insert(*feIt);
      }
      if(m_isVerbose) verbose() 
        << "Size of " << *iCont << " after update " << outputCont->size()
        << endmsg;  
    }else{
      put(m_FEs, *iCont); //push local container into TES
    }
  }
  return StatusCode::SUCCESS;
}
//=======================================================================
// delta ray tail random numbers
// dN/DE=k*1/E^2 for relativistic incident particle
// E(r)=1/r, where r is uniform in range 1/Tmin < r < 1/Tmax
// but Tmax bounded by energy left to allocate, so following is
// not truly correct
//=======================================================================
double VeloSim:: ran_inv_E2(double Tmin,double Tmax){
  //
  if(m_isDebug) debug()<< " ==> ran_inv_E2() " <<endmsg;
  //
  double range=((1./Tmin) - (1./Tmax));
  double offset=1./Tmax;
  double uniform = m_uniformDist()*range+offset;
  double charge=1./uniform;
  //
  return (charge);
}
//=========================================================================
// Returns a gaussian random variable larger than a
// This implementation does one-sided upper-tailed deviates.
// Markus has promised to add this in the next release of the core Gaudi code
// in autumn 2002, till then need this version here.
// This code is based on that from the gsl library.
//=========================================================================
double VeloSim::ran_gaussian_tail(const double a, const double sigma) {
  //
  if(m_isDebug) debug()<< " ==> ran_gaussian_tail() " <<endmsg;
  //
  double s = a / sigma;
  if (s < 1)
    {
      /* For small s, use a direct rejection method. The limit s < 1
         can be adjusted to optimise the overall efficiency */
      double x;
      do
        {
          x = m_gaussDist();
        }
      while (x < s);
      return x * sigma;
    }else{
    /* Use the "supertail" deviates from the last two steps
     * of Marsaglia's rectangle-wedge-tail method, as described
     * in Knuth, v2, 3rd ed, pp 123-128.  (See also exercise 11, p139,
     * and the solution, p586.)
     */
    double u, v, x;
    do
      {
	u = m_uniformDist();
	do
	  {
	    v = m_uniformDist();
	  }
	while (v == 0.0);
	x = sqrt (s * s - 2 * log (v));
      }
    while (x * u > s);
    return x * sigma;
  }
}
//============================================================================
double VeloSim::chargeTimeFactor(double TOF, double bunchOffset, double z)
{
  if(m_isDebug) debug()<< " ==> VeloSim::spillOverReminder " <<endmsg;

  // correction for Z of sensor, assume timed to have z/c as T0 for sensors
  TOF -=  fabs(z/Gaudi::Units::c_light); 

  double time=m_pulseShapePeakTime + m_offPeakSamplingTime + bunchOffset - TOF;
  double chargeReminder = m_SiTimeTool->response(time);
  if(m_isDebug) debug()<< "spillOverReminder= " << chargeReminder 
		       << " from time " << time 
		       << " bunch offset " << bunchOffset<<endmsg;
  //
  return ( chargeReminder );
}
//============================================================================
bool VeloSim::checkConditions(LHCb::MCHit* aHit)
{
  if(m_isDebug) debug()<< " ==> checkConditions(Hit) " <<endmsg;
  //
  const DeVeloSensor* sensor=m_veloDet->sensor(aHit->sensDetID());
  // check the conditions
  bool isReadOut=sensor->isReadOut();
  //
  return ( isReadOut );
}
//

StatusCode VeloSim::finalize(){
  if(m_totalFEs > 0){
    double fracBad = static_cast<double>(m_killedFEsBadStrips)/static_cast<double>(m_totalFEs);
    double fracRandom = static_cast<double>(m_killedFEsRandom)/static_cast<double>(m_totalFEs);
    info() << format("Removed %5.2f%% MCVeloFEs due to bad strips and %5.2f%% as random removals",
		     100.*fracBad,100.*fracRandom)
	   << endmsg;    
  }
  return GaudiAlgorithm::finalize(); // must be executed last
}
