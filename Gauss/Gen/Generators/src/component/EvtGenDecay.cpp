// $Id: EvtGenDecay.cpp,v 1.23 2010-05-05 19:38:36 robbep Exp $
// Header file
#include "EvtGenDecay.h"

// Include files
#include <iostream>
#include <fstream>

// from boost
#include "boost/filesystem.hpp"

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"

// from LHCb
#include "Kernel/ParticleID.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
// from Event
#include "Event/HepMCEvent.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"
 
// from EvtGen
#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtDecayTable.hh"
#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtPDL.hh"

// from EvtGenExtras
#include "EvtGenModels/EvtModelRegExtras.hh"

#include "Generators/StreamForGenerator.h"
#include "Generators/IProductionTool.h"
#include "Generators/F77Utils.h" 
#include "Generators/LbTaula.h"
#include "Generators/LbPhotos.h"

//EvtGen holding tool
#include "IEvtGenTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtGenDecay
//
// 2003-10-15 : Patrick Robbe
//-----------------------------------------------------------------------------
// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( EvtGenDecay )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtGenDecay::EvtGenDecay( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_gen(0),
    m_randomEngine(0),
    // Minimum value for ctau in particle property data.
    // Below, it is set to 0
    m_minctau( 1.e-4 * Gaudi::Units::mm ) ,
    // Maximum value for ctau in particle property data.
    // Above, it is set to 0
    m_maxctau( 1.e+16 * Gaudi::Units::mm ) ,
    // Minimum value for Gamma in particle property data.
    // Below, it is set to 0
    m_minwidth( 1.5e-6 * Gaudi::Units::GeV ) ,
    // Unit for Taula output
    m_taulaUnit( 6 ) ,
    // Unit for PHOTOS output
    m_photosUnit( 6 ) {
    // Declare IEvtGenDecay interface
    declareInterface<IDecayTool>( this ) ;
    // Declare properties for EvtGen
    // The generic decay file
    declareProperty("DecayFile" , m_decayFile = "empty" ) ;
    // The signal decay file
    declareProperty("UserDecayFile" , m_userDecay = "empty" ) ;
    // Keep temporary evt.pdl file
    declareProperty("KeepTempEvtFile" , m_keepTempEvtFile = false ) ;
    // Generate Polarized Lambda_b decays
    declareProperty("PolarizedLambdad" , m_generatePolLambdab = false ) ;
    // Generate Polarized charmonium
    declareProperty("PolarizedCharmonium" , m_generatePolCharmonium = false ) ;
    // Parameters for charmonium polarization
    declareProperty("RealHelOne"  , m_realHelOne  = 1. ) ;
    declareProperty("ImHelOne"    , m_imHelOne    = 0. ) ;
    declareProperty("RealHelZero" , m_realHelZero = 1. ) ;
    declareProperty("ImHelZero"   , m_imHelZero   = 0. ) ;   
    // Initialize signalId
    m_signalId = EvtId( -1, -1 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
EvtGenDecay::~EvtGenDecay( ) { }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode EvtGenDecay::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  MsgStream * msg = new MsgStream( msgSvc() , name() ) ;
  StreamForGenerator::getStream() = msg ;

  //EvtGenTool
  debug() << "Getting EvtGenTool" << endmsg ;
  m_evtgentool = tool<IEvtGenTool>("EvtGenTool") ;

  // Find Generic DECAY.DEC file
  // Default location (if not specified in job options is  $DECFILESROOT/dkfiles/DECAY.DEC
  if ( m_decayFile.empty() || "empty" == m_decayFile )
    if ( "UNKNOWN" != System::getEnv("DECFILESROOT") ) 
      m_decayFile  = System::getEnv( "DECFILESROOT" ) + "/dkfiles/DECAY.DEC" ;
  
  // Check if file exists:
  if ( ! boost::filesystem::exists( m_decayFile ) ) 
    return Error( "The specified generic decay table does not exist" ) ;

  // create temporary evt.pdl file filled with Gaudi ParticlePropertySvc
  boost::filesystem::path evtPdlFile( std::tmpnam(NULL) ) ;
  if ( boost::filesystem::exists( evtPdlFile ) ) boost::filesystem::remove( evtPdlFile ) ;
  sc = createTemporaryEvtFile( evtPdlFile ) ;
  if ( m_keepTempEvtFile ) always() << "Keep the file: " << evtPdlFile << endmsg ;
  if ( ! sc.isSuccess() ) return sc ;

  // create random engine for EvtGen
  IRndmGenSvc * randSvc( 0 ) ;
  try { randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ; }
  catch ( const GaudiException & exc ) {
    Exception( "RndmGenSvc not found to initialize EvtGen random engine" ) ;
  }

  m_randomEngine = new EvtGenGaudiRandomEngine( randSvc , sc ) ;
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize EvtGenGaudiRandomService" , sc ) ;
  release( randSvc ) ;

  //this list will be deleted after use
  //the actual models will be deleted by the singleton EvtModel
  const std::auto_ptr<const EvtModelList> models = EvtModelRegExtras::getModels();
  EvtAbsRadCorr* isrEngine = 0;//dummy needed for compile

  // redirect taola output to temporary file
  if ( ! ( msgLevel( MSG::DEBUG ) ) ) {
    boost::filesystem::path taulaTempFileName( std::tmpnam(NULL) ) ;
    if ( boost::filesystem::exists( taulaTempFileName ) ) 
      boost::filesystem::remove( taulaTempFileName ) ;
    m_taulaUnit = F77Utils::getUnit() ;
    sc = F77Utils::openNew( m_taulaUnit , taulaTempFileName.string() ) ;
    if ( ! sc.isSuccess() ) 
      return Error( "Cannot create output file for TAULA" ) ;
    LbTaula::setOutputUnit( m_taulaUnit ) ;
  }

  // change PHOTOS output unit
  if ( ! ( msgLevel( MSG::DEBUG ) ) ) {
    boost::filesystem::path photosTempFileName( std::tmpnam(NULL) ) ;
    if ( boost::filesystem::exists( photosTempFileName ) ) 
      boost::filesystem::remove( photosTempFileName ) ;
    m_photosUnit = F77Utils::getUnit() ;
    sc = F77Utils::openNew( m_photosUnit , photosTempFileName.string() ) ;
    if ( ! sc.isSuccess() ) 
      return Error( "Cannot create output file for PHOTOS" ) ;
    LbPhotos::setOutputUnit( m_photosUnit ) ;
  }
  
  // create EvtGen engine from decay file, evt.pdl file and random engine
  if ( m_evtgentool -> isInit() )  m_gen = m_evtgentool->getEvtGen() ;
  else { 
    m_gen = new EvtGen ( m_decayFile.c_str() , evtPdlFile.string().c_str() ,
                         m_randomEngine, isrEngine, models.get()) ;
    m_evtgentool->setEvtGen( m_gen ) ;
  }
  
  // Remove temporary file if not asked to keep it
  if ( ! m_keepTempEvtFile ) boost::filesystem::remove( evtPdlFile ) ;

  // Read the optional signal decay file
  if ( ! m_userDecay.empty() && "empty" != m_userDecay ) {
    if ( ! boost::filesystem::exists( m_userDecay ) ) 
      return Error( "The specified user decay file does not exist" ) ;
    m_gen -> readUDecay( m_userDecay.c_str() ) ; 
  }

  debug() << "EvtGenDecay initialized" << endmsg ;
 
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode EvtGenDecay::finalize() {
  delete m_randomEngine ;
  
  debug() << "EvtGenDecay finalized" << endmsg ;
 
  release( m_evtgentool ) ;
	
  if ( ! ( msgLevel( MSG::DEBUG ) ) ) { 
    F77Utils::close( m_taulaUnit ) ;
    F77Utils::close( m_photosUnit ) ;
  }
  
  delete StreamForGenerator::getStream() ;
  StreamForGenerator::getStream() = 0 ;

  return GaudiTool::finalize( ) ;
}

//=============================================================================
// Generate a Decay tree from a particle theMother in the event theEvent
//=============================================================================
StatusCode EvtGenDecay::generateDecay( HepMC::GenParticle * theMother ) const {
  // Call EvtGen for the particle to generate
  checkParticle( theMother ) ;

  EvtParticle * part( 0 ) ;
  StatusCode sc = callEvtGen( part , theMother , EvtId( -1, -1 ) ) ;
  if ( ! sc.isSuccess( ) ) return sc ;

  // Update HepMCEvent theEvent and HepMCParticle theMother

  // sets PDG Id of theMother
  // because EvtGen might have asked to change the original one
  // for CP modes (if flip is enabled)
  theMother -> set_pdg_id( EvtPDL::getStdHep( part->getId() ) ) ;

  // Get reference position in space and time to be able to assign
  // correct vertex for daughter particles of theMother
  // because EvtGen gives position with respect to the "root" particle
  // This reference position is production vertex of theMother
  HepMC::GenVertex * V = theMother -> production_vertex() ;
  Gaudi::LorentzVector theOriginPosition( V -> position() ) ;

  // Fill HepMC event theEvent with EvtGen decay tree part
  // starting from theMother
  makeHepMC( part , theMother , theOriginPosition , -999 ) ;

  // delete EvtGen particle and all its daughters
  part -> deleteTree ( ) ;

  // Set status to "decayed by evtgen"
  theMother -> 
    set_status( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen ) ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Generate a Decay tree from a particle theMother in the event theEvent
//=============================================================================
StatusCode EvtGenDecay::generateSignalDecay( HepMC::GenParticle * theMother ,
                                             bool & flip) const {
  // If particle already has daughters, return now
  if ( 0 != theMother -> end_vertex() ) {
    flip = false ;
    return StatusCode::SUCCESS ;
  }
  
  // Call EvtGen for the particle to generate
  checkParticle( theMother ) ;

  // get signal alias
  EvtId decayId = getSignalAlias( theMother -> pdg_id() ) ;

  EvtParticle * part( 0 ) ;
  StatusCode sc = callEvtGen( part , theMother , decayId ) ;
  if ( ! sc.isSuccess( ) ) return sc ;

  // Update HepMCEvent theEvent and HepMCParticle theMother

  // sets PDG Id of theMother
  // because EvtGen might have asked to change the original one
  // for CP modes (if flip is enabled)
  theMother -> set_pdg_id( EvtPDL::getStdHep( part->getId() ) ) ;
  flip = ( decayId != part -> getId() ) ;

  // Get reference position in space and time to be able to assign
  // correct vertex for daughter particles of theMother
  // because EvtGen gives position with respect to the "root" particle
  // This reference position is production vertex of theMother
  HepMC::GenVertex * PV = theMother -> production_vertex() ;
  Gaudi::LorentzVector theOriginPosition( PV -> position() ) ;
  
  // Fill HepMC event theEvent with EvtGen decay tree part
  // starting from theMother
  makeHepMC( part , theMother , theOriginPosition , -999 ) ;

  // delete EvtGen particle and all its daughters
  part -> deleteTree ( ) ;

  // Set status to "signal in lab frame"
  theMother -> set_status( LHCb::HepMCEvent::SignalInLabFrame ) ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Generate a Decay tree from a particle theMother.
// It stops when the particle to decay has a mass larger than theMassLimit
// or is of the type targetId
//=============================================================================
StatusCode EvtGenDecay::generateDecayWithLimit( HepMC::GenParticle * theMother ,
                                                const int targetId ) const {
  checkParticle( theMother ) ;

  EvtParticle * part( 0 ) ;
  StatusCode sc = callEvtGen( part , theMother , EvtId( -1 , -1 ) ) ;
  if ( ! sc.isSuccess() ) return sc ;
  
  // Update HepMCParticle theMother

  // sets PDG Id of theMother
  // because EvtGen might have asked to change the original one
  // for CP modes (if flip is enabled)
  theMother -> set_pdg_id( EvtPDL::getStdHep( part->getId() ) ) ;

  // Get reference position in space and time to be able to assign
  // correct vertex for daughter particles of theMother
  // because EvtGen gives position with respect to the "root" particle
  // This reference position is production vertex of theMother  
  HepMC::GenVertex * PV = theMother -> production_vertex() ;
  Gaudi::LorentzVector theOriginPosition( PV -> position() ) ;
  
  // Fill HepMC event theEvent with EvtGen decay tree part
  // starting from theMother and stopping at particle with pdgId
  // equals to targetId
  makeHepMC( part , theMother , theOriginPosition , targetId ) ;
  
  // delete EvtGen particle and all its daughters
  part -> deleteTree ( ) ;

  // Set particle status to "decayed by evtgen"
  theMother ->
    set_status( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen ) ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Update the HepMC tree ( theMother ) with a EvtGen tree ( the EvtGenPart).
// The origin is the production 
// position of the root particle in the decay tree. The conversion will stop
// when the particle is of requested type targetId (if the value is negative
// -- which is the default value -- it will be ignored and all decay chain
// will be converted)
//=============================================================================
StatusCode EvtGenDecay::makeHepMC( EvtParticle * theEvtGenPart ,
                                   HepMC::GenParticle * theMother ,
                                   const Gaudi::LorentzVector & theOrigin ,
                                   int targetId ) const {
  if ( 0 != theEvtGenPart->getNDaug() ) {
    // Find the vertex
    // In EvtGen all positions are defined with respect to the
    // root mother particle whose production vertex is theOrigin
    EvtVector4R position = theEvtGenPart -> getDaug( 0 ) -> get4Pos() ;
    double t  = ( position . get( 0 ) * Gaudi::Units::mm / EvtConst::c ) 
      *  Gaudi::Units::s + theOrigin.t() ;
    double x  = position . get( 1 ) * Gaudi::Units::mm + theOrigin.x() ;
    double y  = position . get( 2 ) * Gaudi::Units::mm + theOrigin.y() ;
    double z  = position . get( 3 ) * Gaudi::Units::mm + theOrigin.z() ;

    // Create a new vertex corresponding to the decay vertex of
    // theMother and add it to theEvent
    HepMC::GenVertex * end_vtx = 
      new HepMC::GenVertex( HepMC::FourVector( x , y , z , t ) ) ;
    theMother -> parent_event() -> add_vertex( end_vtx ) ;
    end_vtx->add_particle_in( theMother ) ;
    
    for ( unsigned int it = 0 ; it < theEvtGenPart -> getNDaug() ; ++it ) {
      // For each daughter create a new HepMCParticle with correct
      // 4 momentum and PDG Id and with status 777 (= decayed with EvtGen)
      EvtVector4R momentum = theEvtGenPart -> getDaug( it ) -> getP4Lab() ;
      double e  = momentum . get( 0 ) * Gaudi::Units::GeV ;
      double px = momentum . get( 1 ) * Gaudi::Units::GeV ;
      double py = momentum . get( 2 ) * Gaudi::Units::GeV ;
      double pz = momentum . get( 3 ) * Gaudi::Units::GeV ;
      
      int id = EvtPDL::getStdHep( theEvtGenPart->getDaug( it )->getId() ) ;
      int status = LHCb::HepMCEvent::DecayedByDecayGen ;
      
      HepMC::GenParticle * prod_part = new
        HepMC::GenParticle( HepMC::FourVector(px,py,pz,e) , id , status ) ;
      
      end_vtx->add_particle_out( prod_part ) ;
      
      // Fill event with the daughter (recursive procedure)
      // if the particle is not of requested type
      if ( abs( id ) != targetId )
          makeHepMC( theEvtGenPart->getDaug(it) , prod_part , theOrigin , 
                     targetId ) ;
      // otherwise (particle has targetId pdg ID) it can be decayed further
      // by EvtGen (it is the signal particle in pratice) so give it 
      // status 888 unless it is a stable particle and then give it status 999
      else 
        {
          if ( theEvtGenPart->getDaug(it)->getNDaug() > 0 ) 
            prod_part -> 
              set_status
              ( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen ) ;
          else prod_part -> set_status( LHCb::HepMCEvent::StableInDecayGen ) ;
        }
    }
  }
  else {
    theMother -> set_status( LHCb::HepMCEvent::StableInDecayGen ) ;
  }
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Check if given Pdg Id is defined in the decay table
//=============================================================================
bool EvtGenDecay::isKnownToDecayTool( const int pdgId ) const {
  EvtId id = EvtPDL::evtIdFromStdHep( pdgId ) ;
  if ( id == EvtId(-1,-1) ) return false ;
  if ( 0  == EvtDecayTable::getInstance()->getNMode( id.getAlias() ) ) return false ;
  return true ;
}

//=============================================================================
// Create a temporary evt.pdl file taking available informations
// from the Particle Property Service
//=============================================================================
StatusCode EvtGenDecay::createTemporaryEvtFile( const boost::filesystem::path & 
                                                newFile ) const {
  // Create temporary file
  std::ofstream g( newFile.string().c_str() ) ;
  if ( ! g.is_open() ) return Error( "Cannot create evt.pdl file" ) ;

  // Write description of the file columns
  // Lines beginning with * are comments
  g << "* " << std::endl 
    << "*                  name                  id      mass/GeV    "
    << "width/GeV     "
    << "max_Dm/GeV    3*charge   2*spin    lifetime*c/mm    PythiaId   " 
    << std::endl ;

  g << std::left << std::fixed ;
  
  // retrieve Gaudi particle property service  
  LHCb::IParticlePropertySvc * ppSvc( 0 ) ;
  try { ppSvc = 
      svc< LHCb::IParticlePropertySvc > ( "LHCb::ParticlePropertySvc" , true ) ; }
  catch ( const GaudiException & exc ) {
    Exception( "Cannot open ParticlePropertySvc to fill EvtGen" , exc ) ;
  }

  // Loop over particle properties
  // and make a evt.pdl file with them

  // evry line starts with this command
  std::string cmnd = "add  p Particle " ;
  double      mass , ctau , pwidth ;
  int         charge , spin2 ;
  
  for ( LHCb::IParticlePropertySvc::iterator i = ppSvc->begin() ;
        i != ppSvc->end() ; ++i ) {
    if ( 0 == *i ) continue ;
    // if particle is not known to EvtGen, skip it
    if ( "unknown" == (*i)->evtGenName() ) continue ;

    g << cmnd << " " << std::scientific ;
    // Particle Name (EvtGen Name)
    g << std::setw( 22 ) << std::left << (*i) -> evtGenName ( ) << " " ;
    // PDG Id
    g << std::right << std::setw( 13 ) << (*i) -> pid().pid() << " " ;
    // Mass in GeV 
    mass   = (*i) -> mass() / Gaudi::Units::GeV ;
    // ctau in mm 
    ctau   = (*i) -> lifetime() * EvtConst::c /Gaudi::Units::s ;
    // Apply limits for ctau. If outside limits, sets it to 0
    // (that is to say 0 lifetime particle or stable particle)
    if ( ( ctau * Gaudi::Units::mm < m_minctau ) || 
         ( ctau * Gaudi::Units::mm > m_maxctau ) ) 
      ctau = 0. ;
    // Width in GeV 
    if ( (*i) -> lifetime() > 0. ) {
      pwidth = ( Gaudi::Units::hbarc / ( (*i) -> lifetime() * Gaudi::Units::c_light ) ) / Gaudi::Units::GeV ; 
    } else {
      pwidth = 0. ;
    }
    // Apply limit for the width. If width is below the limit,
    // set it to 0 and do not generate a Breit-Wigner lineshape
    // for this particle
    if ( pwidth * Gaudi::Units::GeV < m_minwidth ) pwidth = 0. ;
  
    g << std::setw( 14 ) << std::setprecision( 7 ) << mass << " " ;
    g << std::setw( 14 ) << std::setprecision( 7 ) << pwidth << " " ;
    // Max width deviation in GeV 
    g << std::setw( 14 ) << std::setprecision( 7 ) 
      << (*i) -> maxWidth() / Gaudi::Units::GeV << " " ;
    // 3 times particle charge
    charge = (int) floor( 3 * (*i) -> charge( ) + 0.5 ) ;
    g << std::setw( 5 ) << charge << " " ;
    // 2 times particle spin
    LHCb::ParticleID pid ( (*i) -> pid() ) ;
    if ( pid.jSpin() > 0 ) spin2 = pid.jSpin() - 1 ;
    else spin2 = getParticleSpin( pid ) - 1 ;
    g << std::setw( 5 ) << spin2 << " " ;
    // ctau
    g << std::setw( 14 ) << std::setprecision( 7 )
      << ctau << " " ;
    // PythiaID (ID with which Pythia knows the particle)
    // enables EvtGen to update particle masses and width in Pythia
    // common blocks
    g << std::setw( 10 ) << (*i) -> pythiaID() << std::left << std::endl ;
  }

  // File must en with key word "end"
  g << "end" << std::endl ;
  g.close() ;
  release( ppSvc ) ;

  return StatusCode::SUCCESS ;
}

//============================================================================
// Return the alias of the signal particle corresponding to the pdg Id
//============================================================================
const EvtId EvtGenDecay::getSignalAlias( int pdgId ) const {
  EvtId trueId = EvtPDL::evtIdFromStdHep( pdgId ) ;
  if ( EvtPDL::getStdHep( trueId ) == EvtPDL::getStdHep( m_signalId ) ) 
    return m_signalId ;
  else if ( EvtPDL::getStdHep( trueId ) == 
            EvtPDL::getStdHep( EvtPDL::chargeConj( m_signalId ) ) )
    return EvtPDL::chargeConj( m_signalId ) ;
  debug() << m_signalId << '\t' << trueId << '\t' << EvtPDL::getStdHep( trueId ) 
          << '\t' << EvtPDL::getStdHep( m_signalId ) << endmsg ; 
  Exception( "There is no signal ID corresponding to the pdgId" ) ;
  return m_signalId ;
}

//============================================================================
// Return 2J+1 for particles which are not supported by ParticleID
//============================================================================
int EvtGenDecay::getParticleSpin( const LHCb::ParticleID & pid ) const {
  // leptons
  if ( pid.isLepton() ) return 2 ;
  // quarks
  if ( 8 >= pid.abspid() ) return 2 ;
  // photon
  if ( 22 == pid.abspid() ) return 3 ;
  // He3 et Tritium
  if ( 490000000 == pid.abspid() ) return 2 ;
  if ( 460000000 == pid.abspid() ) return 2 ;
  // Cerenkov
  if ( 20022 == pid.abspid() ) return 3 ;
  // W+, W- and Z0
  if ( ( 23 == pid.abspid() ) || ( 24 == pid.abspid() ) ) return 3 ;
  // gluon or vpho
  if ( ( 21 == pid.abspid() ) || ( 10022 == pid.abspid() ) ) return 3 ;
  // Z', W' 
  if ( ( 32 == pid.abspid() ) || ( 33 == pid.abspid() ) ||
       ( 34 == pid.abspid() ) || ( 41 == pid.abspid() ) ) return 3 ;

  return 1 ;
}

//=============================================================================
// get the total branching fraction of the signal decay mode in the generic
// decay table.
//=============================================================================
double EvtGenDecay::getSignalBr( ) const {
  if ( EvtId( -1, -1) == m_signalId ) return 0. ; 
  if ( ! checkSignalPresence( ) ) return 0. ;
  return branching( m_signalId ) ;
}

//=============================================================================
// get branching of EvtId id in the generic decay table
//=============================================================================
double EvtGenDecay::branching( const EvtId& id ) const {
  double result = 0. ;

  if ( ! id.isAlias( ) ) return 1. ;

  for ( int i = 0 ; i < EvtDecayTable::getInstance()->getNMode( id.getAlias() ) ; ++i ) {
    // find corresponding decay mode in generic table
    EvtDecayBase * theDecAlias = EvtDecayTable::getInstance()->getDecay( id.getAlias() , i ) ;
    EvtId daugs_scratch[ 50 ] ;
    if ( theDecAlias -> getNDaug( ) > 50 ) return 1. ;
    for ( int k = 0 ; k < theDecAlias -> getNDaug( ) ; ++k ) {
      daugs_scratch[ k ] = EvtId ( theDecAlias -> getDaugs()[ k ].getId() ,
                                   theDecAlias -> getDaugs()[ k ].getId() ) ;
    }
    
    int index = EvtDecayTable::getInstance()->inChannelList( EvtId( id.getId(), id.getId() ),
							     theDecAlias -> getNDaug( ) ,
							     daugs_scratch ) ;
    if ( -1 == index ) return 1. ;

    EvtDecayBase * theTrueDecay = 
      EvtDecayTable::getInstance()->getDecay( id.getId() , index ) ;

    double tempBr = 1. ;
    
    for ( int j = 0 ; j < theDecAlias -> nRealDaughters( ) ; ++j ) {
      tempBr *= branching( theDecAlias -> getDaugs( )[ j ] ) ;
    }

    result += theTrueDecay -> getBranchingFraction( ) * tempBr ;
  }

  return result ;
}

//=============================================================================
// check if the signal decay mode is in the generic decay table.
//=============================================================================
bool EvtGenDecay::checkSignalPresence( ) const {
  if ( EvtId( -1 , -1 ) == m_signalId ) return false ;
  return checkGeneric( m_signalId ) ;
}

//=============================================================================
// Check if id exist in the generic decay table
//=============================================================================
bool EvtGenDecay::checkGeneric( const EvtId& id ) const {
  bool result = true ;
  
  if ( ! id.isAlias( ) ) return true ;
  
  for ( int i = 0 ; i < EvtDecayTable::getInstance()->getNMode( id.getAlias() ) ; ++i ) {
    EvtDecayBase * theDecAlias = EvtDecayTable::getInstance()->getDecay( id.getAlias() , i ) ;
    
    if ( theDecAlias -> getNDaug( ) > 50 ) return false ;
    EvtId daugs_scratch[ 50 ] ;
    for ( int k = 0 ; k < theDecAlias -> getNDaug( ) ; ++k ) {
      daugs_scratch[ k ] = EvtId ( theDecAlias -> getDaugs()[ k ].getId() ,
                                   theDecAlias -> getDaugs()[ k ].getId() ) ;
    }

    int index = EvtDecayTable::getInstance()->inChannelList( EvtId( id.getId(), id.getId() ),
							     theDecAlias -> getNDaug( ) ,
							     daugs_scratch ) ;

    if ( -1 == index ) return false ;
    
    for ( int j = 0 ; j < theDecAlias -> nRealDaughters( ) ; ++j ) {
      result = result && checkGeneric( theDecAlias -> getDaugs( ) [ j ] ) ;
    }
  }
  
  return result ;    
}


//=============================================================================
// Check if the HepMC particle is a good candidate
//=============================================================================
void EvtGenDecay::checkParticle( const HepMC::GenParticle * thePart ) const {
  if ( 0 == thePart -> production_vertex() ) 
    Exception( "Trying to decay a particle without production vertex" ) ;
  if ( 0 != thePart -> end_vertex ( ) ) 
    Exception( std::string( "Trying to add an end vertex to a " ) +
               std::string( "particle which has already one !" ) ) ;
}
  
//=============================================================================
// Generate with EvtGen
//=============================================================================
StatusCode EvtGenDecay::callEvtGen( EvtParticle * &thePart , 
                                   const HepMC::GenParticle * theHepMCPart ,
                                   const EvtId & eid ) 
const {
  // Get parameters of particle to generate ( theHepMCPart )
  // Type of particle and 4-momentum
  // The mother is created outside EvtGenDecay
  Gaudi::LorentzVector lVect( theHepMCPart -> momentum() ) ;
  
  EvtVector4R p_init ( lVect.e() / Gaudi::Units::GeV , 
                       lVect.px() / Gaudi::Units::GeV , 
                       lVect.py() / Gaudi::Units::GeV , 
                       lVect.pz() / Gaudi::Units::GeV ) ;
  
  // Create the corresponding EvtGen particle
  // If eid is not specified (no alias) take PID from theHepMCPart
  if ( eid == EvtId( -1, -1 ) ) {
    EvtId pid = EvtPDL::evtIdFromStdHep( theHepMCPart -> pdg_id() ) ;
    thePart   =  EvtParticleFactory::particleFactory( pid , p_init ) ;
  }
  else 
    thePart = EvtParticleFactory::particleFactory( eid , p_init ) ; 
 
  if ( 0 == thePart ) 
    return Error( std::string( "Could not create EvtParticle. \n" ) +
                  std::string( "There must be a configuration error." ) ) ;
  
  // sets diagonal polarization matrix for unpolarized decays for the
  // moment
  // or generate Polarized Lambda_b if requested
  if ( ( m_generatePolLambdab ) && 
       ( abs( theHepMCPart -> pdg_id() ) == 5122 ) ) {
    EvtSpinDensity rho ;
    rho.setDim( 2 ) ;
    rho.set( 0 , 0 , 1.0 ) ;
    rho.set( 0 , 1 , 0.0 ) ;
    rho.set( 1 , 0 , 0.0 ) ;
    rho.set( 1 , 1 , 0.0 ) ;
    thePart -> setSpinDensityForwardHelicityBasis( rho ) ;
  } else {
    thePart -> setDiagonalSpinDensity( ) ;
  }
  
  // Generates polarized charmonium if requested: 
  // Only if vector particle, is meson, and contains two charm quarks
  if ( m_generatePolCharmonium ) {
    if ( ( abs( theHepMCPart -> pdg_id() ) == 443 ) ||
         ( abs( theHepMCPart -> pdg_id() ) == 20443 ) ) {
      EvtSpinDensity rho ;
      rho.setDiag( thePart -> getSpinStates() ) ;
      rho.set( 0 , 0 , EvtComplex( m_realHelOne  , m_imHelOne  ) ) ;
      rho.set( 1 , 1 , EvtComplex( m_realHelZero , m_imHelZero ) ) ;
      rho.set( 2 , 2 , EvtComplex( m_realHelOne  , m_imHelOne  ) ) ;
      thePart -> setSpinDensityForwardHelicityBasis( rho ) ;    
    } else if ( 445 == abs( theHepMCPart -> pdg_id() ) ) {
         // WHAT TO DO HERE FOR TENSOR PARTICLES ? 
    }
  }

  // Generate decay tree of part with EvtGen  
  m_gen -> generateDecay( thePart ) ;
  if ( msgLevel( MSG::DEBUG ) ) thePart -> printTree() ;

  // Check EvtGen result:
  if ( ! thePart -> isDecayed() ) 
    return Error( "EvtGen particle not decayed" ) ;

  return StatusCode::SUCCESS ;
}

//=============================================================================
// Set signal ID
//=============================================================================
void EvtGenDecay::setSignal( const int pdgSignal ) 
{
  EvtId theSignal = EvtPDL::evtIdFromStdHep( abs( pdgSignal ) ) ;
  std::string nameSignal = EvtPDL::name( theSignal ) + "sig" ;
  m_signalId = EvtPDL::getId( nameSignal ) ;
}

//=============================================================================
// Implementation of class EvtGenGaudiRandomEngine
//=============================================================================
//=============================================================================
// Constructor
//=============================================================================
EvtGenGaudiRandomEngine::EvtGenGaudiRandomEngine( IRndmGenSvc* i , 
                                                  StatusCode & sc ) {
  // Initialize Random Engine with flat generator
  sc = m_randomgaudi.initialize( i , Rndm::Flat( 0 , 1 ) ) ;      
}

//=============================================================================
// Destructor
//=============================================================================
EvtGenGaudiRandomEngine::~EvtGenGaudiRandomEngine( ) { 
  m_randomgaudi.finalize( ) ;
}

