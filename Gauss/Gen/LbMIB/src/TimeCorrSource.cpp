// $Id: TimeCorrSource.cpp,v 1.8 2010-02-24 14:12:44 mlieng Exp $
// Include files
 
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IRndmGenSvc.h"
 
// STL
//#include <fstream>

// ROOT
#include "TFile.h"
#include "TTree.h"

// LHCb
#include "Event/GenHeader.h"
 
// local
#include "TimeCorrSource.h"


//-----------------------------------------------------------------------------
// Implementation file for class : TimeCorrSource
//
// 2007-01-31 : Magnus Lieng
// 2007-08-10 : Gloria Corti, modify in a tool
//-----------------------------------------------------------------------------
 
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TimeCorrSource );
 
 
//===========================================================================
// Constructor
//===========================================================================
TimeCorrSource::TimeCorrSource(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
  : GaudiHistoTool ( type, name , parent ) ,
    m_overrideSource( 0. ) , 
    m_luminosity( 0. ) ,
    m_counter( 0 ) ,
    m_sumOfWeights( 0. ) ,
    m_rootFile( 0 ) ,
    m_evtTree( 0 ) ,
    m_partTree( 0 ) ,
    m_zGen( 0. ) ,
    m_xyDistGen( 0 ) ,
    m_pxVSrGen( 0 ) , 
    m_pyVSrGen( 0 ) , 
    m_pzVSrGen( 0 ) , 
    m_absPGen( 0 ) ,
    m_thetaGen( 0 ) ,
    m_timeGen( 0 ) ,
    m_ppSvc( 0 ) 
{

  declareInterface<IMIBSource>(this);

  // Set Defaults to check if user has changed options
  m_dzDef = -999999;
  m_zOriginDef = -999999.;
  m_protInFileDef = -999999.;
  m_beamEnergyDef = -999999.;
  
  // Particle input file
  declareProperty("ParticleSourceFile", m_pSourceFile = "");

  // Scaling factors
  declareProperty("ScalingFactor", m_scalingFactor = 1.0);
  declareProperty("ProtonsInFile", m_protInFile = m_protInFileDef);
  declareProperty("BunchSize", m_bunchSize = 1.15e11);
  declareProperty("BeamEnergy", m_beamEnergy = m_beamEnergyDef);
  

  // Histogram generation
  declareProperty("GenerationHist", m_genHist = false);

  //  -1 : Use weight to find number of losses in event.
  // >=0 : Force the generation of this many losses in event.
  declareProperty("ForceLossPerEvt", m_pPerEvt = -1);

  //  -1 : Choose random losses using weight based envelope method.
  // >=0 : Pick losses sequentually from file starting at this entry number.
  declareProperty("ReadFileFromOffset", m_fileOffset = -1);

  // Envelope Size used for random loss selection.
  declareProperty("EnvelopeSize", m_envelopeSize = 10);

  // Z origin and direction for particle source file.
  declareProperty("ZParticleOrigin", m_zOrigin = m_zOriginDef);
  declareProperty("ZParticleOffset", m_zGenOff = 0.0*Gaudi::Units::m);
  declareProperty("ZDirection", m_dz = m_dzDef);

  // Global timing offset
  declareProperty("TimeOffset", m_timeOffset = 0.0*Gaudi::Units::ns);

}

//===========================================================================
// Destructor
//===========================================================================
TimeCorrSource::~TimeCorrSource() { } 

//===========================================================================
// Initialization
//===========================================================================
StatusCode TimeCorrSource::initialize() {

  debug() << "Initialize" << endmsg;

  // Initialize the base class
  StatusCode sc = GaudiHistoTool::initialize( ) ;  
  if ( sc.isFailure( ) ) return sc ;

  // Get Particle property service
  m_ppSvc = svc< IParticlePropertySvc >( "Gaudi::ParticlePropertySvc", true ) ;
  
  // Check if particle source file is present
  if ( m_pSourceFile.empty() ) {
    return Error( "File containing the particles not specified" );
  }

  // Book histograms
  if ( m_genHist == true ) {
    sc = bookHistos();
    if ( ! sc.isSuccess( ) ) return Error( "Failed to book histograms" , sc ) ;
  }

  // Open root file
  m_rootFile = TFile::Open(m_pSourceFile.c_str());

  // Get root trees
  m_evtTree = (TTree*)m_rootFile->Get("Events");
  if (m_evtTree == NULL) return Error( "TTree 'Events' not found in input file" ,StatusCode::FAILURE);
  m_partTree = (TTree*)m_rootFile->Get("Particles");
  if (m_partTree == NULL) return Error( "TTree 'Particles' not found in input file" ,StatusCode::FAILURE);

  // Get header info from file
  info() << "*****************************************************************" << endmsg;
  if (m_rootFile->Get("Header")){
    info() << "HEADER" << endmsg;
    info() << "Sample name:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Name"))->getVal() << endmsg;
    info() << "Sample description:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Description"))->getVal() << endmsg;
    info() << "Beam direction:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Beam"))->getVal() << endmsg;
    info() << "Beam energy:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Energy"))->getVal() << endmsg;
    info() << "Sample interface plane:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Plane"))->getVal() << endmsg;
    info() << "Num. protons represented:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Scale"))->getVal() << endmsg;
    info() << "Additional info:" << endmsg;
    info() << "  " << ((RooStringVar*)m_rootFile->Get("Header/Misc"))->getVal() << endmsg;
    info() << "" << endmsg;
    if (  ( (TString) *((RooStringVar*)m_rootFile->Get("Header/Beam")) )  != "" && m_dz == m_dzDef){
      m_dz = atoi(((RooStringVar*)m_rootFile->Get("Header/Beam"))->getVal())==1?1:-1;
    }
    if (  ( (TString) *((RooStringVar*)m_rootFile->Get("Header/Plane")) ) != "" && m_zOrigin == m_zOriginDef){
      m_zOrigin = atof(((RooStringVar*)m_rootFile->Get("Header/Plane"))->getVal());
    }
    if (  ( (TString) *((RooStringVar*)m_rootFile->Get("Header/Scale")) ) != "" && m_protInFile == m_protInFileDef){
      m_protInFile = atof(((RooStringVar*)m_rootFile->Get("Header/Scale"))->getVal());
    }
    if (  ( (TString) *((RooStringVar*)m_rootFile->Get("Header/Energy")) ) != "" && m_beamEnergy == m_beamEnergyDef){
      m_beamEnergy = atof(((RooStringVar*)m_rootFile->Get("Header/Energy"))->getVal());
    }  
  }
  else {
    info() << "No header data found" << endmsg;
    info() << "" << endmsg;
    // If not set: set to default
    if (m_dz == m_dzDef) {m_dz = 1;}
    if (m_zOrigin == m_zOriginDef) {m_zOrigin = -1.0*Gaudi::Units::m;}
    if (m_protInFile == m_protInFileDef) {m_protInFile = 1.0e11;}
    if (m_beamEnergy == m_beamEnergyDef) {m_beamEnergy = 7.0*Gaudi::Units::TeV;}
  }

  // Check if particle direction is set correctly
  if ( m_dz != 1 && m_dz != -1) {
    return Error( "Z direction of flight must be +/-1" );
  }
  
  // Set Generation point
  m_zGen = m_zOrigin + m_zGenOff;
  
  // Create envelopes
  sc = createEnvelopes();
  if( ! sc.isSuccess( ) ) return Error( "Failed to create envelopes" , sc ) ;

  // Load random number generators
  IRndmGenSvc* rSvc =  svc<IRndmGenSvc> ( "RndmGenSvc", true );

  sc = m_flatGenerator.initialize( rSvc, Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) {
    return Error( "Cannot initialize flat generator", sc ) ;
  }

  sc = m_poissonGenerator.initialize( rSvc,
       Rndm::Poisson( m_sumOfWeights*m_scalingFactor/m_protInFile*m_bunchSize ) ) ;
  if ( ! sc.isSuccess() ) {
    return Error( "Cannot initialize Poisson generator", sc ) ;
  }

  info() << "Generator Settings:" << endmsg;
  info() << "Particles generated at " << m_zGen << " mm in direction " << m_dz 
         << " with a time offset of " << m_timeOffset << " ns." << endmsg;
  info() << "Beam energy is set to " << m_beamEnergy/1000. 
         << " GeV with a bunch size of " << m_bunchSize << " protons" << endmsg;
  info() << "The average number of background interaction resulting in at least one" << endmsg;
  info() << "MIB particle at the LHCb per bunch crossing is "
         << m_sumOfWeights*m_scalingFactor/m_protInFile*m_bunchSize << endmsg;
  info() << "*****************************************************************" << endmsg;
  
  return sc ;
}


//===========================================================================
// Generate one event with given source
//===========================================================================
StatusCode TimeCorrSource::generateEvent( LHCb::GenHeader* theHeader,
                                            LHCb::GenCollisions* theCollisions,
                                            LHCb::HepMCEvents* theEvents,
                                            int& numPart) {
  
  debug() << "generateEvent" << endmsg;
  counter("NCalls") += 1;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  // Choose amount of MIB interactions in event
  int numInt = 0;
  numPart = 0;
  if ( m_pPerEvt >= 0 ) {
    numInt = m_pPerEvt;
  }
  else {
    numInt = (int)floor(m_poissonGenerator());
  }
  counter("NInteractions") += numInt;

  // Registed empty events
  if( numInt == 0 ) {
    counter("NEmptyEvents") += 1;
  } else {
    counter("NNoEmptyEvents") +=1;
    verbose() << "Event contains " << numInt << " interactions" << endmsg;
  }

  // Generate the interactions
  for( int i=0; i<numInt; i++) {
    
    // Randomly choose an interaction
    int firstPart;
    int nParts;
    StatusCode sc;
    if( m_fileOffset >= 0 ){
      sc = getInt(firstPart, nParts);
    }
    else {
      sc = getRandInt(firstPart, nParts);
    }
    if ( ! sc.isSuccess() ) {
      return Error("Random interaction selection failure", sc);
    }

    // Get the requested particles
    for( int i = firstPart; i < (firstPart+nParts); i++ ) {

      // Create MC event
      LHCb::HepMCEvent* mcevt = new LHCb::HepMCEvent();
      mcevt->setGeneratorName( name() );
      HepMC::GenEvent* evt = mcevt->pGenEvt();
      sc = generateParticle(evt, i, numPart);
      
      // Check if the particle really was created
      if ( sc.isSuccess() ) {

        // Number of parts in event (identifier)
        evt->set_signal_process_id( numInt ); //numPart );

        // Add event to event store
        theEvents->insert( mcevt );

        // Create the GenCollision, fill it and put it in the TES
        LHCb::GenCollision* coll = new LHCb::GenCollision();
        coll->setIsSignal( false ) ;
        coll->setProcessType( 999999 ) ;
        coll->setEvent( mcevt ) ;
        theCollisions->insert( coll ) ;

        // Add this collision to GenHeader
        theHeader->addToCollisions( coll );
      }
      // Delete the event if it did not create a particle
      else {
        delete mcevt;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//===========================================================================
// Generate one particle from source
//===========================================================================
StatusCode TimeCorrSource::generateParticle( HepMC::GenEvent* evt, int i, int& numPart ) {

  verbose() << "generateParticle" << endmsg;

  // Generate particle
  ParticleData* rawPart = new ParticleData();
  int nPart = 0;
  StatusCode sc = getPart( rawPart, i, nPart );
  if( sc.isSuccess() && nPart>0 ) {
    verbose() << "Particle generated from particle entry " << i << endmsg;
    counter("NParticles") += nPart;
    numPart = numPart + nPart;

    // Find Vertex and four momentum
    const HepMC::FourVector vtx = getVertex( rawPart->ekin*Gaudi::Units::GeV, 
                                             rawPart->pid, 
                                             rawPart->x*Gaudi::Units::mm, 
                                             rawPart->y*Gaudi::Units::mm, 
                                             rawPart->dx, 
                                             rawPart->dy,
                                             rawPart->dt );
    HepMC::FourVector fourMom = getMomentum( rawPart->ekin*Gaudi::Units::GeV, 
                                             rawPart->pid, rawPart->dx, 
                                             rawPart->dy );

    // Make vertex
    HepMC::GenVertex* mcvtx;
    mcvtx = new HepMC::GenVertex( vtx );

    // Add to event
    evt -> add_vertex( mcvtx );
    debug() << "Numpart is " << numPart << endmsg; 
    if ( 0 == evt -> signal_process_vertex() ){
      verbose() << "Set signal process." << endmsg;
      evt -> set_signal_process_vertex( mcvtx ) ;
    }

    // Add particle(s) 
    for( int j=0; j<nPart; j++){
      mcvtx->add_particle_out( new HepMC::GenParticle( fourMom,
                                                       rawPart->pid,
                                                       LHCb::HepMCEvent::StableInProdGen ) );
    }
  
    // Generate plots (x and y are in cm)
    if( m_genHist ){
      double r = sqrt( rawPart->x*rawPart->x + rawPart->y*rawPart->y );
      m_xyDistGen->fill( rawPart->x/Gaudi::Units::cm, rawPart->y/Gaudi::Units::cm, nPart );
      m_pxVSrGen->fill( r/Gaudi::Units::cm, fourMom.px()/Gaudi::Units::GeV, nPart );
      m_pyVSrGen->fill( r/Gaudi::Units::cm, fourMom.py()/Gaudi::Units::GeV, nPart );
      m_pzVSrGen->fill( r/Gaudi::Units::cm, fourMom.pz()/Gaudi::Units::GeV, nPart );
      m_absPGen->fill( ( rawPart->dx==0 && rawPart->dy==0 ? fourMom.pz() : 
                       ( rawPart->dx!=0 ? fourMom.px()/rawPart->dx : 
                         fourMom.py()/rawPart->dy ) )/Gaudi::Units::GeV, nPart );
      m_thetaGen->fill( ( fourMom.pz()==0 ? Gaudi::Units::pi/2 : 
                          atan(sqrt(fourMom.px()*fourMom.px()+fourMom.py()*fourMom.py())/
                          fourMom.pz()) )/Gaudi::Units::degree, nPart );
      m_timeGen->fill( vtx.t()/Gaudi::Units::ns, nPart );
    }
  }
  
  delete rawPart;
  return sc ;  

}


//===========================================================================
// Finalize
//===========================================================================
StatusCode TimeCorrSource::finalize() {

  debug() << "Finalize" << endmsg;
  
  // Print counters of how many times particle called and how many
  // empty + summary of conditions
  // Print conditions of generation
  //info() << "********************************************************" 
  //       << endmsg;
  //info() << " Used as input file " << m_pSourceFile << endmsg;
  //info() << " With interface plane at z = " << m_zOrigin/Gaudi::Units::m << " m" 
  //       << " and with direction dz = " << m_dz << endmsg;
  //info() << " The particles were generated at z = " << m_zGen/Gaudi::Units::m 
  //       << " m" << endmsg;
  //if( m_pPerEvt == -1 ) {
  //  info() << " Using weight to find number of particles in event" << endmsg;
  //  info() << " Sum(weights) of events in file is " << m_sumOfWeights << endmsg;
  //  info() << " The file represents " << m_timeOfFile/Gaudi::Units::s << " seconds" << endmsg; 
  //  info() << " The resulting weights is " << m_sumOfWeights / m_timeOfFile *Gaudi::Units::hertz
  //         << " Hz (i.e. per 1 sec of LHC running)" << endmsg;
  //  info() << " Events are generate per bunch with frequency " << m_bunchFreq/Gaudi::Units::hertz
  //         << " Hz and scaling factor " << m_scalingFactor << endmsg;
  //  info() << "  === Average num of MIB events per generated event is "
  //         <<  m_sumOfWeights * m_scalingFactor / m_bunchFreq / m_timeOfFile << endmsg;
  //} else {
  //  info() << " Forcing " << m_pPerEvt << " to be generated in each event" 
  //         << endmsg;
  //}
  //if( m_fileOffset == -1 ) {
  //  info() << " Choosing MIB events randomly" << endmsg;
  //} else {
  //  info() << " Picking MIB events from file starting from number "
  //         << m_fileOffset << endmsg;
  //}
  
  return GaudiHistoTool::finalize();  ///< Finalize base class

} 

//===========================================================================
// Create Envelopes
//===========================================================================
StatusCode TimeCorrSource::createEnvelopes() {

  verbose() << "createEnvelopes " << m_pSourceFile << endmsg;

  // Variables used
  Double_t sumOfWeights;

  // Get number of entries in tree
  m_counter = m_evtTree->GetEntries();

  // Make envelopes
  m_evtTree->SetBranchAddress("sumOfWeights", &sumOfWeights);
  for ( int i=0; i<m_counter; i+=m_envelopeSize ){
    m_evtTree->GetEntry(i);
    m_envelopeHolders.push_back( (double)sumOfWeights );
  }

  // Set sum of weights and number of particles
  m_evtTree->GetEntry(m_counter-1);
  m_sumOfWeights = (double)sumOfWeights;

  //info() << "Input file contains " << m_counter 
  //       << " interactions. The sum of interaction weight is " 
  //       << m_sumOfWeights << endmsg;
  info() << "Envelopes will consume " 
         << floor(double(sizeof(double)*m_envelopeHolders.size()/1024)) 
         << " kB memory" << endmsg;

  return StatusCode::SUCCESS;
}




//===========================================================================
// Book Histograms
//===========================================================================
StatusCode TimeCorrSource::bookHistos() {

  debug() << "bookHistos" << endmsg;

  // Generated particles histograms
  m_xyDistGen = book2D( 200, "Generated: XY distribution of particle origin (cm)", 
                        -500., 500., 200, -500., 500., 200 );
  m_pxVSrGen  = book2D( 201, "Generated: Px vs r at particle origin (cm)(GeV)", 
                        0., 500., 100, -10., 10., 200 );
  m_pyVSrGen  = book2D( 202, "Generated: Py vs r at particle origin (cm)(GeV)", 
                        0., 500., 100, -10., 10., 200 );
  m_pzVSrGen  = book2D( 203, "Generated: Pz vs r at particle origin (cm)(GeV)", 
                        0., 500., 100, 0., 5000., 100 );
  m_absPGen   = book1D( 204, "Generated: Momentum of particles (GeV)", 0., 100., 200 );
  m_thetaGen  = book1D( 205, "Generated: Angular distribution of particles (degree)", 
                        0., 360., 180 );
  m_timeGen   = book1D( 206, "Generated: Generation time of particles (ns)", -25., 75., 100 );

  return StatusCode::SUCCESS;
}

//===========================================================================
// Get Momentum four vector
//===========================================================================

HepMC::FourVector TimeCorrSource::getMomentum(double ekin, int pid, 
                                                double dx, double dy) {
  
  verbose() << " getMomentum" << endmsg;

  double dz, dz2, px, py, pz, etot, ptot, mass;

  // Get particle mass
  ParticleProperty* particle = m_ppSvc -> findByStdHepID( pid );
  mass = particle -> mass();

  // Find total momentum.
  etot = ekin + mass;
  ptot = sqrt( (etot + mass) * ekin );

  // Find dz. Need to make sure lack of precission causes no problems..
  dz2 = 1 - dx*dx - dy*dy;
  if ( dz2 > 0 ){ dz = m_dz*sqrt( dz2 ); }
  else{ dz = 0; }

  // Find momentum
  pz = ptot * dz;
  px = ptot * dx;
  py = ptot * dy;

  HepMC::FourVector fourMom( px , py , pz , etot ) ;

  return fourMom;
}


//===========================================================================
// Get vertex four vector
//===========================================================================

HepMC::FourVector TimeCorrSource::getVertex(double ekin, int pid, 
                                              double x, double y, 
                                              double dx, double dy, double dt) {
  
  verbose() << " getVertex" << endmsg;

  double dz, dz2, mass, pMass;

  // Get particle mass
  ParticleProperty* particle = m_ppSvc -> findByStdHepID( pid );
  mass = particle -> mass();
  
  ParticleProperty* proton = m_ppSvc -> findByStdHepID( 2212 );
  pMass = proton -> mass();

  // Find dz. Need to make sure lack of precission causes no problems..
  dz2 = 1 - dx*dx - dy*dy;
  if ( dz2 > 0 ){ dz = m_dz*sqrt( dz2 ); }
  else{ dz = 0; }

  // Time at which LHC clock passes interface plane
  double protonAtIntPlaneClock = m_dz*m_zOrigin*sqrt(1/(1-1/pow(1+m_beamEnergy/pMass,2)))/Gaudi::Units::c_light;

  // Time at which the paticle has to start from the interface plane
  double partAtIntPlaneClock = dt + protonAtIntPlaneClock;

  // Time at which the particle has to start. (If generation point an interface plane are not the same.)
  double partAtGenClock = partAtIntPlaneClock + m_dz*(m_zGen-m_zOrigin)*sqrt(1/(1-1/pow(1+ekin/mass,2)))/(Gaudi::Units::c_light*dz) + m_timeOffset;
  
  const HepMC::FourVector vtx( x, y, m_zGen, partAtGenClock); 
  
  return vtx;
}

//===========================================================================
// Get random interaction
//===========================================================================

StatusCode TimeCorrSource::getRandInt( int &firstPart, int &nParts ){

  verbose() << "getRandInt " << endmsg;

  long i = 0;

  // Find a random particle
  double randomWeight = m_flatGenerator() * m_sumOfWeights;
  verbose() << "Looking for interaction at weight position " << randomWeight 
            << endmsg;

  // Find start of envelope containing randomWeight
  long low = 0;
  long high = m_envelopeHolders.size() - 1;
  i = high;

  // Interpolation method on envelopes
  while( m_envelopeHolders[low] <= randomWeight && 
         m_envelopeHolders[high] > randomWeight && low < high) {
    i = (long)floor( low + ((randomWeight - m_envelopeHolders[low]) 
                            * (high - low))/ 
                     (m_envelopeHolders[high] - m_envelopeHolders[low]) );
    verbose() << "Env  Lower:    " << low*m_envelopeSize << " weight: " 
              << m_envelopeHolders[low] << endmsg;
    verbose() << "Env  Current:  " << i*m_envelopeSize << " weight: " 
              << m_envelopeHolders[i] << endmsg;
    verbose() << "Env  Higher:   " << high*m_envelopeSize << " weight: " 
              << m_envelopeHolders[high] << endmsg;
    if (m_envelopeHolders[i] < randomWeight){
      low = i + 1;
      verbose() << "Env  New Low:  " << low*m_envelopeSize << " weight: " 
                << m_envelopeHolders[low] << endmsg;
    }
    else if (m_envelopeHolders[i] > randomWeight){
      high = i - 1;
      i = high;
      verbose() << "Env  New High: " << high*m_envelopeSize << " weight: " 
                << m_envelopeHolders[high] << endmsg;
    }
    else if (m_envelopeHolders[i] == randomWeight){
      low = i;
      high = i;
    }
    verbose() << "----------------------------------------------------" 
              << endmsg;
  }

  // Find particle in envelope using lookup file
  low = i*m_envelopeSize;
  high = (i+1)*m_envelopeSize;
  if (high > m_counter-1){high = m_counter-1;}

  // Find correct entry (linear search)
  verbose() << "Linear search in envelope from " << low << " to " << high
            << endmsg;
  Double_t sumOfWeights;
  Int_t locFirstPart;
  Int_t locNumParts;
  m_evtTree->SetBranchAddress("sumOfWeights", &sumOfWeights);
  m_evtTree->SetBranchAddress("firstPart", &locFirstPart);
  m_evtTree->SetBranchAddress("nParts", &locNumParts);
  m_evtTree->GetEntry(low);
  verbose() << "Entry: " << low << " Weight: " << (double)sumOfWeights
            << endmsg;
  
  int j = low+1;
  bool isFirst = true;
  while( sumOfWeights <= randomWeight || isFirst){
    isFirst = false;
    firstPart = (int)locFirstPart;
    nParts = (int)locNumParts;
    if (j <= high){
      m_evtTree->GetEntry(j);
      verbose() << "Entry: " << j << " Weight: " << (double)sumOfWeights
                << endmsg;
      j++;
    }
    else{
      j++;
      break;
    }
  }
  
  verbose() << "Chose interaction entry " << j-2 <<  endmsg;

  return StatusCode::SUCCESS;
}


//===========================================================================
// Get linear read interaction
//===========================================================================

StatusCode TimeCorrSource::getInt( int &firstPart, int &nParts ){

  verbose() << "getInt " << endmsg;

  // Choose an Interaction
  Double_t sumOfWeights;
  Int_t locFirstPart;
  Int_t locNumParts;
  m_evtTree->SetBranchAddress("sumOfWeights", &sumOfWeights);
  m_evtTree->SetBranchAddress("firstPart", &locFirstPart);
  m_evtTree->SetBranchAddress("nParts", &locNumParts);
  if( m_fileOffset >= m_evtTree->GetEntries() ) m_fileOffset = 0;
  verbose() << "Choosing interaction number " << m_fileOffset << endmsg;
  m_evtTree->GetEntry(m_fileOffset);
  firstPart = (int)locFirstPart;
  nParts = (int)locNumParts;
  verbose() << "Chose entry: " << m_fileOffset << endmsg;
  m_fileOffset++;

  return StatusCode::SUCCESS;
}


//===========================================================================
// Get sequencial particle
//===========================================================================
StatusCode TimeCorrSource::getPart( ParticleData* target, int i, int &nPart ){

  verbose() << "getPart " << endmsg;
  
  // Get particle
  m_partTree->SetBranchAddress("PartPdgId",&(target->pid));
  m_partTree->SetBranchAddress("PartEk",&(target->ekin));
  m_partTree->SetBranchAddress("PartX",&(target->x));
  m_partTree->SetBranchAddress("PartY",&(target->y));
  m_partTree->SetBranchAddress("PartDx",&(target->dx));
  m_partTree->SetBranchAddress("PartDy",&(target->dy));
  m_partTree->SetBranchAddress("PartDt",&(target->dt));
  m_partTree->SetBranchAddress("PartW",&(target->weight));
  m_partTree->GetEntry(i);
  
  if ( target->weight - int(target->weight) >= m_flatGenerator() ){
    nPart = int(target->weight)+1;
  }
  else {
    nPart = int(target->weight);
  }
  return StatusCode::SUCCESS;
}

 










