// $Id: CollimatorSource.cpp,v 1.2 2007-08-22 17:34:07 gcorti Exp $
// Include files
 
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IRndmGenSvc.h"
 
// STL
#include <fstream>

// LHCb
#include "Event/GenHeader.h"
 
// local
#include "CollimatorSource.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CollimatorSource
//
// 2007-01-31 : Magnus Lieng
// 2007-08-10 : Gloria Corti, modify in a tool
//-----------------------------------------------------------------------------
 
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CollimatorSource );
 
 
//===========================================================================
// Constructor
//===========================================================================
CollimatorSource::CollimatorSource(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
  : GaudiHistoTool ( type, name , parent ) 
{

  declareInterface<IMIBSource>(this);
  
  // Particle input file
  declareProperty("ParticleSourceFile", m_pSourceFile = "");

  // Input file type. True for binary. False for ascii.
  declareProperty("FileIsBinary", m_binaryFile = true);
  declareProperty("SaveTempBinFile", m_saveBinFile = false);
  declareProperty("TempBinFileName", m_tempFileName = "BinPartFile.tmp");

  // Scaling factors
  declareProperty("ScalingFactor", m_scalingFactor = 1.0);
  declareProperty("BunchFrequency", m_bunchFreq = 31.6*1000000);

  // Histogram generation
  declareProperty("GenerationHist", m_genHist = false);

  //  -1 : Use weight to find number of particles in event.
  // >=0 : Force the generation of this many particles in event.
  declareProperty("ForcePartPerEvt", m_pPerEvt = -1);

  //  -1 : Choose random particles using weight based envelope method.
  // >=0 : Pick particles sequentually from file starting at this particle number.
  declareProperty("ReadFileFromOffset", m_fileOffset = -1);

  // Envelope Size used for random particle selection.
  declareProperty("EnvelopeSize", m_envelopeSize = 1000);

  // Z origin and direction for generated particles
  declareProperty("ZParticleOrigin", m_zOrigin = -1.0*m);
  declareProperty("ZDirection", m_dz = 1);

}

//===========================================================================
// Destructor
//===========================================================================
CollimatorSource::~CollimatorSource() { } 

//===========================================================================
// Initialization
//===========================================================================
StatusCode CollimatorSource::initialize() {

  debug() << "Initialize" << endmsg;

  // Initialize the base class
  StatusCode sc = GaudiHistoTool::initialize( ) ;  
  if ( sc.isFailure( ) ) return sc ;

  // Get Particle property service
  m_ppSvc = svc< IParticlePropertySvc >( "ParticlePropertySvc" , true ) ;
  
  // Check if particle source file is present
  if ( m_pSourceFile.empty() ) {
    return Error( "File containing the particles not specified" );
  }

  // Check if particle direction is set correctly
  if ( m_dz != 1 && m_dz != -1) {
    return Error( "Z direction of flight must be +/-1" );
  }

  // Book histograms
  if ( m_genHist == true ) {
    sc = bookHistos();
    if ( ! sc.isSuccess( ) ) return Error( "Failed to book histograms" , sc ) ;
  }

  // Create binary file if needed
  if ( ! m_binaryFile ){
    sc = createBinFile();
  }
  if( ! sc.isSuccess( ) ) return Error( "Failed to create binary file" , sc ) ;

  // Create envelopes (or set file read position for sequential read)
  if ( m_fileOffset <= -1 ){
    sc = createEnvelopes();
    if( ! sc.isSuccess( ) ) return Error( "Failed to create envelopes" , sc ) ;
  }
  else {
    m_mark = m_fileOffset*sizeof(ParticleData);
  }

  // Load random number generators
  IRndmGenSvc* rSvc =  svc<IRndmGenSvc> ( "RndmGenSvc", true );

  sc = m_flatGenerator.initialize( rSvc, Rndm::Flat( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) {
    return Error( "Cannot initialize flat generator", sc ) ;
  }

  sc = m_poissonGenerator.initialize( rSvc,
       Rndm::Poisson( m_sumOfWeights * m_scalingFactor / m_bunchFreq ) ) ;
  if ( ! sc.isSuccess() ) {
    return Error( "Cannot initialize Poisson generator", sc ) ;
  }

  return sc ;
}


//===========================================================================
// Generate one event with given source
//===========================================================================
StatusCode CollimatorSource::generateEvent( LHCb::GenHeader* theHeader,
                                            LHCb::GenCollisions* theCollisions,
                                            LHCb::HepMCEvents* theEvents,
                                            int& numPart) {
  
  debug() << "generateEvent" << endmsg;
  counter("NCalls") += 1;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  // Choose amount of particles in event
  numPart = 0;
  if( m_pPerEvt <= -1 ){
    numPart = (int)floor(m_poissonGenerator());
  }
  else{
    numPart = m_pPerEvt;
  }
  
  counter("NParticles") += numPart;
  if( numPart == 0 ) {
    counter("NEmptyEvents") += 1;
  } else {
    counter("NNoEmptyEvents") +=1;
  }

  
  for( int i=0; i<numPart; i++) {
    
    // Create MC event
    LHCb::HepMCEvent* mcevt = new LHCb::HepMCEvent();
    mcevt->setGeneratorName( name() );
    HepMC::GenEvent* evt = mcevt->pGenEvt();
      
    // Randomly choose particle (If not, the particles are taken sequentially
    // from file offset.)
    sc = generateParticle( evt );
    if ( ! sc.isSuccess() ) {
      return Error("Particle generation failure", sc);
    }
    
    // Number of parts in event
    evt->set_signal_process_id( numPart );

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

  return StatusCode::SUCCESS;
  
}


//===========================================================================
// Generate one particle from source
//===========================================================================
StatusCode CollimatorSource::generateParticle( HepMC::GenEvent* evt ) {

  verbose() << "generateParticle" << endmsg;

  // Randomly choose particle (If not, the particles are taken sequentially
  // from file offset.)
  ParticleData* rawPart = new ParticleData();
  if( m_fileOffset <= -1 ){
    StatusCode sc = getRandPart( rawPart );
    if( ! sc.isSuccess() ) {
      return Error("Random particle selection failure", sc);
    } 
  }
  else {
    StatusCode sc = getPart( rawPart );
    if( ! sc.isSuccess() ) return Error("Particle selection failure", sc);
  }

  // Find Vertex and four momentum
  const HepLorentzVector vtx = getVertex( rawPart->ekin*GeV, rawPart->pid, 
                                          rawPart->x*cm, rawPart->y*cm, 
                                          m_zOrigin, rawPart->dx, 
                                          rawPart->dy );
  HepLorentzVector fourMom = getMomentum( rawPart->ekin*GeV, 
                                          rawPart->pid, rawPart->dx, 
                                          rawPart->dy );

  // Make vertex
  HepMC::GenVertex* mcvtx;
  if( m_fileOffset >= 0 ){
    // Keep weights when reading sequentially from file
    const HepMC::WeightContainer weights( 1, rawPart->weight );
    mcvtx = new HepMC::GenVertex( vtx, 0, weights );
  }
  else{
    mcvtx = new HepMC::GenVertex( vtx );
  }
  evt -> add_vertex( mcvtx );
  
  // Make the spesific particle
  mcvtx->add_particle_out( new HepMC::GenParticle( fourMom,
                                                   rawPart->pid, 
                                                   LHCb::HepMCEvent::StableInProdGen ) );

  // Generate plots (x and y are in cm)
  if( m_genHist ){
    double r = sqrt( rawPart->x*rawPart->x + rawPart->y*rawPart->y );
    m_xyDistGen->fill( rawPart->x, rawPart->y );
    m_pxVSrGen->fill( r, fourMom.px()/GeV );
    m_pyVSrGen->fill( r, fourMom.py()/GeV );
    m_pzVSrGen->fill( r, fourMom.pz()/GeV );
    m_absPGen->fill( ( rawPart->dx==0 && rawPart->dy==0 ? fourMom.pz() : 
                       ( rawPart->dx!=0 ? fourMom.px()/rawPart->dx : 
                         fourMom.py()/rawPart->dy ) )/GeV );
    m_thetaGen->fill( ( fourMom.pz()==0 ? pi/2 : 
                        atan(sqrt(fourMom.px()*fourMom.px()+fourMom.py()*fourMom.py())/
                             fourMom.pz()) )/degree );

    if( m_fileOffset >= 0 ){
      m_xyDistGenWeight->fill( rawPart->x, rawPart->y, rawPart->weight );
      m_pxVSrGenWeight->fill( r, fourMom.px()/GeV, rawPart->weight );
      m_pyVSrGenWeight->fill( r, fourMom.py()/GeV, rawPart->weight );
      m_pzVSrGenWeight->fill( r, fourMom.pz()/GeV, rawPart->weight );
      m_absPGenWeight->fill( ( rawPart->dx==0 && rawPart->dy==0 ? fourMom.pz() :
                               ( rawPart->dx!=0 ? fourMom.px()/rawPart->dx : 
                                 fourMom.py()/rawPart->dy ) ) /GeV, rawPart->weight );
      m_thetaGenWeight->fill( ( fourMom.pz()==0 ? pi/2 : 
                                atan(sqrt(fourMom.px()*fourMom.px()+fourMom.py()*fourMom.py())/
                                     fourMom.pz()) )/degree, rawPart->weight );
    }
  }
  
  delete rawPart;

  return StatusCode::SUCCESS ;  

}


//===========================================================================
// Finalize
//===========================================================================
StatusCode CollimatorSource::finalize() {

  debug() << "Finalize" << endmsg;
  
  // Keep or delete temporary binary file.
  if ( (!m_saveBinFile) && (!m_binaryFile) ){
    remove( m_tempFileName.c_str() );
  }

  // Print counters of how many times particle called and how many
  // empty + summary of conditions
  // Print conditions of generation
  info() << "********************************************************" 
         << endmsg;
  info() << " Used as input file " << m_pSourceFile << endmsg;
  info() << " With particle at z = " << m_zOrigin/Gaudi::Units::m << " m" 
         << " and with direction dz = " << m_dz << endmsg;
  if( m_pPerEvt == -1 ) {
    info() << "  using weight to find number of particles in event" << endmsg;
    info() << "  Sum(weights) in file is " << m_sumOfWeights 
           << " Hz (i.e. per 1 sec of LHC running)" << endmsg;
    info() << "  Events are generate per bunch with frequency " << m_bunchFreq
           << " and scaling factor " << m_scalingFactor << endmsg;
    info() << "  === Sum(weights) for generated event is "
           <<  m_sumOfWeights * m_scalingFactor / m_bunchFreq << endmsg;
  } else {
    info() << "forcing " << m_pPerEvt << " to be generated in each event" 
           << endmsg;
  }
  if( m_fileOffset == -1 ) {
    info() << "and choosing envelope method to chose particles" << endmsg;
  } else {
    info() << "picking particles from file starting from particle number "
           << m_fileOffset << endmsg;
  }
  
  return GaudiHistoTool::finalize();  ///< Finalize base class

} 


//===========================================================================
// Create Binary File
//===========================================================================

StatusCode CollimatorSource::createBinFile(){

  debug() << "createBinFile" << endmsg;

  // Set up variables for envelope creation
  std::string textLine;
  int pid;
  double weight,ekin,x,y,dx,dy;

  // Open file
  std::ifstream inFile( m_pSourceFile.c_str(), std::ios::in );
  if( !inFile ) {
    return Error( "Unable to open particle text file" );
  }

  // Create a temporary binary file
  std::ofstream outFile( m_tempFileName.c_str(), std::ios::out | std::ios::binary );
  if( !outFile ) {
    return Error( "Unable to create binary file" );
  }

  // Read through file and make envelopes
  info() << "Reading particles from text file " << m_pSourceFile << endmsg;
  info() << "This may take a while..." << endmsg;

  // Go through file
  m_counter = 0;
  m_sumOfWeights = 0;
  while( std::getline( inFile, textLine, '\n' )){

    // Get one line and add weight to sum
    std::istringstream inputString( textLine );
    inputString >> pid >> weight >> ekin >> x >> y >> dx >> dy;
    m_sumOfWeights += weight;

    // Save to file
    ParticleData toLookup = { pid, weight, ekin, x, y, dx, dy, m_sumOfWeights };
    outFile.write( (char*)&toLookup, sizeof(ParticleData) );

    // Fill input histogram
    if( m_genHist == true ) {
      m_xyDistInput->fill( x, y, weight );
      m_eKinInput->fill( ekin, weight );
      m_logEKinInput->fill( log10(ekin), weight );
      m_thetaInput->fill( ( (dx*dx+dy*dy)>=1 ? pi/2 : 
                            ( sqrt((dx*dx+dy*dy)/(1-dx*dx-dy*dy)) ) )/degree,
                          weight );      
    }
    
    m_counter++;
  }
  info() << "The text file contains " << m_counter 
         << " particles. The sum of particle weights is " 
         << m_sumOfWeights << endmsg;
  info() << "Binary file created: " << m_tempFileName << endmsg;

  // Set new file name for further processing
  m_pSourceFile = m_tempFileName;

  return StatusCode::SUCCESS;
}


//===========================================================================
// Create Envelopes
//===========================================================================
StatusCode CollimatorSource::createEnvelopes() {

  verbose() << "createEnvelopes " << m_pSourceFile << endmsg;

  // Variables used
  ParticleData partData;

  // Open binary file
  std::ifstream inFile( m_pSourceFile.c_str(), std::ios::in | std::ios::binary );
  if( !inFile ) {
    return Error( "Unable to open binary file " );
  }

  // Get number of entries in file
  inFile.seekg (0, std::ios::end);
  m_counter = (int)(inFile.tellg() / sizeof(ParticleData));
  inFile.seekg (0, std::ios::beg);

  // Make envelopes
  for ( long i=0; i<m_counter;i+=m_envelopeSize ){
    inFile.seekg( i*(long)sizeof(ParticleData) );
    inFile.read( (char*)&partData, sizeof(ParticleData) );
  
    m_envelopeHolders.push_back( partData.sumOfWeights );
  }

  // Set sum of weights and number of particles
  if (inFile.eof()) inFile.clear();
  inFile.seekg( (long)(m_counter-1)*(long)sizeof(ParticleData) );
  inFile.read( (char*)&partData, sizeof(ParticleData) );
  m_sumOfWeights = partData.sumOfWeights;

  info() << "Input file contains " << m_counter 
         << " particles. The sum of particle weights is " 
         << m_sumOfWeights << endmsg;
  info() << "Envelopes will consume " 
         << floor(sizeof(double)*m_envelopeHolders.size()/1024) 
         << "kB memory" << endmsg;

  inFile.clear();
  inFile.close();

  return StatusCode::SUCCESS;
}




//===========================================================================
// Book Histograms
//===========================================================================
StatusCode CollimatorSource::bookHistos() {

  debug() << "bookHistos" << endmsg;

  if( ! m_binaryFile ) {
    m_xyDistInput  = book2D( 100, 
                             "Input: XY distribution of particle origin (cm)", 
                             -500., 500., 200, -500., 500., 200 );
    m_eKinInput    = book1D( 104, "Input: Ekin of particles (GeV)", 
                             0., 100., 100 );
    m_logEKinInput = book1D( 114, "Input: log10(Ekin) of particles (GeV)", 
                             -2., 4., 100 );
    m_thetaInput   = book1D( 105, 
                     "Input: Angular distribution particle momentum (degree)",
                             0., 360., 180 );
  }

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

  if( m_fileOffset >= 0 ){
    m_xyDistGenWeight = book2D( 210, "Generated: XY distribution of particle origin, weighted (cm)", 
                                -500., 500., 200, -500., 500., 200 );
    m_pxVSrGenWeight  = book2D( 211, "Generated: Px vs r at particle origin, weighted (cm)(GeV)", 
                                0., 500., 100, -10., 10., 200 );
    m_pyVSrGenWeight  = book2D( 212, "Generated: Py vs r at particle origin, weighted (cm)(GeV)", 
                                0., 500., 100, -10., 10., 200 );
    m_pzVSrGenWeight  = book2D( 213, "Generated: Pz vs r at particle origin, weighted (cm)(GeV)", 
                                0., 500., 100, 0., 5000., 100 );
    m_absPGenWeight   = book1D( 214, "Generated: Momentum of particles, weighted (GeV)", 
                                0., 100., 100 );
    m_thetaGenWeight  = book1D( 215, "Generated: Angular distribution of particles , weighted (degree)",
                                0., 360., 180 );
  }

  return StatusCode::SUCCESS;
}

//===========================================================================
// Get Momentum four vector
//===========================================================================

HepLorentzVector CollimatorSource::getMomentum(double ekin, int pid, 
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

  HepLorentzVector fourMom( Hep3Vector( px,py,pz ), mass );

  return fourMom;
}


//===========================================================================
// Get vertex four vector
//===========================================================================

HepLorentzVector CollimatorSource::getVertex(double ekin, int pid, 
                                             double x, double y, double z, 
                                             double dx, double dy) {

  verbose() << " getVertex" << endmsg;

  double dz, dz2, mass;

  // Get particle mass
  ParticleProperty* particle = m_ppSvc -> findByStdHepID( pid );
  mass = particle -> mass();

  // Find dz. Need to make sure lack of precission causes no problems..
  dz2 = 1 - dx*dx - dy*dy;
  if ( dz2 > 0 ){ dz = m_dz*sqrt( dz2 ); }
  else{ dz = 0; }

  const HepLorentzVector vtx( x, y, z, 
                              z*sqrt(1/(1-1/pow(1+ekin/mass,2)))/(c_light*dz) );

  return vtx;
}

//===========================================================================
// Get random particle
//===========================================================================

StatusCode CollimatorSource::getRandPart( ParticleData* target ){

  verbose() << " getRandPart " << m_pSourceFile << endmsg;

  long i = 0;

  // Open binary file
  std::ifstream inFile( m_pSourceFile.c_str(), std::ios::in | std::ios::binary );
  if( !inFile ) {
    return Error( "Unable to open binary file" );
  }

  // Find a random particle
  double randomWeight = m_flatGenerator() * m_sumOfWeights;
  verbose() << "Looking for particle at weight position " << randomWeight 
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

  // Interpolation method
  ParticleData lowData, highData;
  inFile.seekg( low*(long)sizeof(ParticleData) );
  inFile.read( (char*)&lowData, sizeof(ParticleData) );
  inFile.seekg( high*(long)sizeof(ParticleData) );
  inFile.read( (char*)&highData, sizeof(ParticleData) );
  inFile.seekg( low*(long)sizeof(ParticleData) );
  inFile.read( (char*)target, sizeof(ParticleData) );

  while( lowData.sumOfWeights < randomWeight 
         && highData.sumOfWeights > randomWeight && low < high) {
    if ( inFile.eof() ) { inFile.clear(); }

    i = (long)floor( low + ((randomWeight - lowData.sumOfWeights) 
                            * (high - low))/ 
                     (highData.sumOfWeights - lowData.sumOfWeights) );
    inFile.seekg( i*(long)sizeof(ParticleData) );
    inFile.read( (char*)target, sizeof(ParticleData) );

    verbose() << "Part Lower:    " << low << " weight: " 
              << lowData.sumOfWeights << endmsg;
    verbose() << "Part Current:  " << i << " weight: " 
              << target->sumOfWeights << endmsg;
    verbose() << "Part Higher:   " << high << " weight: " 
              << highData.sumOfWeights << endmsg;

    if (target->sumOfWeights < randomWeight){
      low = i+1;
      inFile.seekg( low*(long)sizeof(ParticleData) );
      inFile.read( (char*)&lowData, sizeof(ParticleData) );
      verbose() << "Part New Low:  " << low << " weight: " 
                << lowData.sumOfWeights << endmsg;
    }
    else if (target->sumOfWeights > randomWeight){
      high = i-1;
      inFile.seekg( high*(long)sizeof(ParticleData) );
      inFile.read( (char*)&highData, sizeof(ParticleData) );
      verbose() << "Part New High: " << high << " weight: " 
                << highData.sumOfWeights << endmsg;
    }
    verbose() << "----------------------------------------------------" 
              << endmsg;

  }

  verbose() << "Found a particle at weight position " << target->sumOfWeights 
            <<  endmsg;

  inFile.clear();
  inFile.close();

  return StatusCode::SUCCESS;
}



//===========================================================================
// Get sequencial particle
//===========================================================================

StatusCode CollimatorSource::getPart( ParticleData* target ){

  verbose() << " getPart " << m_pSourceFile << endmsg;

  // Open binary file
  std::ifstream inFile( m_pSourceFile.c_str(), std::ios::in | std::ios::binary );
  if( !inFile ) {
    return Error( "Unable to open binary file" );
  }

  // Test if eof is reached
  inFile.seekg (0, std::ios::end);
  if (m_mark >= inFile.tellg()){
    warning() << "End of file reached. Lapping round." << endmsg;
    inFile.seekg(0, std::ios::beg);
    m_mark = inFile.tellg();
  }
  
  // Get particle
  inFile.seekg( m_mark );
  inFile.read( (char*)target, sizeof(ParticleData) );
  m_mark = inFile.tellg();

  verbose() << "Found a particle at weight position " << target->sumOfWeights 
            <<  endmsg;

  inFile.clear();
  inFile.close();

  return StatusCode::SUCCESS;
}

 










