// $Id: VeloSim.cpp,v 1.32 2004-12-03 18:01:58 dhcroft Exp $
// Include files
// STL
#include <string>
#include <stdio.h>
#include <vector>
#include <math.h>

// clhep
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

//GSL
#include "gsl/gsl_sf_erf.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "Kernel/LHCbMath.h"

// from VeloEvent
#include "Event/MCVeloHit.h"
#include "Event/MCVeloFE.h"

// VeloDet
#include "VeloDet/DeVelo.h"
// from LHCbEvent
#include "Event/MCParticle.h"

// VeloKernel
#include "VeloKernel/VeloSimParams.h"

// local
#include "VeloSim.h"
#include "VeloAlgorithms/VeloEventFunctor.h"



//-----------------------------------------------------------------------------
// Implementation file for class : VeloSim
//
// 16/01/2002 : Chris Parkes
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<VeloSim>          Factory ;
const         IAlgFactory& VeloSimFactory = Factory ;

struct chargeThreshold : public std::unary_function<MCVeloFE*,  bool> {
  bool operator()(MCVeloFE* FE) {
    // decide to cut out large -ve noise
    return FE->charge() < VeloSimParams::threshold;
  }
};

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloSim::VeloSim( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_inputContainer       ( MCVeloHitLocation::Default )
  , m_spillOverInputContainer ( "Prev/" + m_inputContainer )
  , m_pileUpInputContainer    ( MCVeloHitLocation::PuVeto )
  , m_pileUpSpillOverInputContainer ( "Prev/" + m_pileUpInputContainer )
  , m_outputContainer      ( MCVeloFELocation::Default )
  , m_pileUpOutputContainer   ( MCVeloFELocation::PuVeto )
  , m_chargeSim            ( true )
  , m_inhomogeneousCharge  ( true )
  , m_coupling             ( true )
  , m_noiseSim             ( true )
  , m_noiseConstant        (VeloSimParams::noiseConstant)
  , m_pedestalSim          ( true )
  , m_CMSim                ( true )
  , m_stripInefficiency    ( 0.0 )
  , m_spillOver            ( true )
  , m_pileUp               ( true )
  , m_testSim              ( false )
  , m_smearPosition        ( 0.0 )
{
  declareProperty( "InputContainer"      ,m_inputContainer  );
  declareProperty( "SpillOverInputData"  ,m_spillOverInputContainer  );
  declareProperty( "PileUpInputContainer" ,m_pileUpInputContainer  );
  declareProperty( "PileUpSpillOverInputData",m_pileUpSpillOverInputContainer);
  declareProperty( "OutputContainer"     ,m_outputContainer );
  declareProperty( "PileUpOutputContainer" , m_pileUpOutputContainer );
  declareProperty( "ChargeSim"           ,m_chargeSim );
  declareProperty( "InhomogeneousCharge" ,m_inhomogeneousCharge );
  declareProperty( "Coupling"            ,m_coupling );
  declareProperty( "NoiseSim"            ,m_noiseSim );
  declareProperty( "NoiseConstant"       ,m_noiseConstant);
  declareProperty( "PedestalSim"         ,m_pedestalSim );
  declareProperty( "CMSim"               ,m_CMSim );
  declareProperty( "StripInefficiency"   ,m_stripInefficiency );
  declareProperty( "SpillOver"           ,m_spillOver );
  declareProperty( "PileUp"              ,m_pileUp );
  declareProperty( "TestSimulation"      ,m_testSim );
  declareProperty( "SmearPosition"       ,m_smearPosition );

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

  debug() << "==> Initialise" << endmsg;

  m_velo=getDet<DeVelo>("/dd/Structure/LHCb/Velo" );

  // calculation of diffusion parameter
  m_baseDiffuseSigma=sqrt(2*VeloSimParams::kT/VeloSimParams::biasVoltage);

  // random number initialisation
  StatusCode scr1=m_gaussDist.initialize( randSvc(), Rndm::Gauss(0.,1.0));
  StatusCode scr2=m_uniformDist.initialize( randSvc(), Rndm::Flat(0.,1.0));
  if ( !(scr1&scr2) ) {
    error() << "Random number init failure" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSim::execute() {

  debug() << "==> Execute" << endmsg;

  StatusCode sc;
  sc=getInputData(); // get Velo MCHits, pile-up MCHits and spillOver data

  if (sc){
    // velo simulation
    m_hits = m_veloHits;
    m_spillOverHits=NULL;
    if (0!=m_veloSpillOverHits) m_spillOverHits = m_veloSpillOverHits;
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

  if (sc) sc= storeOutputData(); // add MCFEs to TDS

  return sc;
}

StatusCode VeloSim::simulation() {
  // perform simulation
  StatusCode sc;

  // simulate signals in strips from GEANT hits of current event
  if (m_chargeSim) sc= chargeSim(false); 
  // simulate signals in strips from GEANT hits of spillOver event
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

  return sc;
}

//=========================================================================
//
//=========================================================================

StatusCode VeloSim::getInputData() {

  bool isDebug   = msgLevel( MSG::DEBUG   );

  // *** get the velo input data
  if(isDebug) debug() << "Retrieving MCVeloHits from " << m_inputContainer
		      << endmsg;
  m_veloHits=get<MCVeloHits>( m_inputContainer );

  if(isDebug) debug() << m_veloHits->size() << " hits retrieved" << endmsg;

  // *** get the pile-up input data
  if (m_pileUp) {
    if(isDebug) debug() << "Retrieving MCVeloHits from " 
			<< m_pileUpInputContainer << endmsg;
      
    m_pileUpHits = get<MCVeloHits>(m_pileUpInputContainer );

    if(isDebug) debug() << m_pileUpHits->size()
			<< " pile-up hits retrieved" << endmsg;
    //     for (MCVeloHits::const_iterator hitIt=m_pileUpHits->begin(); 
    //           hitIt < m_pileUpHits->end(); hitIt++){
    //        // add 100 to pileup sensors number
    //        (*hitIt)->setSensor((*hitIt)->sensor()+100);
    //        if(isDebug) debug() << " pileup sensors " << (*hitIt)->sensor()<<endmsg;
    //      }

  }
    
  // *** get the velo SpillOver input data
  if (m_spillOver){
    if(isDebug) debug() << "Retrieving MCVeloHits of SpillOver Event from "
			<< m_spillOverInputContainer << endmsg;
      
    if( !exist<MCVeloHits>(m_spillOverInputContainer) ) {
      if(isDebug) debug()
	<< "Spill over event not present unable to retrieve input container="
	<< m_spillOverInputContainer << endmsg;
      m_veloSpillOverHits = 0;
    } else { 
      m_veloSpillOverHits = get<MCVeloHits>(m_spillOverInputContainer);
      if(isDebug) debug() << m_veloSpillOverHits->size()
			  << " spill over hits retrieved" << endmsg;
    }
  }

  // *** get the pileUp SpillOver input data
  if (m_spillOver&&m_pileUp){
    if(isDebug) debug() 
      << "Retrieving MCVeloHits of Pile Up SpillOver Event from "
      << m_pileUpSpillOverInputContainer << endmsg;
      
    if( !exist<MCVeloHits>(m_pileUpSpillOverInputContainer) ){      
      if(isDebug) debug()
	<< "Spill over event for PileUp not present unable to retrieve " 
	<< "input data container=" << m_spillOverInputContainer << endmsg;
      m_pileUpSpillOverHits=0;
    } else {
      m_pileUpSpillOverHits=get<MCVeloHits>(m_pileUpSpillOverInputContainer);
      if(isDebug) debug() << m_pileUpSpillOverHits->size()
			  << " PileUp Spill over hits retrieved" << endmsg;
      //       for (MCVeloHits::const_iterator hitIt=m_pileUpSpillOverHits->begin(); 
      //             hitIt < m_pileUpSpillOverHits->end(); hitIt++){
      //          // add 100 to pileup sensors number
      //          (*hitIt)->setSensor((*hitIt)->sensor()+100);
      //          if(isDebug) debug() << " pileup sensors " << (*hitIt)->sensor() 
      //              << endmsg;
      //        }
    }
  }
    
  // *** make vectors for output
    
  if (m_pileUp) {m_pileUpFEs = new MCVeloFEs();}
  else          {m_pileUpFEs =NULL;}

  m_veloFEs = new MCVeloFEs();

  return StatusCode::SUCCESS;
}

//=========================================================================
// loop through hits allocating the charge to strips
//=========================================================================
StatusCode VeloSim::chargeSim(bool spillOver) {
 
  bool isDebug   = msgLevel( MSG::DEBUG   );
  bool isVerbose = msgLevel( MSG::VERBOSE );

  debug() << "===> Charge Simulation";

  if (spillOver) debug() << " for spill over Event";
  debug() << endreq;

  MCVeloHits* hits;
  if (!spillOver){
    // hits from current event
    hits=m_hits;
  } else{
    // hits from spill Over Event. Return if no spillover
    if( 0 == m_spillOverHits ) return StatusCode::SUCCESS;
    hits=m_spillOverHits;
  }

  if(isDebug) debug() << "Number of hits to simulate=" 
		      << hits->size() << endmsg;

  //loop over input hits
  for ( MCVeloHits::const_iterator hitIt = hits->begin() ;
        hits->end() != hitIt ; hitIt++ ) {
    MCVeloHit* hit = (*hitIt);


    // degrade resolution for April 2003 Robustness Test
    if (m_smearPosition>0) {
      m_movePosition.setX(m_gaussDist()*m_smearPosition);
      m_movePosition.setY(m_gaussDist()*m_smearPosition);  
      m_movePosition.setZ(0.);
    } else{
      m_movePosition.setX(0.);
      m_movePosition.setY(0.);  
      m_movePosition.setZ(0.);
    }

    if (m_testSim) { testSim(hit,spillOver);}
    else{
      // calculate a set of points in the silicon
      //with which the simulation will work
      int NPoints = simPoints(hit);
      if( isVerbose ) {
        verbose() << "Simulating " << NPoints
		  << " points in Si for this hit" << endmsg;
      }
      if (NPoints>0){
        // calculate charge to assign to each point
        // taking account of delta ray inhomogeneities
        std::vector<double> sPoints(NPoints);
        chargePerPoint(*hitIt,NPoints,sPoints,spillOver);

        // diffuse charge from points to strips
        diffusion(*hitIt,NPoints,sPoints,spillOver);
      }
    }
  }

  if(isDebug) debug() << "Number of MCVeloFEs "
		      << m_FEs->size() << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
// test simulation - allocate all charge to entry/exit point strip
//=========================================================================
void VeloSim::testSim(MCVeloHit* hit,bool spillOver){
  // test routine - simplest possible simulation.
  double EntryFraction;
  double pitch;
  StatusCode sc;VeloChannelID entryChan;
  if (m_uniformDist()>0.5){
    sc=m_velo->pointToChannel(hit->entry(), entryChan, EntryFraction, pitch);
  }
  else{
    sc=m_velo->pointToChannel(hit->exit(), entryChan, EntryFraction, pitch);
  }
  double charge=(hit->energy()/eV)/VeloSimParams::eVPerElectron;
  if (spillOver) charge*=VeloSimParams::spillOverChargeFraction;

  MCVeloFE* myFE = findOrInsertFE(entryChan);
  fillFE(myFE,hit,charge);
}


//=========================================================================
// calculate how many points in the silicon the simulation will be performed at
//=========================================================================
long VeloSim::simPoints(MCVeloHit* hit){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  double EntryFraction=0.,ExitFraction=0.;
  double pitch=0.;
  StatusCode EntryValid, ExitValid;
  VeloChannelID entryChan, exitChan;
  EntryValid = m_velo->pointToChannel(hit->entry(),entryChan,EntryFraction,
                                      pitch);
  ExitValid=m_velo->pointToChannel(hit->exit(),exitChan,ExitFraction,
                                   pitch);
  if( isVerbose ) {
    verbose() << "calculate number of points to simulate in Si"
	      << endmsg;
    verbose() << "entry/exit points "
	      << entryChan.sensor() << " " << entryChan.strip()
	      <<  " + " << EntryFraction
	      << " / " <<  exitChan.sensor()  << " " << exitChan.strip()
	      <<  " + " << ExitFraction;
    if (!EntryValid) verbose() << " invalid entry point";
    if (!ExitValid) verbose() << " invalid exit point";
    verbose() <<endmsg;
  }
  
  double NPoints=0.;
  if (EntryValid&&ExitValid){
    // both entry and exit are at valid strip numbers,
    // calculate how many full strips apart
    int INeighb;
    StatusCode sc=m_velo->channelDistance(entryChan,exitChan,INeighb);
    if (sc) NPoints = fabs(float(INeighb)-(EntryFraction-ExitFraction));
    if( isVerbose ) {
      verbose() << "Integer number of strips apart " << INeighb
		<< " floating number " << NPoints << endmsg;
    }
  }
  else {
    // either entry or exit or both are invalid, ignore this hit
    NPoints=0.;
    if( isVerbose && (EntryValid!=ExitValid) ) {
      verbose()
	<< "simPoints: only one of entry and exit point of hit are in "
	<< "silicon - hit ignored " << endmsg;
    }
  }

  return int(ceil(NPoints)*VeloSimParams::simulationPointsPerStrip);
}

//=========================================================================
// allocate charge to points
//=========================================================================
void VeloSim::chargePerPoint(MCVeloHit* hit, int Npoints,
                             std::vector<double>& Spoints, bool spillOver){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  // total charge in electrons
  double charge=(hit->energy()/eV)/VeloSimParams::eVPerElectron;
  if (spillOver) charge*=VeloSimParams::spillOverChargeFraction;
  if( isVerbose ) {
    verbose() << "calculating charge per simulation point" << endmsg;
    verbose() << "total charge " << charge
	      << " in eV " << hit->energy()/eV;
    if (spillOver) verbose()  << " for spill over event ";
    verbose() << endmsg;
  }
  
  // charge to divide equally
  double chargeEqual;
  if (m_inhomogeneousCharge){
    // some of charge allocated by delta ray algorithm
    chargeEqual=VeloSimParams::chargeUniform*
      m_velo->siliconThickness(1)/micron;
    //      m_velo->siliconThickness(hit->sensor())/micron;
    // sensor numbers for hits currently incorrect (7/02) 
    // nasty fudge - use one hardcoded number

    if (spillOver) chargeEqual*=VeloSimParams::spillOverChargeFraction;
    if (chargeEqual>charge)  chargeEqual=charge;
  }

  else{
    // all of charge allocated equally to points
    chargeEqual=charge;
  }
  if( isVerbose ) {
    verbose() << "total charge " << charge
	      << " charge for equal allocation " << chargeEqual << endmsg;
  }
  
  // divide equally
  double chargeEqualN=chargeEqual/Npoints;
  double fluctuate=0.;
  for (int i=0; i<Npoints; i++){
    // gaussian fluctuations
    if (m_inhomogeneousCharge) fluctuate=m_gaussDist()*sqrt(chargeEqualN);
    Spoints[i]=chargeEqualN+fluctuate;
    if( isVerbose ) {
      verbose() << "charge for pt" << i << " is " << Spoints[i] 
		<< endmsg;
    }
  }

  // inhomogeneous charge dist from delta rays
  if (m_inhomogeneousCharge){
    deltaRayCharge(charge-chargeEqual, 0.001*charge, Npoints, Spoints);
    // ensure total charge is allocated
    double total=0.;
    for (int i=0; i<Npoints; i++){total+=Spoints[i];}
    if( 1.e-10 < total ) {
      double adjust=charge/total;
      for (int j=0; j<Npoints; j++){Spoints[j]*=adjust;}
    }
  }

  return;
}


//=========================================================================
// allocate remaining charge from delta ray distribution
//=========================================================================
void VeloSim::deltaRayCharge(double charge, double tol,
                             int Npoints, std::vector<double>& Spoints){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  double Tmax= charge;// upper limit on charge of delta ray
  double Tmin= VeloSimParams::deltaRayMinEnergy/VeloSimParams::eVPerElectron;
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
    if( isVerbose ) {
      verbose() << " delta ray charge added to point " << ipt
		<< "/" << Npoints << endmsg;
    }
    Spoints[ipt]+=charge;
    Tmax-=charge;
  }
  return;
}


//=========================================================================
// allocate the charge to the collection strips
//=========================================================================
void VeloSim::diffusion(MCVeloHit* hit,int Npoints,
                        std::vector<double>& Spoints,bool spillOver){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  if( isVerbose ) {
    verbose() << "diffusion of charge from simulation points"
	      << endmsg; 
  }
  HepVector3D path = (hit->exit())-(hit->entry());
  path/=(Npoints*2); // distance between steps on path
  HepPoint3D point= (hit)->entry()+path+m_movePosition;
  // double thickness=m_velo->siliconThickness(hit->sensor())/micron;
  // sensor numbers for hits currently incorrect (7/02) 
  // nasty fudge - use one hardcoded number
  double thickness=m_velo->siliconThickness(1)/micron;
  double ZDiffuse=thickness;
  // assume strips are at opposite side of Si to entry point
  double dz=ZDiffuse/ (double(Npoints)*2.); // distance between steps on path
  ZDiffuse-=dz;

  for (int ipt=0; ipt<Npoints; ipt++){ //loop over points on path
    double fraction,pitch;
    if( isVerbose ) {
      verbose() << " ipt " << ipt << " point " << point << endmsg;
    }
    //calculate point on path
    VeloChannelID entryChan;
    StatusCode valid=m_velo->pointToChannel(point,entryChan,fraction,pitch);

    if ( !valid){
      if( isVerbose ) { 
	verbose() << " point is not in active silicon " << point << 
	  " entry " << hit->entry() << " exit " <<  hit->exit() << endmsg;
      }
    }

    if (valid) {
      // a point may be invalid, despite entry and exit points being valid,
      // as it lies in say a bias rail dead area.
      // charge from this point is considered lost.

      if( isVerbose ) {
        verbose() << "chan " << entryChan.strip() << " fraction "
		  << fraction << " pitch " << pitch << " valid " << valid << endmsg;
      }
      const int neighbs=1; // only consider =/- this many neighbours
      double chargeFraction[2*neighbs+1];
      double totalFraction=0.;
      // loop over neighbours per point
      int iNg;
      for  (iNg=-neighbs; iNg<=+neighbs; iNg++){
        double diffuseDist1=((iNg-0.5)-fraction)*pitch/micron;
        double diffuseDist2=((iNg+0.5)-fraction)*pitch/micron;
        //      double diffuseDist1=((iNg)-fraction)*pitch/micron;
        //      double diffuseDist2=((iNg+1.)-fraction)*pitch/micron;
        double diffuseSigma=m_baseDiffuseSigma*sqrt(thickness*ZDiffuse);
        if( isVerbose ) {
          verbose() << "diffuseDist1 " << diffuseDist1
		    <<   " diffuseDist2 " << diffuseDist2 << " diffuseSigma "
		    << diffuseSigma << " base " << m_baseDiffuseSigma
		    << " zdiff " << ZDiffuse << endmsg;
        }

        double prob1= gsl_sf_erf_Q(diffuseDist1/diffuseSigma);
        double prob2= gsl_sf_erf_Q(diffuseDist2/diffuseSigma);
        if( isVerbose ) {
          verbose() << " prob1+2 " <<  prob1 << " " << prob2 << endmsg;
        }
        int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
        chargeFraction[i]=fabs(prob1-prob2);
        totalFraction+= fabs(prob1-prob2);
        if( isVerbose ) {
          verbose() << i << " iNg " << iNg << " cfrac "
		    << chargeFraction[i]  << " tot " << totalFraction << endmsg;
        }
      }

      // renormalise allocated fractions to 1., and update strip signals
      for  (iNg=-neighbs; iNg<=+neighbs; iNg++ ){
        int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
        // if(isDebug) debug() << i << " iNg " << iNg << " ipt " << ipt
        //      << " " << endmsg;
        double charge=Spoints[ipt]*(chargeFraction[i]/totalFraction);
        //  if(isDebug) debug() << i << " ipt " << ipt << " charge "
        //      << charge << endmsg;
        if (charge>VeloSimParams::threshold*0.1){
          // ignore if below 10% of threshold
          // calculate index of this strip
          VeloChannelID stripKey;
          valid = m_velo->neighbour(entryChan,iNg,stripKey);
          // if(isDebug) debug() << " neighbour " << entryChan.strip() << " "
          //     << stripKey.strip() << " iNg " << iNg << endmsg;
          // update charge and MCHit list
          if (valid){
            MCVeloFE* myFE = findOrInsertFE(stripKey);
            if (!spillOver) fillFE(myFE,hit,charge); // update and add MC link
            else fillFE(myFE,charge); // update, no MC link
          }
        }
      } // neighbours loop
    } // valid point
    point+=2*path; // update to look at next point on path
    ZDiffuse-=2.*dz;
  } // loop over points

  return;
}


//=========================================================================
// update signal and list of MCHits
//=========================================================================
void VeloSim::fillFE(MCVeloFE* myFE, MCVeloHit* hit, double charge){

  bool isVerbose = msgLevel( MSG::VERBOSE );

  myFE->setAddedSignal(myFE->addedSignal()+charge);
  // add link to MC hit / update with weight
  if( isVerbose ) verbose() << "fillFE " << myFE << endmsg;
  int size=myFE->NumberOfMCVeloHits();
  int i=0;
  MCVeloHit* hitChk=NULL;
  while (hit!=hitChk && i<size) { 
    if( isVerbose ) {
      verbose() << "hit number " << i << " / " << size 
		<< " charge " << charge << " hit " << hit << endmsg;
    }
    hitChk = myFE->mcVeloHit(i);  
    i++; 
  }; 
  i--;
  if (hit==hitChk){
    double sig=myFE->mcVeloHitCharge(i);
    if( isVerbose ) verbose() << "hit exists has signal " << sig; 
    sig+=charge;
    myFE->setMCVeloHitCharge(i,sig);
    if( isVerbose ){
      verbose() << " new signal value " << sig << " for hit "
		<< hit << " hit check " << hitChk << endmsg;
    }
  }
  else{
    if( isVerbose ) verbose() << "hit added" << endmsg;
    myFE->addToMCVeloHits(hit,charge);
  }
  return;
}

//=========================================================================
// update signal
//=========================================================================
void VeloSim::fillFE(MCVeloFE* myFE,double charge){
  myFE->setAddedSignal(myFE->addedSignal()+charge);
  return;
}

//=========================================================================
// add a % of signal in strip to the two neighbouring strips
// it is assumed that this is a small % and hence it doesn't matter
// in what order this procedure is applied to the strip list.
//=========================================================================
StatusCode VeloSim::coupling(){

  bool isDebug = msgLevel( MSG::DEBUG );
  bool isVerbose = msgLevel( MSG::VERBOSE );
  if(isDebug) debug() <<  "--- strip coupling ------" << endmsg;
  // sort FEs into order of ascending sensor + strip
  std::stable_sort(m_FEs->begin(),m_FEs->end(),
                   VeloEventFunctor::Less_by_key<const MCVeloFE*>());

  // make new container for any added strips
  m_FEs_coupling=new MCVeloFEs();

  for ( MCVeloFEs::iterator FEIt = m_FEs->begin() ;
        m_FEs->end() != FEIt ; FEIt++ ) {

    // calculate signal to couple to neighbouring strips
    double coupledSignal=(*FEIt)->addedSignal()*VeloSimParams::coupling;
    if( isVerbose ) {
      verbose() << "coupledSignal " <<   coupledSignal
		<< " orig " << (*FEIt)->addedSignal() << endmsg;
    }

    // subtract coupled signal from this strip
    (*FEIt)->setAddedSignal((*FEIt)->addedSignal()-2.*coupledSignal);
    if( isVerbose ) {
      verbose() << " subtracted " << (*FEIt)->addedSignal() << endmsg;
    }

    // add to previous strip (if doesn't exist then create)
    // apply charge threshold to determine if worth creating
    bool create = (coupledSignal > VeloSimParams::threshold*0.1);
    bool valid;
    MCVeloFE* prevStrip=findOrInsertPrevStrip(FEIt,valid,create);
    if (valid) fillFE(prevStrip,coupledSignal);

    if( isVerbose ) {
      verbose() << " base " << (*FEIt)->strip() << " "
		<< (*FEIt)->sensor() << endmsg;
      if(valid) verbose() << " prev " << prevStrip->strip()
			  << " " << prevStrip->sensor() << endmsg;
    }

    // add to next strip
    MCVeloFE* nextStrip=findOrInsertNextStrip(FEIt,valid,create);
    if( isVerbose ) {
      verbose() << " create " << create << " valid " << valid
		<< endmsg;
    }
    if (valid) fillFE(nextStrip,coupledSignal);

    if( isVerbose ) {
      verbose() << " base " << (*FEIt)->strip() << " "
		<< (*FEIt)->sensor() << endmsg;
      if (valid) verbose() << " next " << nextStrip->strip() << " "
			   << nextStrip->sensor() << endmsg;
    }

  } // end of loop over hits

  // add any newly created FEs
  if(isDebug) debug() << "FEs created by coupling routine "
		      << m_FEs_coupling->size() << endmsg;
  for (MCVeloFEs::iterator coupIt=m_FEs_coupling->begin();
       coupIt<m_FEs_coupling->end(); coupIt++){
    MCVeloFE* myFE = m_FEs->object( (*coupIt)->key() );
    if ( 0 != myFE ) {
      myFE->setAddedSignal( myFE->addedSignal() + (*coupIt)->addedSignal() );
      if( isVerbose ) {
        verbose() << " -- Existing FE. "
		  << (*coupIt)->sensor() << ","
		  << (*coupIt)->strip() << " Update with coupling FE." << endmsg;
      }
    } else {
      if( isVerbose ) {
        verbose() << " -- Add coupling FE "
		  << (*coupIt)->sensor() << ","
		  << (*coupIt)->strip() << endmsg;
      }
      m_FEs->insert(*coupIt);
    }
  }
  delete m_FEs_coupling;
  if(isDebug) debug() << "Number of FEs after coupling simulation "
		      << m_FEs->size() << endmsg;

  return StatusCode::SUCCESS;
}


//=========================================================================
// From an originally sorted list, find the strip with the previous key,
// or create a new one.
//=========================================================================
MCVeloFE* VeloSim::findOrInsertPrevStrip(MCVeloFEs::iterator FEIt,
                                         bool& valid, bool& create){
  bool isVerbose = msgLevel( MSG::VERBOSE );
  // try previous entry in container
  MCVeloFE* prevStrip=(*FEIt);
  if (FEIt!=m_FEs->begin()){
    FEIt--;
    prevStrip=(*(FEIt));
    FEIt++;
  }
  // check this
  int checkDistance;
  StatusCode sc=m_velo->channelDistance((*FEIt)->key(),prevStrip->key(),
                                        checkDistance);
  valid = sc.isSuccess();
  bool exists=(-1 == checkDistance && valid);
  if(exists) return prevStrip;

  // check if just added this strip in other container
  if (m_FEs_coupling->size()!=0){
    MCVeloFEs::iterator last=m_FEs_coupling->end(); last--;
    prevStrip=(*last);
  }
  // check this
  sc=m_velo->channelDistance((*FEIt)->key(),prevStrip->key(),
                             checkDistance);
  valid = sc.isSuccess();
  exists=(-1 == checkDistance && valid);
  if(exists) return prevStrip;
  
  // doesn't exist so insert a new strip (iff create is true)
  if (create){
    VeloChannelID stripKey;
    sc = m_velo->neighbour((*FEIt)->key(),-1,stripKey);
    if(sc.isSuccess()){
      //== Protect if key already exists ==
      prevStrip = m_FEs_coupling->object(stripKey);
      if ( 0 != prevStrip ) return prevStrip;
      if( isVerbose ) {
        verbose() << " create strip" << stripKey.strip() << " "
		  << stripKey.sensor() << endmsg;
      }
      prevStrip = new MCVeloFE(stripKey);
      m_FEs_coupling->insert(prevStrip);
      valid=true;
    }
    else{
      valid=false;
      prevStrip=NULL;
    }
  }
  else{
    valid=false;
    prevStrip=NULL;
  }
  return prevStrip;
}

//=========================================================================
//
//=========================================================================
MCVeloFE* VeloSim::findOrInsertNextStrip(MCVeloFEs::iterator FEIt,
                                         bool& valid, bool& create){
  // From an originally sorted list, find the strip with the previous key,
  // or create a new one.
  bool isVerbose = msgLevel( MSG::VERBOSE );

  // try next entry in container
  MCVeloFE* nextStrip=*FEIt;
  MCVeloFEs::iterator last = m_FEs->end(); last--;
  if (FEIt!=last){
    FEIt++;
    nextStrip=(*(FEIt));
    FEIt--;
  }

  // check this
  // check this
  int checkDistance;
  StatusCode sc=m_velo->channelDistance((*FEIt)->key(),nextStrip->key(),
					checkDistance);
  valid = sc.isSuccess();
  bool exists=(1 == checkDistance && valid);
  if(exists) return nextStrip;

  // doesn't exist so insert a new strip (iff create is true)
  if (create){
    VeloChannelID stripKey;
    sc=m_velo->neighbour((*FEIt)->key(),+1,stripKey);
    if(sc.isSuccess()){
      //== Protect if key already exists ==
      nextStrip = m_FEs_coupling->object(stripKey);
      if ( 0 != nextStrip ) return nextStrip;
      if( isVerbose ) {
        verbose() << " create strip" << stripKey.strip() << " "
		  << stripKey.sensor() << endmsg;
      }
      nextStrip = new MCVeloFE(stripKey);
      m_FEs_coupling->insert(nextStrip);
      valid=true;
    }
    else{
      valid=false;
      nextStrip=NULL;
    }
  }
  else{
    valid=false;
    nextStrip=NULL;
  }

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

  bool isDebug   = msgLevel( MSG::DEBUG   );
  bool isVerbose = msgLevel( MSG::VERBOSE );

  if(isDebug) debug() << "===> Noise simulation" << endmsg;
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
  double stripCapacitance=VeloSimParams::averageStripCapacitance;
  // should be capacitance of each strip, currently just typical value

  for ( MCVeloFEs::iterator FEIt = m_FEs->begin() ;
        m_FEs->end() != FEIt ; FEIt++ ) {
    if ((*FEIt)->addedNoise()==0){
      double noise=noiseValue(stripCapacitance);
      (*FEIt)->setAddedNoise(noise);
      if( isVerbose ) {
        verbose() << " noise added to existing strip "
		  << (*FEIt)->addedNoise()<< endmsg;
      }
    }
  }

  // allocate noise (above threshold) to channels that don't currently
  // have signal
  int maxSensor=0;
  int minSensor=0;
  if (m_simMode=="velo") {
    minSensor=m_velo->numberPileUpSensors();
    maxSensor=m_velo->numberSensors();
  } else if (m_simMode=="pileUp") {
    maxSensor=m_velo->numberPileUpSensors();
  }
  
  for (int iSensorArrayIndex=minSensor; iSensorArrayIndex< maxSensor;
       iSensorArrayIndex++){
    unsigned int sensor=m_velo->sensorNumber(iSensorArrayIndex);

    double noiseSig=noiseSigma(stripCapacitance);
    // use average capacitance of sensor, should be adequate if variation in
    // cap. not too large.
    // number of hits to add noise to (i.e. fraction above threshold)
    // add both large +ve and -ve noise.
    int maxStrips= m_velo->numberStrips(sensor);
    int hitNoiseTotal= 
      int(LHCbMath::round(2.*gsl_sf_erf_Q(VeloSimParams::threshold/noiseSig)
			  *float(maxStrips)));
    Rndm::Numbers poisson(randSvc(), Rndm::Poisson(hitNoiseTotal));
    //    info() << " poisson" << poisson() << endmsg;
    hitNoiseTotal = int(poisson());

    if( isVerbose ) {
      verbose() << "Number of strips to add noise to "
		<< hitNoiseTotal
		<< " sensor Number " << sensor
		<< " maxStrips " << maxStrips
		<<  " sigma of noise " << noiseSig
		<< " threshold " << VeloSimParams::threshold
		<< " tail probability "
		<< gsl_sf_erf_Q(VeloSimParams::threshold/noiseSig)
		<< endmsg;
    }
    for (int noiseHit=0; noiseHit<hitNoiseTotal; noiseHit++){
      // choose random hit to add noise to
      // get strip number
      int stripArrayIndex=int(LHCbMath::round(m_uniformDist()*(maxStrips-1)));
      VeloChannelID stripKey(sensor,stripArrayIndex);
      // find strip in list.
      MCVeloFE* myFE = findOrInsertFE(stripKey);
      if (myFE->addedNoise()==0){
        double noise=noiseValueTail(stripCapacitance);
        myFE->setAddedNoise(noise);
        if( isVerbose ) {
          verbose() << "hit from tail of noise created "
		    << myFE->addedNoise() << endmsg;
        }
      }
      else{
        // already added noise here - so generate another strip number
        noiseHit--;
      }
    }

  }
  if(isDebug) debug() << "Number of FEs after noise simulation "
		      << m_FEs->size() << endmsg;


  return StatusCode::SUCCESS;
}



//=========================================================================
// sigma of noise to generate
//=========================================================================
double VeloSim::noiseSigma(double stripCapacitance){
  double noiseSigma=stripCapacitance*VeloSimParams::noiseCapacitance+
    m_noiseConstant;
  return noiseSigma;
}

//=========================================================================
// generate some noise
//=========================================================================
double VeloSim::noiseValue(double stripCapacitance){
  double noise=m_gaussDist()*noiseSigma(stripCapacitance);
  return noise;
}


//=========================================================================
// generate some noise from tail of distribution
//=========================================================================
double VeloSim::noiseValueTail(double stripCapacitance){
  double noiseSig=noiseSigma(stripCapacitance);
  double noise=ran_gaussian_tail(VeloSimParams::threshold,noiseSig);
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

  // Add some strip inefficiency
  // channels are given zero signal, and hence will be removed by the 
  // threshold cut in final process.
  // e.g. set stripInefficiency to 0.01 for 1% dead channels
  for ( MCVeloFEs::iterator itF1 = m_FEs->begin(); m_FEs->end() != itF1;
        itF1++) {
    double cut =  m_uniformDist();
    if ( m_stripInefficiency > cut ) {
      (*itF1)->setAddedSignal( 0. );
    }
  }
  return StatusCode::SUCCESS;
}


//=========================================================================
// find a strip in list of FEs, or if it does not currently exist create it
//=========================================================================
MCVeloFE* VeloSim::findOrInsertFE(VeloChannelID& stripKey){
  bool isVerbose = msgLevel( MSG::VERBOSE );
  MCVeloFE* myFE = m_FEs->object(stripKey);
  if (myFE==NULL) {
    // this strip has not been used before, so create
    myFE = new MCVeloFE(stripKey);
    if( isVerbose ) {
      verbose() << " -- Add FE " << stripKey.sensor() << ","
		<< stripKey.strip() << endmsg;
    }
    m_FEs->insert(myFE);
  }
  return myFE;
}

//=========================================================================
// remove any MCFEs with charge below abs(threshold)
//=========================================================================
StatusCode VeloSim::finalProcess(){
  // cannot do this by remove_if, erase as storing/erasing pointers.
  // instead sort whole container and erase.
  std::sort(m_FEs->begin(), m_FEs->end(), 
            VeloEventFunctor::Less_by_charge<const MCVeloFE*>());
  std::reverse(m_FEs->begin(),m_FEs->end());
  MCVeloFEs::iterator it1 = std::find_if(m_FEs->begin(),
                                         m_FEs->end(),
                                         chargeThreshold());
  MCVeloFEs::iterator it2 = m_FEs->end();
  m_FEs->erase(it1, it2);

  // sort FEs into order of ascending sensor + strip
  std::sort(m_FEs->begin(),m_FEs->end(),
            VeloEventFunctor::Less_by_key<const MCVeloFE*>());

  return StatusCode::SUCCESS;
}


//=========================================================================
// store MCFEs
//=========================================================================
StatusCode VeloSim::storeOutputData(){
  bool isDebug   = msgLevel( MSG::DEBUG   );

  StatusCode sc;
  // velo FEs
  sc = eventSvc()->registerObject(m_outputContainer,m_veloFEs);

  if ( sc ) {
    if(isDebug) debug() << "Stored " << m_veloFEs->size() << " MCVeloFEs at "
			<< m_outputContainer << endmsg;
  }
  else{
    error() << "Unable to store MCVeloFEs at "
	    << m_outputContainer << endmsg;
  }

  // pileup FEs

  if (m_pileUp){
    sc = eventSvc()->registerObject(m_pileUpOutputContainer,m_pileUpFEs);

    if ( sc ) {
      if(isDebug) debug() << "Stored " << m_pileUpFEs->size() << " MCVeloFEs at "
			  << m_pileUpOutputContainer << endmsg;
    }
    else{
      error() << "Unable to store MCVeloFEs at "
	      << m_pileUpOutputContainer << endmsg;
    }
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloSim::finalize() {
  
  debug() << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}


//=========================================================================
// delta ray tail random numbers
// dN/DE=k*1/E^2 for relativistic incident particle
// E(r)=1/r, where r is uniform in range 1/Tmin < r < 1/Tmax
// but Tmax bounded by energy left to allocate, so following is
// not truly correct
//=========================================================================
double VeloSim:: ran_inv_E2(double Tmin,double Tmax)  {
  double range=((1./Tmin) - (1./Tmax));
  double offset=1./Tmax;
  double uniform = m_uniformDist()*range+offset;
  double charge=1./uniform;
  return charge;
}


//=========================================================================
// Returns a gaussian random variable larger than a
// This implementation does one-sided upper-tailed deviates.
// Markus has promised to add this in the next release of the core Gaudi code
// in autumn 2002, till then need this version here.
// This code is based on that from the gsl library.
//=========================================================================
double VeloSim::ran_gaussian_tail(const double a, const double sigma) {

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
    }
  else
    {
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
