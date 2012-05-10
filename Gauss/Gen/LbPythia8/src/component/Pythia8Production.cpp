// $Id: Pythia8Production.cpp,v 1.9 2010-03-01 13:11:54 robbep Exp $

// Include files

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/PhysicalConstants.h"
 
// from Event
#include "Event/GenCollision.h"

// from Pythia8
#include "Pythia.h"
#include "HepMCInterface.h"

// Generators 
#include "Generators/IBeamTool.h"

// HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/HEPEVT_Wrapper.h"

// LbPythia8
#include "LbPythia8/GaudiRandomForPythia8.h" 

// local
#include "Pythia8Production.h"

//-----------------------------------------------------------------------------
//  Implementation file for class: Pythia8Production
//
//  2007-07-31 : Arthur de Gromard
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Pythia8Production );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Pythia8Production::Pythia8Production( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent ) , 
    m_defaultSettings() , 
    m_id1( 0 ) , 
    m_id2( 0 ) , 
    m_engCM( 0. ) ,
    m_beamTool( 0 ) ,
    m_pythia( 0 ) ,
    m_nEvents( 0 ) ,
    m_randomEngine( 0 ) ,
    m_validate_HEPEVT ( false ) ,// force the valiadation of I_Pythia8 
    m_inconsistencies ( "HEPEVT_inconsistencies.out" ) ,
    m_HEPEVT_errors ( 0 ) {
  
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "Commands" , m_commandVector ) ;
  declareProperty( "BeamToolName" , m_beamToolName = "CollidingBeams" ) ;
  declareProperty( "ValidateHEPEVT"  , m_validate_HEPEVT = false ); //The flag to force the validation (mother&daughter) of HEPEVT
  declareProperty( "Inconsistencies" , m_inconsistencies ); //The file to dump HEPEVT inconsinstencies
  declareProperty( "ListAllParticles", m_listAllParticles = false ); //list all particles

  // Set the default settings for Pythia8 here:
  m_defaultSettings.clear() ;
  m_defaultSettings.push_back( "Beam Settings"); 
  m_defaultSettings.push_back( "PDG id code for the first incoming particle");
  m_defaultSettings.push_back( "PDG id code for the second incoming particle");
  m_defaultSettings.push_back( "Mode commands for Pythia8" ); 
  //In a first time, cutting off all processes remaining
  m_defaultSettings.push_back( "SoftQCD:all = off" );
  m_defaultSettings.push_back( "HardQCD:all = off" );
  m_defaultSettings.push_back( "PromptPhoton:all = off" );
  m_defaultSettings.push_back("WeakBosonExchange:all = off" );
  m_defaultSettings.push_back( "WeakSingleBoson:all = off" );
  m_defaultSettings.push_back("WeakDoubleBoson:all = off") ;
  m_defaultSettings.push_back( "WeakBosonAndParton:all = off" );
  m_defaultSettings.push_back("Charmonium:all = off" );
  m_defaultSettings.push_back("Bottomonium:all = off" );
  m_defaultSettings.push_back("Top:all = off" );
  m_defaultSettings.push_back("Process commands for Pythia8" );
  //Then, setting the different processes    
  /* m_defaultSettings.push_back("HardQCD:qq2qq = on");
  m_defaultSettings.push_back("HardQCD:qqbar2ccbar = on");
  m_defaultSettings.push_back("HardQCD:qqbar2bbbar = on");
  m_defaultSettings.push_back("HardQCD:qqbar2qqbarNew = on");
  m_defaultSettings.push_back("HardQCD:qqbar2gg = on");
  m_defaultSettings.push_back("HardQCD:qg2qg = on");
  m_defaultSettings.push_back("HardQCD:gg2qqbar = on");
  m_defaultSettings.push_back("HardQCD:gg2ccbar = on");
  m_defaultSettings.push_back("HardQCD:gg2bbbar = on");
  m_defaultSettings.push_back("HardQCD:gg2gg = on");
  m_defaultSettings.push_back("SoftQCD:elastic = on");
  m_defaultSettings.push_back("SoftQCD:singleDiffractive = on");
  m_defaultSettings.push_back("SoftQCD:doubleDiffractive = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[3S1(1)]g = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[3P0(1)]g = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[3P1(1)]g = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[3P2(1)]g = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[3S1(8)]g = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[1S0(8)]g = on");
  m_defaultSettings.push_back("Charmonium:gg2QQbar[3S1(1)]g = on");*/
  m_defaultSettings.push_back("SigmaProcess:alphaSorder = 2");
  m_defaultSettings.push_back("MultipartonInteractions:Kfactor = 3.0");
  m_defaultSettings.push_back("MultipartonInteractions:bProfile = 1");
  m_defaultSettings.push_back("MultipartonInteractions:pTmin = 4.28");
  m_defaultSettings.push_back("MultipartonInteractions:ecmRef = 14000");
  m_defaultSettings.push_back("MultipartonInteractions:ecmPow = 0.162");
  m_defaultSettings.push_back("StringFlav:mesonCL1S0J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonCL1S1J0 = 0.0135");
  m_defaultSettings.push_back("StringFlav:mesonCL1S1J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonCL1S1J2 = 0.0675");
  m_defaultSettings.push_back("StringFlav:mesonBL1S0J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonBL1S1J0 = 0.0135");
  m_defaultSettings.push_back("StringFlav:mesonBL1S1J1 = 0.0405");
  m_defaultSettings.push_back("StringFlav:mesonBL1S1J2 = 0.0675");
  m_defaultSettings.push_back("PDF:useLHAPDF = on");
  //Settings for LHAPDF  
  m_defaultSettings.push_back("PDF:LHAPDFset = cteq6l.LHpdf");
  m_defaultSettings.push_back("PDF:LHAPDFmember = 1");
  m_defaultSettings.push_back("PDF:extrapolateLHAPDF = off");    
  m_defaultSettings.push_back("Output commands for Pythia8");
}

//=============================================================================
// Destructor 
//=============================================================================
Pythia8Production::~Pythia8Production( ) { }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Pythia8Production::initialize( ) {  
  //Initializing Gaudi tool
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  always() << "============================================================="
           << endmsg;
  always() << "Using as production engine  " << this->type() << endmsg;
  always() << "============================================================="
           << endmsg;
  
  //Initializing the beam tool
  m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;
  
  // Get XMLDOC path
  std::string xmlpath = "" ;
  if ( "UNKNOWN" != System::getEnv("PYTHIA8XML") ) 
    xmlpath  = System::getEnv( "PYTHIA8XML" ) ;

  //Initializing the pythia object
  m_pythia = new Pythia8::Pythia( xmlpath );
  
  //Setting the random generator
  IRndmGenSvc* randSvc( 0 );
  try { randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ; }
  catch ( const GaudiException & exc ) {
    Exception( "RndmGenSvc not found to initialize Pythia8 random engine" ) ;
  }
  
  m_randomEngine = new GaudiRandomForPythia8( randSvc , sc ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Cannot initialize GaudiRandomForPythia8" , sc ) ;
  release( randSvc ) ;
  
  m_pythia -> setRndmEnginePtr( m_randomEngine );

  return initializeGenerator() ;
}

//=============================================================================
// Part specific to generator initialization
//=============================================================================
StatusCode Pythia8Production::initializeGenerator( ) {
  m_id1 = 2212;
  m_id2 = 2212;
  bool success = true ;
  StatusCode sc = StatusCode::SUCCESS ;
  int i = 4 ;
  double mass1, mass2;
  
  // Initializing default settings
  for (unsigned int count = 4; count<m_defaultSettings.size(); ++count) {
    if ((!(m_defaultSettings[count]=="Process commands for Pythia8")) 
        && (!(m_defaultSettings[count]=="Output commands for Pythia8"))) {
      success = m_pythia->readString(m_defaultSettings[count]);
    }
    i = count ;
  }
  if (!success) {
    return Error("CHECK DEFAULT COMMANDS::: Pythia did not find input string in settings databases");
  }

  m_pythia->readString("Main:inCMFrame = on");
  if ( ! m_commandVector.empty() ) {
    //Initializing user settings
    if (!(m_commandVector[0]=="Beam Settings")) {
      return Error( "Syntax error" ) ;
    } else {
      //Initializing settings for the beams
      if (m_commandVector[2]=="PDG id code for the second incoming particle") {
        m_id1 = 2212;
        i = 3;
      } else {
        m_id1 = atoi(m_commandVector[2].c_str());
        i = 4;
      }
      if (m_commandVector[i]=="Mode commands for Pythia8") {
        m_id2 = 2212;     
        i = 4;
      } else {
        m_id2 = atoi(m_commandVector[i].c_str());      
        i = i + 2;
      } 
    }  
  }
  
    
  Gaudi::XYZVector pBeam1 , pBeam2 ;
  m_beamTool->getMeanBeams( pBeam1 , pBeam2 ) ;  
  // retrieve Gaudi particle property service
  IParticlePropertySvc* ppSvc( 0 ) ;
  try { ppSvc = svc< IParticlePropertySvc > ( "Gaudi::ParticlePropertySvc" , 
                                              true ) ; }
  catch ( const GaudiException & exc ) {
    Exception( "Cannot open ParticlePropertySvc to fill EvtGen" , exc ) ;
  }
  
  mass1 = (ppSvc -> findByStdHepID(m_id1)) -> mass();
  mass2 = (ppSvc -> findByStdHepID(m_id2)) -> mass();
  
  m_engCM = (sqrt(pBeam1.Dot(pBeam1)+mass1*mass1) +  
             sqrt(pBeam2.Dot(pBeam2) + mass2*mass2))/Gaudi::Units::GeV;
  
  
  //Initializing the settings for the generator
  //First cut all processes in case of user settings for processes
  for (unsigned int count = i; count<m_commandVector.size(); ++count) {	 
    if (m_commandVector[count]=="Do user change the processes settings ?") {
      if (m_commandVector[count + 1]=="yes") {  
        m_pythia->readString("HardQCD:qq2qq = off");
        m_pythia->readString("HardQCD:qqbar2ccbar = off");
        m_pythia->readString("HardQCD:qqbar2bbbar = off");
        m_pythia->readString("HardQCD:qqbar2qqbarNew = off");
        m_pythia->readString("HardQCD:qqbar2gg = off");
        m_pythia->readString("HardQCD:qg2qg = off");
        m_pythia->readString("HardQCD:gg2qqbar = off");
        m_pythia->readString("HardQCD:gg2ccbar = off");
        m_pythia->readString("HardQCD:gg2bbbar = off");
        m_pythia->readString("HardQCD:gg2gg = off");
        m_pythia->readString("SoftQCD:elastic = off");
        m_pythia->readString("SoftQCD:singleDiffractive = off");
        m_pythia->readString("SoftQCD:doubleDiffractive = off");
        m_pythia->readString("Charmonium:gg2QQbar[3S1(1)]g = off");
        m_pythia->readString("Charmonium:gg2QQbar[3P0(1)]g = off");
        m_pythia->readString("Charmonium:gg2QQbar[3P1(1)]g = off");
        m_pythia->readString("Charmonium:gg2QQbar[3P2(1)]g = off");
        m_pythia->readString("Charmonium:gg2QQbar[3S1(8)]g = off");
        m_pythia->readString("Charmonium:gg2QQbar[1S0(8)]g = off");
        m_pythia->readString("Charmonium:gg2QQbar[3S1(1)]g = off");
        m_pythia->readString("StringFlav:mesonCL1S0J1 = 0");
        m_pythia->readString("StringFlav:mesonCL1S1J0 = 0");
        m_pythia->readString("StringFlav:mesonCL1S1J1 = 0");
        m_pythia->readString("StringFlav:mesonCL1S1J2 = 0");
        m_pythia->readString("StringFlav:mesonBL1S0J1 = 0");
        m_pythia->readString("StringFlav:mesonBL1S1J0 = 0");
        m_pythia->readString("StringFlav:mesonBL1S1J1 = 0");
        m_pythia->readString("StringFlav:mesonBL1S1J2 = 0");
        count = count + 2;
      }
      if (m_commandVector[count + 1]=="no")
        count = count + 2;
    }
    //Then initializing user settings for processes and output
    if ((!(m_commandVector[count]=="Process commands for Pythia8")) 
        && (!(m_commandVector[count]=="Output commands for Pythia8"))) {
      success = m_pythia->readString(m_commandVector[count]);
    }
  }
  
  if (!success) sc = StatusCode::FAILURE;

  info() << " will INIT" << endmsg;
  m_pythia->init(m_id1, m_id2, m_engCM) ;
  info() << " FINISHED INIT" << endmsg;
  return sc;
}

//=============================================================================
//   Function called to generate one event with Pythia8
//=============================================================================
StatusCode Pythia8Production::generateEvent( HepMC::GenEvent * theEvent , 
                                             LHCb::GenCollision * 
                                             theCollision ) 
{
  // Generate Event
  m_pythia->next();
  m_event = m_pythia->event;  
  // Update event counter
  ++m_nEvents ;
  return toHepMC( theEvent, theCollision ) ;
}

//=============================================================================
// Set stable the given particle in Pythia8
//=============================================================================
void Pythia8Production::setStable( const ParticleProperty * thePP ) {
  int pdgId = thePP -> pdgID();
  m_pythia->particleData.mayDecay(pdgId, false);
}

//=============================================================================
// Update particle properties
//=============================================================================
void Pythia8Production::updateParticleProperties( const ParticleProperty * 
                                                  thePP ) {
  int pythiaId = thePP -> pythiaID() ;
  int pdgId = thePP -> pdgID();
  double pwidth , lifetime ;
  if ( 0 != pythiaId ) {
    if ( 0 == thePP -> lifetime() ) pwidth = 0. ;
    else pwidth = ( Gaudi::Units::hbarc / ( thePP -> lifetime() * Gaudi::Units::c_light ) ) ;
    if ( pwidth < ( 1.5e-6 * Gaudi::Units::GeV ) ) pwidth = 0. ;
    
    lifetime =  thePP -> lifetime() * Gaudi::Units::c_light ;
    if ( ( lifetime <= 1.e-4 * Gaudi::Units::mm ) || ( lifetime >= 1.e16 * Gaudi::Units::mm ) ) 
      lifetime = 0. ;
    
    m_pythia -> particleData.m0(pdgId, thePP -> mass() / Gaudi::Units::GeV) ;
    
    // For Higgs, top, Z and W: update only masses
    if ( ( 6 != pdgId ) && ( 23 != pdgId ) && ( 24 != pdgId ) 
         && ( 25 != pdgId ) ) {
      m_pythia -> particleData.mWidth(pdgId, pwidth / Gaudi::Units::GeV) ;
      m_pythia -> particleData.mMax(pdgId, thePP -> maxWidth() / Gaudi::Units::GeV) ;
      m_pythia -> particleData.tau0(pdgId, lifetime / Gaudi::Units::mm) ;
    }
  }
}

//=============================================================================
// Retrieve the Hard scatter information
//=============================================================================
void Pythia8Production::hardProcessInfo( LHCb::GenCollision * theCollision ) {
  theCollision -> setProcessType( m_pythia -> info.codeSub() ) ;
  theCollision -> setSHat( m_pythia -> info.sHat() );
  theCollision -> setTHat( m_pythia -> info.tHat() );
  theCollision -> setUHat( m_pythia -> info.uHat() );
  theCollision -> setPtHat( m_pythia -> info.pTHat() );
  theCollision -> setX1Bjorken( m_pythia -> info.x1() );
  theCollision -> setX2Bjorken( m_pythia -> info.x2() );
} 

//=============================================================================
// Finalize method
//=============================================================================
StatusCode Pythia8Production::finalize( ) {
  m_pythia -> statistics() ;
  delete m_randomEngine;
  delete m_pythia ;
  return GaudiTool::finalize( ) ;
}  


//=============================================================================
// Print Pythia8 parameters
//=============================================================================
void Pythia8Production::printPythiaParameter( ) {
  m_pythia->statistics();
}

//=============================================================================
// Turn on fragmentation in Pythia8
//=============================================================================
void Pythia8Production::turnOnFragmentation( )
{
  //this facility is not needed anymore within pythia8
}

//=============================================================================
// Turn off fragmentation in Pythia8
//=============================================================================
void Pythia8Production::turnOffFragmentation( ) 
{
  //this facility is not needed anymore within pythia8
}

//=============================================================================
// Save parton event
//=============================================================================
void Pythia8Production::savePartonEvent( HepMC::GenEvent * /* theEvent */ ) 
{
  m_event = m_pythia->event;
}

//=============================================================================
// Load parton event
//=============================================================================
void Pythia8Production::retrievePartonEvent( HepMC::GenEvent * /* theEvent */ )
{
  m_pythia->event = m_event;
}

//=============================================================================
// Hadronize Pythia8 event
//=============================================================================
StatusCode Pythia8Production::hadronize( HepMC::GenEvent * theEvent , 
                                         LHCb::GenCollision * 
                                         theCollision ) {
  if (!m_pythia->forceHadronLevel()) return StatusCode::FAILURE ;
  return toHepMC ( theEvent , theCollision ) ;
}

//=============================================================================
// Debug print out to be printed after all initializations
//=============================================================================
void Pythia8Production::printRunningConditions( ) { 
  if (m_nEvents==0 && m_listAllParticles==true) m_pythia->particleData.listAll();
  if (msgLevel(MSG::DEBUG)) m_pythia->settings.listAll();
  else m_pythia->settings.listChanged();
}

//=============================================================================
// Retrieve the process code information
//=============================================================================
int Pythia8Production::processCode( ) { 
  if (m_pythia->info.hasSub())
    return m_pythia -> info.codeSub();
  else
    return m_pythia -> info.code();
}

//=============================================================================
// TRUE if the particle is a special particle which must not be modify
//=============================================================================
bool Pythia8Production::isSpecialParticle( const ParticleProperty * thePP ) 
  const { 
  switch ( abs( thePP -> pdgID() ) ) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 17:
  case 18:
  case 21:
  case 110:
  case 990:
    //case 32:
  case 33:
  case 34:
    //case 35:
    //case 36:
  case 37:
  case 39:
  case 41:
  case 42:
  case 43:
  case 44:
  case 81:
  case 82:
  case 83:
  case 84:
  case 85:
  case 88:
  case 89:
  case 90:
  case 91:
  case 92:
  case 93:
  case 94:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 9900110:
  case 9900210:
  case 9900220:
  case 9900330:
  case 9900440:
  case 9902110:
  case 9902210:
  case 1101:
  case 1103:
  case 2101:
  case 2103:
  case 2201:
  case 2203:
  case 3101:
  case 3103:
  case 3201:
  case 3203:
  case 3301:
  case 3303:
  case 4101:
  case 4103:
  case 4201:
  case 4203:
  case 4301:
  case 4303:
  case 4401:
  case 4403:
  case 5101:
  case 5103:
  case 5201:
  case 5203:
  case 5301:
  case 5303:
  case 5401:
  case 5403:
  case 5501:
  case 5503:
  case 1000022:
  case 1000024:
    return true ;
    break ;
  default:
    return false ;
    break ;
  }
  return false ;    
}

//=============================================================================
// Setup for forced fragmentation 
//=============================================================================
StatusCode Pythia8Production::setupForcedFragmentation( const int 
                                                        /*thePdgId*/ ) {
  m_pythia->readString("PartonLevel:all = off");
  return StatusCode::SUCCESS ;  
}

// ============================================================================
/// PYTHIA -> HEPEVT -> HEPMC 
// ============================================================================
StatusCode Pythia8Production::toHepMC ( HepMC::GenEvent*     theEvent    , 
                                        LHCb::GenCollision * theCollision ){
  StatusCode sc = StatusCode::SUCCESS ;
  
  //Convert from Pythia8 format to HepMC format
  HepMC::I_Pythia8 conversion ;

  // Force the verification of the HEPEVT  record 
  if ( m_validate_HEPEVT ) 
  { 
    conversion.set_trust_both_mothers_and_daughters ( true ) ;
    conversion.set_trust_mothers_before_daughters   ( true ) ;
    conversion.set_print_inconsistency_errors       ( false );
    if ( msgLevel( MSG::DEBUG ) ) 
      conversion.set_print_inconsistency_errors       ( true );
    
    if ( !m_inconsistencies.empty() && 0 == m_HEPEVT_errors )
    { m_HEPEVT_errors = new std::ofstream ( m_inconsistencies.c_str() ) ; }
    
    if ( 0 != m_HEPEVT_errors ) 
    {
      if ( !HepMC::HEPEVT_Wrapper::check_hepevt_consistency ( *m_HEPEVT_errors ) )
      { Warning ( "Inconsistencies in HEPEVT structure are found" ) ; }   
    }
    else 
    {
      MsgStream& log = warning() ;
      if ( !HepMC::HEPEVT_Wrapper::check_hepevt_consistency ( log.stream() ) )
      {
        log << endreq ;
        Warning ( "Inconsistencies in HEPEVT structure are found" ) ; 
      } 
    }
  }

  if (!(conversion.fill_next_event( *m_pythia , theEvent ))) 
    return Error( "Cannot convert Pythia8 event to HepMC" ) ;
  
  // Now convert to LHCb units:
  for ( HepMC::GenEvent::particle_iterator p = theEvent -> particles_begin() ;
        p != theEvent -> particles_end() ; ++p ) {
  
    int status = (*p) -> status() ;

     if (status>3 && status<20)
	(*p) -> set_status( LHCb::HepMCEvent::DocumentationParticle );
    else if (status>19 && status<80)
        (*p) -> set_status( LHCb::HepMCEvent::DecayedByProdGen );
    else if (status==91 || status==92 || (status>99 && status<110)) {
        if ((*p) -> end_vertex()!=0)
           (*p) -> set_status( LHCb::HepMCEvent::DecayedByProdGen );
        else
           (*p) -> set_status( LHCb::HepMCEvent::StableInProdGen );
    }
    else if (status==93 || status==94)
	(*p) -> set_status( LHCb::HepMCEvent::DecayedByProdGen );
    else if (status==99)
	(*p) -> set_status( LHCb::HepMCEvent::DocumentationParticle );
    else if (status!=LHCb::HepMCEvent::DecayedByProdGen
	  && status!=LHCb::HepMCEvent::StableInProdGen
          && status!=LHCb::HepMCEvent::DocumentationParticle)
        warning() << "Unknown status rule " << status << " for particle" << (*p)->pdg_id() << endmsg;

    (*p) -> set_momentum( HepMC::FourVector( 
                           (*p) -> momentum().px() * Gaudi::Units::GeV ,
                           (*p) -> momentum().py() * Gaudi::Units::GeV , 
                           (*p) -> momentum().pz() * Gaudi::Units::GeV , 
                           (*p) -> momentum().e() * Gaudi::Units::GeV )
                         );
    (*p) -> set_generated_mass( (*p)-> generated_mass() * Gaudi::Units::GeV ) ;
  }
  
  for ( HepMC::GenEvent::vertex_iterator v = theEvent -> vertices_begin() ;
        v != theEvent -> vertices_end() ; ++v ) {
    HepMC::FourVector newPos ;
    newPos.setX( (*v) -> position().x() ) ;
    newPos.setY( (*v) -> position().y() ) ;
    newPos.setZ( (*v) -> position().z() ) ;
    newPos.setT( ( (*v) -> position().t() * Gaudi::Units::mm ) 
                 / Gaudi::Units::c_light ) ;    
    (*v) -> set_position( newPos ) ;
  }

  theEvent -> set_signal_process_id( processCode() ) ;

  hardProcessInfo( theCollision ) ;
  
  return sc;
}
// ============================================================================


// ============================================================================
/// The END 
// ============================================================================
