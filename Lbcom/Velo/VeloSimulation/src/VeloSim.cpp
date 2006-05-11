// $Id: VeloSim.cpp,v 1.14 2006-05-11 16:44:28 cattanem Exp $
// Include files
// STL
#include <string>
#include <vector>

// Mathcore
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// GSL
#include "gsl/gsl_sf_erf.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// from LHCbKernel
#include "Kernel/LHCbMath.h"
#include "Kernel/VeloEventFunctor.h"

// from MCEvent
#include "Event/MCHit.h"
#include "Event/MCVeloFE.h"
#include "Event/MCParticle.h"

// VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "VeloSim.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloSim
//
// 16/01/2002 : Chris Parkes, update Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloSim );

struct chargeThreshold : public std::unary_function<LHCb::MCVeloFE*,  bool> {
  bool operator()(LHCb::MCVeloFE* FE) {
    // decide to cut out large -ve noise
    return FE->charge() < 4500.; //m_threshold;
  }
};

static const double k_spillOverTime=25.;
static const double k_pulseShapePeakTime=30.7848;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloSim::VeloSim( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_veloDet ( 0 ),
    m_sensorOn ( true ),
    m_fitParams( 7, 0. )
{
  declareProperty("InputContainer", m_inputContainer = LHCb::MCHitLocation::Velo );
  declareProperty("SpillOverInputData", m_spillOverInputContainer = "Prev/" + m_inputContainer );
  declareProperty("PileUpInputContainer", m_pileUpInputContainer = LHCb::MCHitLocation::PuVeto );
  declareProperty("PileUpSpillOverInputData", m_pileUpSpillOverInputContainer = "Prev/" + m_pileUpInputContainer );
  declareProperty("OutputContainer", m_outputContainer = LHCb::MCVeloFELocation::Default );
  declareProperty("PileUpOutputContainer", m_pileUpOutputContainer = LHCb::MCVeloFELocation::PuVeto );
  declareProperty("ChargeSim", m_chargeSim = true );
  declareProperty("InhomogeneousCharge", m_inhomogeneousCharge = true );
  declareProperty("Coupling", m_coupling = true );
  declareProperty("NoiseSim", m_noiseSim = true );
  declareProperty("PedestalSim", m_pedestalSim = true );
  declareProperty("CMSim", m_CMSim = true );
  declareProperty("StripInefficiency", m_stripInefficiency = 0.0 );
  declareProperty("SpillOver", m_spillOver = true );
  declareProperty("PileUp", m_pileUp = true );
  declareProperty("TestSimulation", m_testSim = false );
  declareProperty("SmearPosition", m_smearPosition = 0.0 );
  declareProperty("Threshold", m_threshold = 4500. );
  declareProperty("NoiseConstant", m_noiseConstant = 500. );
  declareProperty("kT", m_kT = 0.025 );
  declareProperty("BiasVoltage", m_biasVoltage = 105. );
  declareProperty("eVPerElectron", m_eVPerElectron = 3.6 );
  declareProperty("SimulationPointsPerStrip", m_simulationPointsPerStrip = 3 );
  declareProperty("ChargeUniform", m_chargeUniform = 70. );
  declareProperty("DeltaRayMinEnergy", m_deltaRayMinEnergy = 1000. );
  declareProperty("CapacitiveCoupling", m_capacitiveCoupling = 0.05 );
  declareProperty("AverageStripCapacitance", m_averageStripCapacitance = 20. );
  declareProperty("NoiseCapacitance", m_noiseCapacitance = 50. );
  declareProperty("FitParams", m_fitParams );
  declareProperty("OffPeakSamplingTime", m_offPeakSamplingTime = 0. );
  
  Rndm::Numbers m_gaussDist;
  Rndm::Numbers m_uniformDist;
}

//=============================================================================
// Destructor
//=============================================================================
VeloSim::~VeloSim() {};
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode VeloSim::initialize() {
  //
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialise" << endmsg;

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  m_baseDiffuseSigma=sqrt(2*m_kT/m_biasVoltage);
  
  // random number initialisation
  StatusCode sc1=m_gaussDist.initialize( randSvc(), Rndm::Gauss(0.,1.0));
  StatusCode sc2=m_uniformDist.initialize( randSvc(), Rndm::Flat(0.,1.0));
  if(!(sc1&sc2)){
    error() << "Random number init failure" << endmsg;
    return (StatusCode::FAILURE);
  }
  //
  info()<< "============================================" <<endmsg;
  info()<< "Initialization of the pulse shape parameters" <<endmsg;
  info()<< "--------------------------------------------" <<endmsg;
  info()<< "|    par0: " << m_fitParams[0] <<endmsg;
  info()<< "|    par1: " << m_fitParams[1] <<endmsg;  
  info()<< "|    par2: " << m_fitParams[2] <<endmsg;
  info()<< "|    par3: " << m_fitParams[3] <<endmsg;
  info()<< "|    par4: " << m_fitParams[4] <<endmsg;
  info()<< "|    par5: " << m_fitParams[5] <<endmsg;
  info()<< "|    par6: " << m_fitParams[6] <<endmsg;
  info()<< "============================================" <<endmsg;
  //
  return StatusCode::SUCCESS;
};
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSim::execute() {
  //
  debug() << "==> Execute" << endmsg;
  //
  StatusCode sc;
  sc=getInputData(); // get Velo MCHits, pile-up MCHits and spillOver data
  //
  if (sc){
    // velo simulation
    m_hits = m_veloHits;
    debug()<< "hits cont: " << m_veloHits <<endmsg;
    m_spillOverHits=NULL;
    if (0!=m_veloSpillOverHits) m_spillOverHits = m_veloSpillOverHits;
    debug()<< "spill over cont: " << m_spillOverHits <<endmsg;
    
    m_FEs  = m_veloFEs;
    m_simMode="velo";
    sc = simulation();
    // pile-up stations simulation
    if (m_pileUp){
      m_hits = m_pileUpHits;
      m_spillOverHits = m_pileUpSpillOverHits;
      m_FEs  = m_pileUpFEs;
      m_simMode="pileUp";
      sc = simulation();
    }
  }
  debug() << "let's go to the output data" << endmsg;
  
  if (sc) sc= storeOutputData(); // add MCFEs to TDS
  //
  return (sc);
}
//==============================================================================
StatusCode VeloSim::simulation() {
  // perform simulation
  StatusCode sc;
  // simulate signals in strips from GEANT hits of current event
  if (m_chargeSim) sc= chargeSim(false); 
  // simulate signals in strips from GEANT hits of spillOver event
  if(m_sensorOn){
    if (sc&&m_chargeSim&&m_spillOver) sc= chargeSim(true); 
    // charge sharing from capacitive coupling of strips
    if (sc&&m_coupling) sc=coupling(); 
    // add noise
    if (sc&&m_noiseSim) sc= noiseSim(); 
    // add pedestals - not yet implemented
    if (sc&&m_pedestalSim) sc= pedestalSim(); 
    // common mode - not yet implemented
    if (sc&&m_CMSim) sc=CMSim();
    // dead strips / channels
    if (sc&&(m_stripInefficiency>0.)) sc=deadStrips(); 
    // remove any unwanted elements and sort
    if (sc) sc=finalProcess();
  }
  //
  return (sc);
}
//=========================================================================
StatusCode VeloSim::getInputData() {
  //
  debug()<< " ==> getInputData() " <<endmsg;
  // get the velo input data
  debug()<< "Retrieving MCHits from " << m_inputContainer <<endmsg;
  m_veloHits=get<LHCb::MCHits>( m_inputContainer );
  debug()<< " Size of MCHits: " << m_veloHits->size() <<endmsg;
  //
  debug() << m_veloHits->size() << " hits retrieved" <<endmsg;
  // get the pile-up input data
  if (m_pileUp) {
    debug()<< "Retrieving MCHits from " << m_pileUpInputContainer <<endmsg;
    //
    m_pileUpHits=get<LHCb::MCHits>( m_pileUpInputContainer );
    //
    debug()<< m_pileUpHits->size() << " pile-up hits retrieved" <<endmsg;
  }    
  // get the velo Spillver
  if (m_spillOver){
    debug() << "Retrieving MCHits of SpillOver Event from "
			<< m_spillOverInputContainer <<endmsg;      
    if(!exist<LHCb::MCHits>( m_spillOverInputContainer )){
      debug()<<"Spill over event not present unable to retrieve input container="
	          << m_spillOverInputContainer <<endmsg;
      m_veloSpillOverHits = 0;
    } else { 
      //      m_veloSpillOverHits = get<LHCb::MCHits>( m_spillOverInputContainer );
      SmartDataPtr<LHCb::MCHits> spillOverHits( eventSvc(),
                                 m_spillOverInputContainer );
      m_veloSpillOverHits=spillOverHits;
      //
      debug()<< m_veloSpillOverHits->size() << " spill over hits retrieved" 
             <<endmsg;
    }
  }
  // get the pileUp SpillOver input data
  if (m_spillOver&&m_pileUp){
    debug()<< "Retrieving MCHits of Pile Up SpillOver Event from "
           << m_pileUpSpillOverInputContainer <<endmsg;      
    if( !exist<LHCb::MCHits>(m_pileUpSpillOverInputContainer) ){      
      debug()<< "Spill over event for PileUp not present unable to retrieve " 
	           << "input data container=" << m_spillOverInputContainer <<endmsg;
      m_pileUpSpillOverHits=0;
    }else{
      SmartDataPtr<LHCb::MCHits> pileUpSpillOverHits( eventSvc(),
                                 m_pileUpSpillOverInputContainer );
      m_pileUpSpillOverHits=pileUpSpillOverHits;
      debug()<< m_pileUpSpillOverHits->size()
			       << " PileUp Spill over hits retrieved" <<endmsg;
    }
  }    
  // make vectors for output 
  if (m_pileUp) {m_pileUpFEs=new LHCb::MCVeloFEs();}
  else          {m_pileUpFEs=NULL;}
  m_veloFEs = new LHCb::MCVeloFEs();
  //
  return (StatusCode::SUCCESS);
}
//=========================================================================
// loop through hits allocating the charge to strips
//=========================================================================
StatusCode VeloSim::chargeSim(bool spillOver) {
  //
  debug() << " ==> chargeSim() " <<endmsg;
  //
  if (spillOver) debug()<< " for spill over Event" <<endmsg;
  //
  LHCb::MCHits* hits;
  if (!spillOver){
    // hits from current event
    hits=m_hits;
  }else{
    // hits from spill Over Event. Return if no spillover
    if(0 == m_spillOverHits){
      return (StatusCode::SUCCESS);
    }
    hits=m_spillOverHits;
  }
  verbose()<< "Number of hits to simulate=" << hits->size() << endmsg;
  //loop over input hits
  for(LHCb::MCHits::const_iterator hitIt = hits->begin();
      hits->end() != hitIt ; hitIt++ ){
    LHCb::MCHit* hit = (*hitIt);    
    if(checkConditions(hit)){
      m_sensorOn=true;
      // degrade resolution for April 2003 Robustness Test
      if (m_smearPosition>0) {
        m_movePosition.SetX(m_gaussDist()*m_smearPosition);
        m_movePosition.SetY(m_gaussDist()*m_smearPosition);  
        m_movePosition.SetZ(0.);
      }else{
        m_movePosition.SetX(0.);
        m_movePosition.SetY(0.);  
        m_movePosition.SetZ(0.);
     }
     if(m_testSim){ testSim(hit,spillOver); }
     else{
       // calculate a set of points in the silicon
       //with which the simulation will work
       int NPoints = simPoints(hit);
       verbose()<< "Simulating " << NPoints 
                << " points in Si for this hit" <<endmsg;
       if (NPoints>0){
         // calculate charge to assign to each point
         // taking account of delta ray inhomogeneities
         std::vector<double> sPoints(NPoints);
         chargePerPoint(*hitIt,NPoints,sPoints,spillOver);
         // diffuse charge from points to strips
         diffusion(*hitIt,NPoints,sPoints,spillOver);
        }
      }
    }else{
      debug()<< " the sensor is not read-out" <<endmsg;
      m_sensorOn=false;
    }
  }
  debug()<< "Number of MCVeloFEs " << m_FEs->size() <<endmsg;
  //
  return (StatusCode::SUCCESS);
}
//=========================================================================
// test simulation - allocate all charge to entry/exit point strip
//=========================================================================
void VeloSim::testSim(LHCb::MCHit* hit, bool spillOver){
  //
  debug()<< " ==> testSim() " <<endmsg;
  
  // test routine - simplest possible simulation.
  double EntryFraction;
  double pitch;
  StatusCode sc;
  LHCb::VeloChannelID entryChan;
  const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
  if(sens) {
    if (m_uniformDist()>0.5){
      sc=sens->pointToChannel(hit->entry(), entryChan, EntryFraction, pitch);
    }
    else{
      sc=sens->pointToChannel(hit->exit(), entryChan, EntryFraction, pitch);
    }
    double charge=(hit->energy()/eV)/m_eVPerElectron;
    if(spillOver) charge*=spillOverReminder(hit->time()/ns);
    LHCb::MCVeloFE* myFE = findOrInsertFE(entryChan);
    if(spillOver){
      debug()<< " entry chan strip: " << entryChan.strip() <<endmsg;
      debug()<< " fe strip: " << myFE->key().strip() <<endmsg;
    }
    
    fillFE(myFE,hit,charge);
  } else {
    verbose() << "testSim: No sensor from hit " << (hit->sensDetID()) << endreq;
  }
}
//=========================================================================
// calculate how many points in the silicon the simulation will be performed at
//=========================================================================
long VeloSim::simPoints(LHCb::MCHit* hit){
  //
  debug()<< " ==> simPoints() " <<endmsg;
  //
  double EntryFraction=0.,ExitFraction=0.;
  double pitch=0.;
  StatusCode EntryValid, ExitValid;
  verbose()<< "hit entry: " << hit->entry() <<endmsg;
  LHCb::VeloChannelID entryChan, exitChan;
  const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
  double NPoints=0.;
  if(sens) {
    EntryValid=sens->pointToChannel(hit->entry(),entryChan,EntryFraction,
                                         pitch);
    ExitValid=sens->pointToChannel(hit->exit(),exitChan,ExitFraction,
                                        pitch);
    verbose()<< "calculate number of points to simulate in Si"
	           <<endmsg;
    verbose()<< "entry/exit points "
	           << entryChan.sensor() << " " << entryChan.strip()
	           <<  " + " << EntryFraction
	           << " / " <<  exitChan.sensor()  << " " << exitChan.strip()
	           << " + " << ExitFraction <<endmsg;
    if (!EntryValid) verbose() << " invalid entry point" <<endmsg;
    if (!ExitValid) verbose() << " invalid exit point" <<endmsg;
    //  
    if (EntryValid&&ExitValid){
      // both entry and exit are at valid strip numbers,
      // calculate how many full strips apart
      int INeighb;
      StatusCode sc=sens->channelDistance(entryChan,exitChan,INeighb);
      if (sc) NPoints = fabs(float(INeighb)-(EntryFraction-ExitFraction));
      //
      verbose()<< "Integer number of strips apart " << INeighb
               << " floating number " << NPoints <<endmsg;
    }else{
      // either entry or exit or both are invalid, ignore this hit
      NPoints=0.;
      //
      if((EntryValid!=ExitValid)){
        verbose()<< "simPoints: only one of entry and exit point of hit are in "
                 << "silicon - hit ignored " <<endmsg;
      }
    }
  } else { // matches test on sensor pointer
    verbose() << "Invalid sensor from hit " << (hit->sensDetID());
  }
  //
  return (int(ceil(NPoints)*m_simulationPointsPerStrip));
}
//=========================================================================
// allocate charge to points
//=========================================================================
void VeloSim::chargePerPoint(LHCb::MCHit* hit, int Npoints,
                             std::vector<double>& Spoints, bool spillOver){
  //
  debug()<< " ==> chargePerPoint() " <<endmsg;
  // total charge in electrons
  double charge=(hit->energy()/eV)/m_eVPerElectron;
  debug()<< "Number of electron-hole pairs: " << charge <<endmsg;  
  if(spillOver){ 
    charge*=spillOverReminder(hit->time()/ns);
    debug()<< " charge from spill-over: " << charge <<endmsg;
    //
    debug()<< "calculating charge per simulation point" <<endmsg;
    debug()<< "total charge " << charge << " in eV " 
           << hit->energy()/eV <<endmsg;
    debug()  << " for spill over event " <<endmsg;
  }
  // charge to divide equally
  double chargeEqual;
  if(m_inhomogeneousCharge){
    // some of charge allocated by delta ray algorithm
    const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
    chargeEqual=m_chargeUniform*sens->siliconThickness()/micrometer;
    if(spillOver) 
      chargeEqual*=spillOverReminder(hit->time()/ns);
    if(chargeEqual>charge)  chargeEqual=charge;
  }else{
    // all of charge allocated equally to points
    chargeEqual=charge;
  }
  verbose()<< "total charge " << charge
	         << " charge for equal allocation " << chargeEqual <<endmsg;  
  // divide equally
  double chargeEqualN=chargeEqual/Npoints;
  double fluctuate=0.;
  for (int i=0; i<Npoints; i++){
    // gaussian fluctuations
    if (m_inhomogeneousCharge) fluctuate=m_gaussDist()*sqrt(chargeEqualN);
    Spoints[i]=chargeEqualN+fluctuate;
    verbose()<< "charge for pt" << i << " is " << Spoints[i] 
             << endmsg;
   // inhomogeneous charge dist from delta rays
    if (m_inhomogeneousCharge){
      deltaRayCharge(charge-chargeEqual, 0.001*charge, Npoints, Spoints);
      // ensure total charge is allocated
      double total=0.;
      for (int i=0; i<Npoints; i++){total+=Spoints[i];}
      verbose()<< "charge distributed: " << total <<endmsg;
      if( charge < total ) {
       double adjust=charge/total;
       for (int j=0; j<Npoints; j++){Spoints[j]*=adjust;}
      }
    }
  }
  double totalCharge=0.;
  for(unsigned int i=0; i<Spoints.size(); i++){
    totalCharge+=Spoints[i];
  }
  verbose()<< "total charge after correction: " << totalCharge <<endmsg;
  
  //
  return;
}
//=========================================================================
// allocate remaining charge from delta ray distribution
//=========================================================================
void VeloSim::deltaRayCharge(double charge, double tol,
                             int Npoints, std::vector<double>& Spoints){
  //
  debug()<< " ==> deltaRayCharge() " <<endmsg;
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
    double charge=ran_inv_E2(Tmin,Tmax);
    // choose pt at random to add delta ray
    int ipt=int(LHCbMath::round(m_uniformDist()*(Npoints-1)));
    //
    verbose()<< " delta ray charge added to point " << ipt
		         << "/" << Npoints <<endmsg;
    //
    Spoints[ipt]+=charge;
    Tmax-=charge;
  }
  return;
}
//=========================================================================
// allocate the charge to the collection strips
//=========================================================================
void VeloSim::diffusion(LHCb::MCHit* hit,int Npoints,
                        std::vector<double>& Spoints,bool spillOver){
  //
  debug()<< " ==> diffusion " <<endmsg;
  //
  Gaudi::XYZVector path=(hit->exit())-(hit->entry());
  path/=(Npoints*2); // distance between steps on path
  Gaudi::XYZPoint point=(hit)->entry()+path;//+m_movePosition;
  Gaudi::XYZVector tempVec(m_movePosition);
  point=point+tempVec;
  //
  const DeVeloSensor* sens=m_veloDet->sensor(hit->sensDetID());
  double thickness=sens->siliconThickness()/micrometer;
  // sensor numbers for hits currently incorrect (7/02) 
  // nasty fudge - use one hardcoded number
  //  double thickness=m_velo->siliconThickness(1)/micrometer;
  double ZDiffuse=thickness;
  // assume strips are at opposite side of Si to entry point
  double dz=ZDiffuse/(double(Npoints)*2.); // distance between steps on path
  ZDiffuse-=dz;
  //
  for (int ipt=0; ipt<Npoints; ipt++){ //loop over points on path
    double fraction,pitch;
    //
    verbose()<< " ipt " << ipt << " point " << point <<endmsg;
    //calculate point on path
    LHCb::VeloChannelID entryChan;
    StatusCode valid=sens->pointToChannel(point,entryChan,fraction,pitch);
    if(!valid){
      verbose()<< " point is not in active silicon " << point 
               << " entry " << hit->entry() << " exit " 
               <<  hit->exit() << " detID " << hit->sensDetID() <<endmsg;
    }
    //
    if(valid){
      // a point may be invalid, despite entry and exit points being valid,
      // as it lies in say a bias rail dead area.
      // charge from this point is considered lost
      verbose()<< "chan " << entryChan.strip() << " fraction "
		           << fraction << " pitch " << pitch << " valid " 
               << valid <<endmsg;
      //
      const int neighbs=1; // only consider +/- this many neighbours
      double chargeFraction[2*neighbs+1];
      double totalFraction=0.;
      // loop over neighbours per point
      int iNg;
      for(iNg=-neighbs; iNg<=+neighbs; iNg++){
        double diffuseDist1=((iNg-0.5)-fraction)*pitch/micrometer;
        double diffuseDist2=((iNg+0.5)-fraction)*pitch/micrometer;
        verbose()<< "dif1: " << diffuseDist1 << ", dif2: "
                 << diffuseDist2 <<endmsg;
        
        //      double diffuseDist1=((iNg)-fraction)*pitch/micrometer;
        //      double diffuseDist2=((iNg+1.)-fraction)*pitch/micrometer;
        double diffuseSigma=m_baseDiffuseSigma*sqrt(thickness*ZDiffuse);
        //
        verbose()<< "diffuseDist1 " << diffuseDist1
		             <<   " diffuseDist2 " << diffuseDist2 << " diffuseSigma "
		             << diffuseSigma << " base " << m_baseDiffuseSigma
		             << " zdiff " << ZDiffuse <<endmsg;
        //
        double prob1= gsl_sf_erf_Q(diffuseDist1/diffuseSigma);
        double prob2= gsl_sf_erf_Q(diffuseDist2/diffuseSigma);
        //
        verbose() << " prob1+2 " <<  prob1 << " " << prob2 <<endmsg;
        //
        int i= (iNg<0) ? neighbs+abs(iNg) : iNg;        
        chargeFraction[i]=fabs(prob1-prob2);
        totalFraction+= fabs(prob1-prob2);
        //
        verbose()<< " iNg " << iNg << " cfrac "
                 << chargeFraction[i]  << " tot " << totalFraction <<endmsg;
      }
      // renormalise allocated fractions to 1., and update strip signals
      for(iNg=-neighbs; iNg<=+neighbs; iNg++ ){
        int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
        //
        debug()<< i << " iNg " << iNg << " ipt " << ipt
               << " " <<endmsg;
        //
        double charge=Spoints[ipt]*(chargeFraction[i]/totalFraction);
        //
        debug()<< i << " ipt " << ipt << " charge "
               << charge <<endmsg;
        if (charge>m_threshold*0.1){
          // ignore if below 10% of threshold
          // calculate index of this strip
          LHCb::VeloChannelID stripKey;
          valid=sens->neighbour(entryChan,iNg,stripKey);
          //
          debug()<< " neighbour " << entryChan.strip() << " "
                 << stripKey.strip() << " iNg " << iNg <<endmsg;
          // update charge and MCHit list
          if (valid){
            LHCb::MCVeloFE* myFE = findOrInsertFE(stripKey);
            if(spillOver){
              debug()<< " entry chan strip: " << entryChan.strip() <<endmsg;
              debug()<< " fe strip: " << myFE->key().strip() <<endmsg;
            }
            if (!spillOver) fillFE(myFE,hit,charge); // update and add MC link
            else fillFE(myFE,charge); // update, no MC link
          }
        }
      } // neighbours loop
    } // valid point
    point+=2*path; // update to look at next point on path
    ZDiffuse-=2.*dz;
    
  } // loop over points
  //
  return;
}
//=========================================================================
// update signal and list of MCHits
//=========================================================================
void VeloSim::fillFE(LHCb::MCVeloFE* myFE,
                     LHCb::MCHit* hit, double charge){
  //
  debug()<< " ==> fillFE() " <<endmsg;
  //
  myFE->setAddedSignal(myFE->addedSignal()+charge);
  // add link to MC hit / update with weight
  verbose() << "fillFE " << myFE << endmsg;
  //
  int size=myFE->NumberOfMCHits();
  int i=0;
  LHCb::MCHit* hitChk=NULL;
  while(hit!=hitChk && i<size){
    //
    verbose()<< "hit number " << i << " / " << size 
		         << " charge " << charge << " hit " << hit <<endmsg;
    hitChk=myFE->mcHit(i);  
    i++; 
  }; 
  i--;
  if(hit==hitChk){
    double sig=myFE->mcHitCharge(i);
    //
    verbose() << "hit exists has signal " << sig <<endmsg;
    //
    sig+=charge;
    myFE->setMCHitCharge(i,sig);
    //
    verbose()<< " new signal value " << sig << " for hit "
             << hit << " hit check " << hitChk << endmsg;
  }
  else{
    verbose()<< "hit added" << endmsg;
    myFE->addToMCHits(hit,charge);
  }
  return;
}
//=========================================================================
// update signal
//=========================================================================
void VeloSim::fillFE(LHCb::MCVeloFE* myFE,double charge){
  //
  debug()<< " ==> fillFE() " <<endmsg;
  //
  myFE->setAddedSignal(myFE->addedSignal()+charge);
  return;
}
//=========================================================================
// add a % of signal in strip to the two neighbouring strips
// it is assumed that this is a small % and hence it doesn't matter
// in what order this procedure is applied to the strip list.
//=========================================================================
StatusCode VeloSim::coupling(){
  //
  debug()<< " ==> coupling() " <<endmsg;
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
    verbose()<< "coupledSignal " << coupledSignal
		         << " orig " << (*FEIt)->addedSignal() <<endmsg;
    // subtract coupled signal from this strip
    (*FEIt)->setAddedSignal((*FEIt)->addedSignal()-2.*coupledSignal);
    //
    verbose()<< " subtracted " << (*FEIt)->addedSignal() <<endmsg;
    // add to previous strip (if doesn't exist then create)
    // apply charge threshold to determine if worth creating
    bool create = (coupledSignal > m_threshold*0.1);
    bool valid;
    LHCb::MCVeloFE* prevStrip=findOrInsertPrevStrip(FEIt,valid,create);
    if (valid) fillFE(prevStrip,coupledSignal);
    //
    verbose()<< " base " << (*FEIt)->strip() << " "
		         << (*FEIt)->sensor() << endmsg;
    if(valid) verbose()<< " prev " << prevStrip->strip()
                       << " " << prevStrip->sensor() << endmsg;
    // add to next strip
    LHCb::MCVeloFE* nextStrip=findOrInsertNextStrip(FEIt,valid,create);
    //
    verbose()<< " create " << create << " valid " << valid <<endmsg;
    //
    if (valid) fillFE(nextStrip,coupledSignal);
    //
    verbose()<< " base " << (*FEIt)->strip() << " "
		         << (*FEIt)->sensor() << endmsg;
    if (valid) verbose()<< " next " << nextStrip->strip() << " "
                        << nextStrip->sensor() <<endmsg;
  } // end of loop over hits
  // add any newly created FEs
  debug()<< "FEs created by coupling routine "
         << m_FEs_coupling->size() <<endmsg;
  for(LHCb::MCVeloFEs::iterator coupIt=m_FEs_coupling->begin();
       coupIt<m_FEs_coupling->end(); coupIt++){
    LHCb::MCVeloFE* myFE=m_FEs->object((*coupIt)->key());
    if( 0 != myFE ) {
      myFE->setAddedSignal( myFE->addedSignal() + (*coupIt)->addedSignal() );
      //
      verbose()<< " -- Existing FE. " << (*coupIt)->sensor() << ", "
		           << (*coupIt)->strip() << " Update with coupling FE" <<endmsg;
    }else{
      //
      verbose()<< " -- Add coupling FE " << (*coupIt)->sensor() << ", "
		           << (*coupIt)->strip() << endmsg;
      //
      m_FEs->insert(*coupIt);
    }
  }
  delete m_FEs_coupling;
  //
  debug()<< "Number of FEs after coupling simulation "
         << m_FEs->size() << endmsg;
  //
  return (StatusCode::SUCCESS);
}
//=========================================================================
// From an originally sorted list, find the strip with the previous key,
// or create a new one.
//=========================================================================
LHCb::MCVeloFE* VeloSim::findOrInsertPrevStrip(
                LHCb::MCVeloFEs::iterator FEIt, bool& valid, bool& create){
  //
  debug()<< " ==> findOrInsertPrevStrip() " <<endmsg;
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
      verbose()<< " create strip" << stripKey.strip() << " "
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
//=========================================================================
//
//=========================================================================
LHCb::MCVeloFE* VeloSim::findOrInsertNextStrip(
                LHCb::MCVeloFEs::iterator FEIt, bool& valid, bool& create){
  // From an originally sorted list, find the strip with the previous key,
  // or create a new one.
  debug()<< " ==> findOrInsertNextStrip() " <<endmsg;
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
      verbose()<< " create strip" << stripKey.strip() << " "
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
//=========================================================================
// add pedestal - not yet implemented
//=========================================================================
StatusCode VeloSim::pedestalSim(){
  return StatusCode::SUCCESS;
}
//=========================================================================
//
//=========================================================================
StatusCode VeloSim::noiseSim(){
  //
  debug()<< " ==> noiseSim() " <<endmsg;
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
  double stripCapacitance=m_averageStripCapacitance;
  // should be capacitance of each strip, currently just typical value
  for(LHCb::MCVeloFEs::iterator FEIt = m_FEs->begin() ;
        m_FEs->end() != FEIt ; FEIt++ ){
    if((*FEIt)->addedNoise()==0){
      double noise=noiseValue(stripCapacitance);
      (*FEIt)->setAddedNoise(noise);
      //
      verbose()<< " noise added to existing strip "
		           << (*FEIt)->addedNoise()<< endmsg;
    }
  }
  // allocate noise (above threshold) to channels that don't currently
  // have signal
  std::vector<DeVeloSensor*>::const_iterator sensBegin;
  std::vector<DeVeloSensor*>::const_iterator sensEnd;
  
  if (m_simMode=="velo") {
    sensBegin = m_veloDet->rPhiSensorsBegin();
    sensEnd   = m_veloDet->rPhiSensorsEnd();
  } else if (m_simMode=="pileUp") {
    sensBegin = m_veloDet->pileUpSensorsBegin();
    sensEnd   = m_veloDet->pileUpSensorsEnd();
  }  
  for(std::vector<DeVeloSensor*>::const_iterator iSens = sensBegin;
      iSens != sensEnd;
      ++iSens) {
    const DeVeloSensor* sens = *iSens;
    double noiseSig=noiseSigma(stripCapacitance);
    // use average capacitance of sensor, should be adequate if variation in
    // cap. not too large.
    // number of hits to add noise to (i.e. fraction above threshold)
    // add both large +ve and -ve noise.
    int maxStrips= sens->numberOfStrips();
    int hitNoiseTotal= 
        int(LHCbMath::round(2.*gsl_sf_erf_Q(m_threshold/noiseSig)
			  *float(maxStrips)));
    Rndm::Numbers poisson(randSvc(), Rndm::Poisson(hitNoiseTotal));
    hitNoiseTotal = int(poisson());
    //
    unsigned int sensorNo=sens->sensorNumber();
    verbose()<< "Number of strips to add noise to "
		         << hitNoiseTotal
             << " sensor Number " << sensorNo
		         << " maxStrips " << maxStrips
		         <<  " sigma of noise " << noiseSig
		         << " threshold " << m_threshold
             << " tail probability "
         		 << gsl_sf_erf_Q(m_threshold/noiseSig)
    		     << endmsg;
    //
    for(int noiseHit=0; noiseHit<hitNoiseTotal; noiseHit++){
      // choose random hit to add noise to
      // get strip number
      int stripArrayIndex=int(LHCbMath::round(m_uniformDist()*(maxStrips-1)));
      LHCb::VeloChannelID stripKey(sensorNo,stripArrayIndex);
      // find strip in list.
      LHCb::MCVeloFE* myFE = findOrInsertFE(stripKey);
      if (myFE->addedNoise()==0){
        double noise=noiseValueTail(stripCapacitance);
        myFE->setAddedNoise(noise);
        //
        verbose()<< "hit from tail of noise created "
                 << myFE->addedNoise() << endmsg;
      }else{
        // already added noise here - so generate another strip number
        noiseHit--;
      }
    }
  }
  debug()<< "Number of FEs after noise simulation "
         << m_FEs->size() <<endmsg;
  //
  return (StatusCode::SUCCESS);
}
//=========================================================================
// sigma of noise to generate
//=========================================================================
double VeloSim::noiseSigma(double stripCapacitance){
  //
  debug()<< " ==> noiseSigma() " <<endmsg;
  //
  double noiseSigma=stripCapacitance*m_noiseCapacitance+
    m_noiseConstant;
  //  
  return noiseSigma;
}
//=========================================================================
// generate some noise
//=========================================================================
double VeloSim::noiseValue(double stripCapacitance){
  //
  debug()<< " ==> noiseValue() " <<endmsg;
  //
  double noise=m_gaussDist()*noiseSigma(stripCapacitance);
  return noise;
}
//=========================================================================
// generate some noise from tail of distribution
//=========================================================================
double VeloSim::noiseValueTail(double stripCapacitance){
  //
  debug()<< "noiseValueTail() " <<endmsg;
  //
  double noiseSig=noiseSigma(stripCapacitance);
  double noise=ran_gaussian_tail(m_threshold,noiseSig);
  double sign=m_uniformDist();
  if (sign > 0.5) noise*=-1.; // noise negative or positive
  return noise;
}
//=========================================================================
// common mode - not yet implemented
//=========================================================================
StatusCode VeloSim::CMSim(){
  return StatusCode::SUCCESS;
}
//=========================================================================
// dead strips
//=========================================================================
StatusCode VeloSim::deadStrips(){
  //
  debug()<< " ==> deadStrips() " <<endmsg;
  // Add some strip inefficiency
  // channels are given zero signal, and hence will be removed by the 
  // threshold cut in final process.
  // e.g. set stripInefficiency to 0.01 for 1% dead channels
  for(LHCb::MCVeloFEs::iterator itF1 = m_FEs->begin(); m_FEs->end() != itF1;
        itF1++){
    double cut =  m_uniformDist();
    if ( m_stripInefficiency > cut ) {
      (*itF1)->setAddedSignal( 0. );
    }
  }
  //
  return (StatusCode::SUCCESS);
}
//=========================================================================
// find a strip in list of FEs, or if it does not currently exist create it
//=========================================================================
LHCb::MCVeloFE* VeloSim::findOrInsertFE(LHCb::VeloChannelID& stripKey){
  //
  debug()<< " ==> findOrInsertFE() " <<endmsg;
  //
  LHCb::MCVeloFE* myFE = m_FEs->object(stripKey);
  if (myFE==NULL) {
    // this strip has not been used before, so create
    myFE = new LHCb::MCVeloFE(stripKey);
    //
    verbose()<< " -- Add FE " << stripKey.sensor() << ","
		         << stripKey.strip() << endmsg;
    //
    debug()<< "size of FEs: " << m_FEs->size() <<endmsg;
    m_FEs->insert(myFE);
    debug()<< "size of FEs: " << m_FEs->size() <<endmsg;
    
  }
  //
  return (myFE);
}
//=========================================================================
// remove any MCFEs with charge below abs(threshold)
//=========================================================================
StatusCode VeloSim::finalProcess(){
  //
  debug()<< " ==> finalProcess() " <<endmsg;
  // cannot do this by remove_if, erase as storing/erasing pointers.
  // instead sort whole container and erase.
  std::sort(m_FEs->begin(), m_FEs->end(), 
            VeloEventFunctor::Less_by_charge<const LHCb::MCVeloFE*>());
  std::reverse(m_FEs->begin(),m_FEs->end());
  LHCb::MCVeloFEs::iterator it1=std::find_if(m_FEs->begin(),
                                         m_FEs->end(),
                                         chargeThreshold());
  LHCb::MCVeloFEs::iterator it2=m_FEs->end();
  m_FEs->erase(it1, it2);
  // sort FEs into order of ascending sensor + strip
  std::sort(m_FEs->begin(),m_FEs->end(),
            VeloEventFunctor::Less_by_key<const LHCb::MCVeloFE*>());
  //
  return (StatusCode::SUCCESS);
}
//=========================================================================
// store MCFEs
//=========================================================================
StatusCode VeloSim::storeOutputData(){
  //
  debug()<< " ==> storeOutputData() " <<endmsg;
  //
  // velo FEs
  // update FEs container adding the pileup FEs
  //  sc = eventSvc()->registerObject(m_outputContainer,m_veloFEs);
  debug() << " size of m_veloFE before update: " << m_veloFEs->size() << endmsg;
  //  
  if(m_pileUp){
    LHCb::MCVeloFEs::const_iterator feIt;
    for(feIt=m_pileUpFEs->begin(); feIt!=m_pileUpFEs->end(); feIt++){
      m_veloFEs->insert(*feIt);
    }
    debug()<< " size after update: " << m_veloFEs->size() <<endmsg;
  }
  //
  put(m_veloFEs, m_outputContainer);
  debug()<< "Stored " << m_veloFEs->size() << " MCVeloFEs at "
         << m_outputContainer <<endmsg;

  // pileup FEs
  if (m_pileUp){
    //    sc = eventSvc()->registerObject(m_pileUpOutputContainer,m_pileUpFEs);
    put(m_pileUpFEs, m_pileUpOutputContainer);
    debug()<< "Stored " << m_pileUpFEs->size() << " MCVeloFEs at "
           << m_pileUpOutputContainer << endmsg;
  }
  //
  return StatusCode::SUCCESS;
}
//=========================================================================
// delta ray tail random numbers
// dN/DE=k*1/E^2 for relativistic incident particle
// E(r)=1/r, where r is uniform in range 1/Tmin < r < 1/Tmax
// but Tmax bounded by energy left to allocate, so following is
// not truly correct
//=========================================================================
double VeloSim:: ran_inv_E2(double Tmin,double Tmax){
  //
  debug()<< " ==> ran_inv_E2() " <<endmsg;
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
  debug()<< " ==> ran_gaussian_tail() " <<endmsg;
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
//==============================================================================
double VeloSim::spillOverReminder(double TOF)
{
  debug()<< " ==> VeloSim::spillOverReminder " <<endmsg;
  //
  pulseShapeFunctor pulseShape;
  double time=0.;
  double chargeReminder=0.;
  // the time needed for light to travel distance of .75 m
  const double maxTOF=2.55; // ns
  if(TOF<=0.) return ( chargeReminder );
  if(TOF>maxTOF) TOF=maxTOF;
  //
  time=k_pulseShapePeakTime+k_spillOverTime+m_offPeakSamplingTime-TOF;
  chargeReminder=pulseShape(time, m_fitParams);
  //
  debug()<< "spillOverReminder= " << chargeReminder <<endmsg;
  //
  return ( chargeReminder );
}
//==============================================================================
bool VeloSim::checkConditions(LHCb::MCHit* aHit)
{
  debug()<< " ==> checkConditions(Hit) " <<endmsg;
  //
  const DeVeloSensor* sensor=m_veloDet->sensor(aHit->sensDetID());
  // check the conditions
  bool isReadOut=sensor->isReadOut();
  //
  return ( isReadOut );
}
//
