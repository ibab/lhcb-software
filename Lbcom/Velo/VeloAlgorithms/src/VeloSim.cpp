// $Id: VeloSim.cpp,v 1.10 2002-06-30 14:43:11 parkesb Exp $
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

//nag
#include <nag.h>
#include <nag_stdlib.h>
#include <stdio.h>
#include <nagg01.h>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from VeloEvent
#include "Event/MCVeloHit.h"
#include "Event/MCVeloFE.h"

// VeloDet
#include "VeloDet/DeVelo.h"
// from LHCbEvent
#include "Event/MCParticle.h"

// VeloKernel
#include "VeloKernel/VeloSimParams.h"
#include "VeloKernel/VeloRound.h"

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
    return fabs(FE->charge()) < VeloSimParams::threshold; 
  }
};

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloSim::VeloSim( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
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
  , m_pedestalSim          ( true )
  , m_CMSim                ( true )
  , m_spillOver            ( true )
  , m_pileUp               ( false )
{
  declareProperty( "InputContainer"      ,m_inputContainer  );
  declareProperty( "SpillOverInputData"  ,m_spillOverInputContainer  );
  declareProperty( "PileUpInputContainer" ,m_pileUpInputContainer  );
  declareProperty( "PileUpSpillOverInputData" ,m_pileUpSpillOverInputContainer  );
  declareProperty( "OutputContainer"     ,m_outputContainer );
  declareProperty( "PileUpOutputContainer" , m_pileUpOutputContainer );
  declareProperty( "ChargeSim"           ,m_chargeSim );
  declareProperty( "InhomogeneousCharge" ,m_inhomogeneousCharge );
  declareProperty( "Coupling"            ,m_coupling );
  declareProperty( "NoiseSim"            ,m_noiseSim );
  declareProperty( "PedestalSim"         ,m_pedestalSim );
  declareProperty( "CMSim"               ,m_CMSim );
  declareProperty( "SpillOver"           ,m_spillOver );
  declareProperty( "PileUp"              ,m_pileUp );

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

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  SmartDataPtr<DeVelo> velo( detSvc(), "/dd/Structure/LHCb/Velo" );

  if ( 0 == velo ) {
    log << MSG::ERROR << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
  }

  m_velo = velo;

  // calculation of diffusion parameter
  m_baseDiffuseSigma=sqrt(2*VeloSimParams::kT/VeloSimParams::biasVoltage);

  // random number initialisation
  StatusCode scr1=m_gaussDist.initialize( randSvc(), Rndm::Gauss(0.,1.0));
  StatusCode scr2=m_uniformDist.initialize( randSvc(), Rndm::Flat(0.,1.0));
  if ( !(scr1&scr2) ) {
    log << MSG::ERROR << "Random number init failure" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloSim::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  StatusCode sc;
  sc=getInputData(); // get Velo MCHits, pile-up MCHits and spillOver data

  if (sc){
      // velo simulation
      m_hits = m_veloHits;
      m_spillOverHits = m_veloSpillOverHits;
      m_FEs  = m_veloFEs;
      m_simMode="velo"; 
      sc = simulation();   
   

      // pile-up simulation
      m_hits = m_pileUpHits;
      m_spillOverHits = m_pileUpSpillOverHits;
      m_FEs  = m_pileUpFEs;
      m_simMode="pileUp";
      sc = simulation();       
  }

  if (sc) sc= storeOutputData(); // add MCFEs to TDS

  return sc;
}

StatusCode VeloSim::simulation() {
    // perform simulation
  StatusCode sc;

  if (m_chargeSim) sc= chargeSim(false); // simulate signals in strips from GEANT hits of current event
  if (sc&&m_chargeSim&&m_spillOver) sc= chargeSim(true); // simulate signals in strips from GEANT hits of spillOver event
  if (sc&&m_coupling) sc=coupling(); /// charge sharing from capacitive coupling of strips 
  if (sc&&m_noiseSim) sc= noiseSim(); // add noise
  if (sc&&m_pedestalSim) sc= pedestalSim(); // add pedestals - not yet implemented
  if (sc&&m_CMSim) sc=CMSim(); // common mode - not yet implemented
  if (sc) sc=finalProcess(); // remove any unwanted elements and sort
 
  return sc;
}

//=========================================================================
//
//=========================================================================

StatusCode VeloSim::getInputData() {
  MsgStream  log( msgSvc(), name() );

  //*** get the velo input data
  log << MSG::DEBUG << "Retrieving MCVeloHits from " << m_inputContainer 
      << endreq;
  SmartDataPtr<MCVeloHits> hits ( eventSvc() , m_inputContainer );
  m_veloHits=hits;
 
  if( 0 == m_veloHits ) {
    log << MSG::ERROR
	<< "Unable to retrieve input data container="
	<< m_inputContainer << endreq;
    return StatusCode::FAILURE;
  }
  else log << MSG::DEBUG << m_veloHits->size() << " hits retrieved" << endreq;

  //*** get the pile-up input data
  if (m_pileUp) {
    log << MSG::DEBUG << "Retrieving MCVeloHits from " << m_pileUpInputContainer << endreq;
    SmartDataPtr<MCVeloHits> puhits ( eventSvc() , m_pileUpInputContainer );
    m_pileUpHits=puhits;
 
    if( 0 == m_pileUpHits ) {
      log << MSG::ERROR
	<< "Unable to retrieve pile-up input data container="
	<< m_pileUpInputContainer << endreq;
      return StatusCode::FAILURE;
    }
    else log << MSG::DEBUG << m_pileUpHits->size() 
             << " pile-up hits retrieved" << endreq;  
    for (MCVeloHits::const_iterator hitIt=m_pileUpHits->begin(); hitIt < m_pileUpHits->end(); hitIt++){
	// add 100 to pileup sensors number     
        (*hitIt)->setSensor((*hitIt)->sensor()+100);
	log << MSG::DEBUG << " pileup sensors " << (*hitIt)->sensor() << endreq;   
      }

  }

  //*** get the velo SpillOver input data
  if (m_spillOver){
    log << MSG::DEBUG << "Retrieving MCVeloHits of SpillOver Event from " 
        << m_spillOverInputContainer << endreq;
    SmartDataPtr<MCVeloHits> sphits ( eventSvc() , m_spillOverInputContainer );
    m_veloSpillOverHits=sphits;
 
    if( 0 == m_veloSpillOverHits ) {
      log << MSG::INFO
	  << "Spill over event not present unable to retrieve input data container="
	  << m_spillOverInputContainer << endreq;
    }
   else log << MSG::DEBUG << m_spillOverHits->size() 
            << " spill over hits retrieved" << endreq;
  }

  //*** get the pileUp SpillOver input data
  if (m_spillOver&&m_pileUp){
    log << MSG::DEBUG << "Retrieving MCVeloHits of Pile Up SpillOver Event from " 
        << m_pileUpSpillOverInputContainer << endreq;
    SmartDataPtr<MCVeloHits> spuhits ( eventSvc() , m_pileUpSpillOverInputContainer );
    m_pileUpSpillOverHits=spuhits;
 
    if( 0 == m_pileUpSpillOverHits ) {
      log << MSG::INFO
	  << "Spill over event for PileUp not present unable to retrieve input data container="
	  << m_spillOverInputContainer << endreq;
    }
   else log << MSG::DEBUG << m_pileUpSpillOverHits->size() 
            << " PileUp Spill over hits retrieved" << endreq;
  } 

  //*** make vectors for output

  m_pileUpFEs = new MCVeloFEs();
  m_veloFEs = new MCVeloFEs();

  return StatusCode::SUCCESS; 
}

//=========================================================================
// loop through hits allocating the charge to strips  
//=========================================================================
StatusCode VeloSim::chargeSim(bool spillOver) {
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "===> Charge Simulation";
  if (spillOver) log << MSG::DEBUG << " for spill over Event";
  log <<  MSG::DEBUG << endreq;

  MCVeloHits* hits;
  if (!spillOver){
    // hits from current event
    hits=m_hits;
  }
  else{
    // hits from spill Over Event. Return if no spillover
    if( 0 == m_spillOverHits ) return StatusCode::SUCCESS;
    hits=m_spillOverHits;
  }

  log << MSG::DEBUG << "Number of hits to simulate=" << hits->size() << endreq;

  //loop over input hits
  for ( MCVeloHits::const_iterator hitIt = hits->begin() ;
        hits->end() != hitIt ; hitIt++ ) {

    // calculate a set of points in the silicon
    //with which the simulation will work
    MCVeloHit* hit = (*hitIt);
    int NPoints = simPoints(hit);
    log << MSG::VERBOSE << "Simulating " << NPoints 
        << " points in Si for this hit" << endreq;

    if (NPoints>0){
      // calculate charge to assign to each point
      // taking account of delta ray inhomogeneities
      std::vector<double> sPoints(NPoints);
      chargePerPoint(*hitIt,NPoints,sPoints,spillOver);

      // diffuse charge from points to strips
      diffusion(*hitIt,NPoints,sPoints);
    }
  }

  log << MSG::DEBUG << " Number of MCVeloFEs created "
      << m_FEs->size() << endreq;

  return StatusCode::SUCCESS;
}


//=========================================================================
// calculate how many points in the silicon the simulation will be performed at
//=========================================================================
long VeloSim::simPoints(MCVeloHit* hit){
  MsgStream log(msgSvc(), name());

  log << MSG::VERBOSE << "calculate number of points to simulate in Si"
      << endreq;

  double EntryFraction,ExitFraction;
  double pitch;
  bool EntryValid, ExitValid;
  VeloChannelID entryChan=m_velo->channelID(hit->entry(),EntryFraction,
                                            pitch,EntryValid);
  VeloChannelID exitChan=m_velo->channelID(hit->exit(),ExitFraction,
                                           pitch,ExitValid);

  log << MSG::VERBOSE << "entry/exit points "
      << entryChan.sensor() << " " << entryChan.strip()
      <<  " + " << EntryFraction
      << " / " <<  exitChan.sensor()  << " " << exitChan.strip()
      <<  " + " << ExitFraction;
  if (!EntryValid) log << MSG::VERBOSE << " invalid entry point";
  if (!EntryValid) log << MSG::VERBOSE << " invalid exit point";
  log << MSG::VERBOSE <<endreq;

  double NPoints=0.;
  if (EntryValid&&ExitValid){
    // both entry and exit are at valid strip numbers,
    // calculate how many full strips apart
    bool valid;
    int INeighb=m_velo->neighbour(entryChan,exitChan,valid);
    if (valid) NPoints = fabs(float(INeighb)-(EntryFraction-ExitFraction));
    log << MSG::VERBOSE << "Integer number of strips apart " << INeighb
        << " floating number " << NPoints << endreq;
  }
  else {
    // either entry or exit or both are invalid, ignore this hit
    NPoints=0.;
    if (EntryValid!=ExitValid) {
      log << MSG::VERBOSE
          << "simPoints: only one of entry and exit point of hit are in "
          << "silicon - hit ignored " << endreq;
    }
  }

  return int(ceil(NPoints)*VeloSimParams::simulationPointsPerStrip);
}

//=========================================================================
// allocate charge to points
//=========================================================================
void VeloSim::chargePerPoint(MCVeloHit* hit, int Npoints, 
                             std::vector<double>& Spoints, bool spillOver){
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "calculating charge per simulation point" << endreq;

  // total charge in electrons
  double charge=(hit->energy()/eV)/VeloSimParams::eVPerElectron;
  if (spillOver) charge*=VeloSimParams::spillOverChargeFraction;
  log << MSG::VERBOSE << "total charge " << charge
      << " in eV " << hit->energy()/eV;
  if (spillOver) log << MSG::VERBOSE  << " for spill over event ";
  log << MSG::VERBOSE << endreq;


    // charge to divide equally
  double chargeEqual;
  if (m_inhomogeneousCharge){
    // some of charge allocated by delta ray algorithm
    chargeEqual=VeloSimParams::chargeUniform*
      m_velo->siliconThickness(hit->sensor())/micron;
    if (spillOver) chargeEqual*=VeloSimParams::spillOverChargeFraction;
    if (chargeEqual>charge)  chargeEqual=charge;
  }

  else{
    // all of charge allocated equally to points
    chargeEqual=charge;
  }
  log << MSG::VERBOSE << "total charge " << charge
      << " charge for equal allocation " << chargeEqual << endreq;

    // divide equally
  double chargeEqualN=chargeEqual/Npoints;
  double fluctuate=0.;
  for (int i=0; i<Npoints; i++){
    // gaussian fluctuations
    if (m_inhomogeneousCharge) fluctuate=m_gaussDist()*sqrt(chargeEqualN);
    Spoints[i]=chargeEqualN+fluctuate;
  }

  // inhomogeneous charge dist from delta rays
  if (m_inhomogeneousCharge){
    deltaRayCharge(charge-chargeEqual, 0.001*charge, Npoints, Spoints);
    // ensure total charge is allocated
    double total=0.;
    for (int i=0; i<Npoints; i++){total+=Spoints[i];}
    double adjust=charge/total;
    for (int j=0; j<Npoints; j++){Spoints[j]*=adjust;}
  }

  return;
}


//=========================================================================
// allocate remaining charge from delta ray distribution
//=========================================================================
void VeloSim::deltaRayCharge(double charge, double tol, 
                             int Npoints, std::vector<double>& Spoints){
  MsgStream log(msgSvc(), name());
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
    int ipt=int(VeloRound::round(m_uniformDist()*(Npoints-1)));
    log << MSG::VERBOSE << " delta ray charge added to point " << ipt 
        << "/" << Npoints << endreq;
    Spoints[ipt]+=charge;
    Tmax-=charge;
  }
  return;
}


//=========================================================================
// allocate the charge to the collection strips
//=========================================================================
void VeloSim::diffusion(MCVeloHit* hit,int Npoints,
                        std::vector<double>& Spoints){
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "diffusion of charge from simulation points" 
      << endreq;

  HepVector3D path = (hit->exit())-(hit->entry());
  path/=(Npoints-1); // distance between steps on path
  HepPoint3D point= (hit)->entry();
  double thickness=m_velo->siliconThickness(hit->sensor())/micron;
  double ZDiffuse=thickness;
  // assume strips are at opposite side of Si to entry point
  double dz=ZDiffuse/ (Npoints-1); // distance between steps on path

  for (int ipt=0; ipt<Npoints; ipt++){ //loop over points on path
    double fraction,pitch;
    bool valid;
    //      log << MSG::DEBUG << " ipt " << ipt << "point " << point << endreq;
    //calculate point on path
    VeloChannelID entryChan=m_velo->channelID(point,fraction,pitch,valid); 
    //      log << MSG::DEBUG << "chan " << entryChan.strip() << " fraction " 
    //          << fraction << " pitch " << pitch << " valid " << valid 
    //          << endreq;
    const int neighbs=1; // only consider =/- this many neighbours
    double chargeFraction[2*neighbs+1];
    double totalFraction=0.;
    // loop over neighbours per point
    int iNg;
    for  (iNg=-neighbs; iNg<=+neighbs; iNg++){ 
      //double diffuseDist1=((iNg-0.5)-fraction)*pitch/micron;
      //double diffuseDist2=((iNg+0.5)-fraction)*pitch/micron;
      double diffuseDist1=((iNg)-fraction)*pitch/micron;
      double diffuseDist2=((iNg+1.)-fraction)*pitch/micron;
      double diffuseSigma=m_baseDiffuseSigma*sqrt(thickness*ZDiffuse);
      //  log << MSG::DEBUG << "diffuseDist1 " << diffuseDist1 
      // <<   " diffuseDist2 " << diffuseDist2 << " diffuseSigma " 
      // << diffuseSigma << " base " << m_baseDiffuseSigma 
      // << " zdiff " << ZDiffuse << endreq;

      double prob1= g01eac(Nag_UpperTail,diffuseDist1/diffuseSigma , 
                           NAGERR_DEFAULT);
      double prob2= g01eac(Nag_UpperTail,diffuseDist2/diffuseSigma , 
                           NAGERR_DEFAULT);
      //  log << MSG::DEBUG << " prob1+2 " <<  prob1 << " " << prob2 << endreq;
      int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
      chargeFraction[i]=fabs(prob1-prob2);
      totalFraction+= fabs(prob1-prob2);
      // log << MSG::DEBUG << i << " iNg " << iNg << " cfrac " 
      //     << chargeFraction[i]  << " tot " << totalFraction << endreq;
    }

    // renormalise allocated fractions to 1., and update strip signals
    for  (iNg=-neighbs; iNg<=+neighbs; iNg++ ){
      int i= (iNg<0) ? neighbs+abs(iNg) : iNg;
      // log << MSG::DEBUG << i << " iNg " << iNg << " ipt " << ipt 
      //      << " " << endreq;
      double charge=Spoints[ipt]*(chargeFraction[i]/totalFraction);
      //  log << MSG::DEBUG << i << " ipt " << ipt << " charge " 
      //      << charge << endreq;
      if (charge>VeloSimParams::threshold*0.1){
        // ignore if below 10% of threshold
        // calculate index of this strip
        VeloChannelID stripKey = m_velo->neighbour(entryChan,iNg,valid);
        // log << MSG::DEBUG << " neighbour " << entryChan.strip() << " " 
        //     << stripKey.strip() << " iNg " << iNg << endreq;
        // update charge and MCHit list
        if (valid){
          MCVeloFE* myFE = findOrInsertFE(stripKey);
          fillFE(myFE,hit,charge);
        }
      }
    } // neighbours loop

    point+=path; // update to look at next point on path
    ZDiffuse-=dz;
  } // loop over points

  return;
}


//=========================================================================
// update signal and list of MCHits
//=========================================================================
void VeloSim::fillFE(MCVeloFE* myFE, MCVeloHit* hit, double charge){
  myFE->setAddedSignal(myFE->addedSignal()+charge);
  // add link to MC hit (if not already there)
  SmartRefVector<MCVeloHit> hitlist = myFE->mcVeloHits();
  bool present=false;
  for(SmartRefVector<MCVeloHit>::const_iterator hitIt =hitlist.begin();
      hitIt < hitlist.end(); hitIt++){if (hit==(*hitIt)) present=true;}
  if (!present) myFE->addToMCVeloHits(hit);

  return;
}

//=========================================================================
// update signal
//=========================================================================
void VeloSim::fillFE(MCVeloFE* myFE,double charge){
  MsgStream log(msgSvc(), name());
  myFE->setAddedSignal(myFE->addedSignal()+charge);
  return;
}

//=========================================================================
// add a % of signal in strip to the two neighbouring strips
// it is assumed that this is a small % and hence it doesn't matter
// in what order this procedure is applied to the strip list.
//=========================================================================
StatusCode VeloSim::coupling(){

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG <<  "--- strip coupling ------" << endreq;
  // sort FEs into order of ascending sensor + strip
  std::stable_sort(m_FEs->begin(),m_FEs->end(),
              VeloEventFunctor::Less_by_key<const MCVeloFE*>());

  // make new container for any added strips
  m_FEs_coupling=new MCVeloFEs();

  for ( MCVeloFEs::iterator FEIt = m_FEs->begin() ;
        m_FEs->end() != FEIt ; FEIt++ ) {

    // calculate signal to couple to neighbouring strips
    double coupledSignal=(*FEIt)->addedSignal()*VeloSimParams::coupling;
    log <<  MSG::VERBOSE << "coupledSignal " <<   coupledSignal 
        << " orig " << (*FEIt)->addedSignal() << endreq;

      // subtract coupled signal from this strip
    (*FEIt)->setAddedSignal((*FEIt)->addedSignal()-2.*coupledSignal);
    log <<  MSG::VERBOSE << " subtracted " << (*FEIt)->addedSignal() << endreq;

      // add to previous strip (if doesn't exist then create)
      // apply charge threshold to determine if worth creating
    bool create = (coupledSignal > VeloSimParams::threshold*0.1);
    bool valid;
    MCVeloFE* prevStrip=findOrInsertPrevStrip(FEIt,valid,create);
    if (valid) fillFE(prevStrip,coupledSignal);

    log << MSG::VERBOSE << " base " << (*FEIt)->strip() << " " 
        << (*FEIt)->sensor() << endreq;
    if(valid) log << MSG::VERBOSE << " prev " << prevStrip->strip() 
                  << " " << prevStrip->sensor() << endreq;

    // add to next strip
    MCVeloFE* nextStrip=findOrInsertNextStrip(FEIt,valid,create);
    log << MSG::VERBOSE << " create " << create << " valid " << valid 
        << endreq;
    if (valid) fillFE(nextStrip,coupledSignal);

    log << MSG::VERBOSE << " base " << (*FEIt)->strip() << " " 
        << (*FEIt)->sensor() << endreq;
    if (valid) log << MSG::VERBOSE << " next " << nextStrip->strip() << " " 
                   << nextStrip->sensor() << endreq;

  } // end of loop over hits

  // add any newly created FEs
  log << MSG::DEBUG << "FEs created by coupling routine " 
      << m_FEs_coupling->size() << endreq;
  for (MCVeloFEs::iterator coupIt=m_FEs_coupling->begin(); 
       coupIt<m_FEs_coupling->end(); coupIt++){
    MCVeloFE* myFE = m_FEs->object( (*coupIt)->key() );
    if ( 0 != myFE ) {
      myFE->setAddedSignal( myFE->addedSignal() + (*coupIt)->addedSignal() );
      log << MSG::DEBUG << " -- Existing FE. " 
          << (*coupIt)->sensor() << "," 
          << (*coupIt)->strip() << " Update with coupling FE." << endreq;      
    } else {
      log << MSG::DEBUG << " -- Add coupling FE " 
          << (*coupIt)->sensor() << "," 
          << (*coupIt)->strip() << endreq;
      m_FEs->insert(*coupIt);
    }
  }
  delete m_FEs_coupling;

  return StatusCode::SUCCESS;
}


//=========================================================================
// From an originally sorted list, find the strip with the previous key,
// or create a new one.
//=========================================================================
MCVeloFE* VeloSim::findOrInsertPrevStrip(MCVeloFEs::iterator FEIt, 
                                         bool& valid, bool& create){
  MsgStream  log( msgSvc(), name() );

  bool exists;
  // try previous entry in container
  MCVeloFE* prevStrip=(*FEIt);
  if (FEIt!=m_FEs->begin()){
    FEIt--;
    prevStrip=(*(FEIt));
    FEIt++;
  }
  // check this
  exists = (m_velo->neighbour((*FEIt)->key(),prevStrip->key(),valid)==-1);
  if (exists&&valid) return prevStrip;

  // check if just added this strip in other container
  if (m_FEs_coupling->size()!=0){
    MCVeloFEs::iterator last=m_FEs_coupling->end(); last--;
    prevStrip=(*last);
  }
  // check this
  exists = (m_velo->neighbour((*FEIt)->key(),prevStrip->key(),valid)==-1);
  if (exists&&valid) return prevStrip;

  // doesn't exist so insert a new strip (iff create is true)
  if (create){
    VeloChannelID stripKey = m_velo->neighbour((*FEIt)->key(),-1,valid);
    if(valid){
      //== Protect if key already exists ==
      prevStrip = m_FEs_coupling->object(stripKey);
      if ( 0 != prevStrip ) return prevStrip;
      log << MSG::VERBOSE << " create strip" << stripKey.strip() << " " 
          << stripKey.sensor() << endreq;
      prevStrip = new MCVeloFE(stripKey);
      m_FEs_coupling->insert(prevStrip);
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
  MsgStream  log( msgSvc(), name() );
  
  bool exists;
  // try next entry in container
  MCVeloFE* nextStrip=*FEIt;
  MCVeloFEs::iterator last = m_FEs->end(); last--;
  if (FEIt!=last){
    FEIt++;
    nextStrip=(*(FEIt));
    FEIt--;
  }
  
  // check this
  exists = (m_velo->neighbour((*FEIt)->key(),nextStrip->key(),valid)==+1);
  if (exists&&valid) return nextStrip;
  
  // doesn't exist so insert a new strip (iff create is true)
  if (create){
    VeloChannelID stripKey = m_velo->neighbour((*FEIt)->key(),+1,valid);
    if(valid){
      //== Protect if key already exists ==
      nextStrip = m_FEs_coupling->object(stripKey);
      if ( 0 != nextStrip ) return nextStrip;
      log << MSG::VERBOSE << " create strip" << stripKey.strip() << " " 
          << stripKey.sensor() << endreq;
      nextStrip = new MCVeloFE(stripKey);
      m_FEs_coupling->insert(nextStrip);
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
  MsgStream  log( msgSvc(), name() );
  return StatusCode::SUCCESS;
}


//=========================================================================
//  
//=========================================================================
StatusCode VeloSim::noiseSim(){
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "===> Noise simulation" << endreq;
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
      log << MSG::VERBOSE << " noise added to existing strip "  
          << (*FEIt)->addedNoise()<< endreq;
    }
  }

  // allocate noise (above threshold) to channels that don't currently
  // have signal
  int maxSensor=0;
  if (m_simMode=="velo") maxSensor=m_velo->nbSensor();
  if (m_simMode=="pileUp") maxSensor=m_velo->nbPuSensor();

  for (int iSensorArrayIndex=0; iSensorArrayIndex< maxSensor; 
       iSensorArrayIndex++){
    double sensor=m_velo->sensorNumber(iSensorArrayIndex);
    double noiseSig=noiseSigma(stripCapacitance); 
    // use average capacitance of sensor, should be adequate if variation in 
    // cap. not too large.
    // number of hits to add noise to (i.e. fraction above threshold)
    // add both large +ve and -ve noise.
    int maxStrips= m_velo->nbStrips();
    int hitNoiseTotal= int(VeloRound::round(2.*g01eac(Nag_UpperTail, 
                                           VeloSimParams::threshold/noiseSig ,
                                           NAGERR_DEFAULT)*float(maxStrips)));

    log << MSG::VERBOSE << "Number of strips to add noise to "
        << hitNoiseTotal
        << " sensor Number " << sensor
        << " maxStrips " << maxStrips
        <<  " sigma of noise " << noiseSig
        << " threshold " << VeloSimParams::threshold
        << " tail probability " 
        << g01eac(Nag_UpperTail, VeloSimParams::threshold/noiseSig , 
                  NAGERR_DEFAULT) 
        << endreq;

    for (int noiseHit=0; noiseHit<hitNoiseTotal; noiseHit++){
      // choose random hit to add noise to
      // get strip number
      int stripArrayIndex=int(VeloRound::round(m_uniformDist()*(maxStrips-1)));
      VeloChannelID stripKey(sensor, 
                             m_velo->stripNumber(sensor,stripArrayIndex));
      // find strip in list.
      MCVeloFE* myFE = findOrInsertFE(stripKey);
      if (myFE->addedNoise()==0){
        double noise=noiseValueTail(stripCapacitance);
        myFE->setAddedNoise(noise);
        log << MSG::VERBOSE << "hit from tail of noise created "  
            << myFE->addedNoise() << endreq;
      }
      else{
        // already added noise here - so generate another strip number
        noiseHit--;
      }
    }

  }

  return StatusCode::SUCCESS;
}



//=========================================================================
// sigma of noise to generate
//=========================================================================
double VeloSim::noiseSigma(){
  return noiseSigma(VeloSimParams::averageStripCapacitance);
}


//=========================================================================
// sigma of noise to generate
//=========================================================================
double VeloSim::noiseSigma(double stripCapacitance){
  double noiseSigma=stripCapacitance*VeloSimParams::noiseCapacitance+
    VeloSimParams::noiseConstant;
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
  MsgStream  log( msgSvc(), name() );

  return StatusCode::SUCCESS;
}



//=========================================================================
// find a strip in list of FEs, or if it does not currently exist create it
//=========================================================================
MCVeloFE* VeloSim::findOrInsertFE(VeloChannelID& stripKey){
  MsgStream  log( msgSvc(), name() );
  MCVeloFE* myFE = m_FEs->object(stripKey);
  if (myFE==NULL) {
    // this strip has not been used before, so create
    myFE = new MCVeloFE(stripKey);
    log << MSG::DEBUG << " -- Add FE " << stripKey.sensor() << "," 
        << stripKey.strip() << endreq;
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
    std::sort(m_FEs->begin(), m_FEs->end(), VeloEventFunctor::Less_by_charge<const MCVeloFE*>());
    std::reverse(m_FEs->begin(),m_FEs->end());
    MCVeloFEs::iterator it = std::find_if(m_FEs->begin(), 
                     m_FEs->end(), 
                     chargeThreshold());
    m_FEs->erase(it, m_FEs->end());

// sort FEs into order of ascending sensor + strip
    std::sort(m_FEs->begin(),m_FEs->end(),VeloEventFunctor::Less_by_key<const MCVeloFE*>());

return StatusCode::SUCCESS;
}


//=========================================================================
// store MCFEs
//=========================================================================
StatusCode VeloSim::storeOutputData(){
  MsgStream  log( msgSvc(), name() );

  StatusCode sc;
  // velo FEs
  sc = eventSvc()->registerObject(m_outputContainer,m_veloFEs);

  if ( sc ) {
   log << MSG::DEBUG << "Stored " << m_veloFEs->size() << " MCVeloFEs at " 
       << m_outputContainer << endreq;
  }
  else{
    log << MSG::ERROR << "Unable to store MCVeloFEs at " 
        << m_outputContainer << endreq;
   }

  // pileup FEs

  if (m_pileUp){  
      sc = eventSvc()->registerObject(m_pileUpOutputContainer,m_pileUpFEs);

      if ( sc ) {
	  log << MSG::DEBUG << "Stored " << m_pileUpFEs->size() << " MCVeloFEs at " 
	      << m_pileUpOutputContainer << endreq;
      }
      else{
	  log << MSG::ERROR << "Unable to store MCVeloFEs at " 
	      << m_pileUpOutputContainer << endreq;
      }
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloSim::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

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











