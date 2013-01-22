// $Id: Pythia8Production.cpp,v 1.9 2010-03-01 13:11:54 robbep Exp $

// Include files

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
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
#include "LbPythia8/BeamToolForPythia8.h"
#include "LbPythia8/ILHAupFortranTool.h"

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
    m_xmlLogTool ( 0 ) ,
    m_beamTool( 0 ) ,
    m_pythia( 0 ) ,
    m_nEvents( 0 ) ,
    m_randomEngine( 0 ) ,
    m_fortranUPTool( 0 ) ,
    m_validate_HEPEVT ( false ) ,// force the valiadation of I_Pythia8 
    m_inconsistencies ( "HEPEVT_inconsistencies.out" ) ,
    m_HEPEVT_errors ( 0 ) {
  
  declareInterface< IProductionTool >( this ) ;
  declareProperty( "Commands" , m_commandVector ) ;
  declareProperty( "BeamToolName" , m_beamToolName = "CollidingBeams" ) ;
  declareProperty( "ValidateHEPEVT"  , m_validate_HEPEVT = false ); //The flag to force the validation (mother&daughter) of HEPEVT
  declareProperty( "Inconsistencies" , m_inconsistencies ); //The file to dump HEPEVT inconsinstencies
  declareProperty( "ListAllParticles", m_listAllParticles = false ); //list all particles
  declareProperty( "Tuning", m_tuningFile = "LHCbDefault.cmd");
  declareProperty( "UserTuning", m_tuningUserFile = ""); //a default Pythia8 tune using the Tune: 'subrun' would overwrite LHCb defaults, if chosen here...
  declareProperty( "PythiaUserProcessTool" , m_fortranUPToolName = "" ) ;
  declareProperty( "LHAupOptionFile" , m_LHAupOptionFile = "LHAup.cmd");
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

  //Initializing the beam tool
  m_beamTool = tool< IBeamTool >( m_beamToolName , this ) ;
  m_pythiaBeamTool = new BeamToolForPythia8( m_beamTool, m_pythia->settings, sc );
  if ( ! sc.isSuccess() )
    return Error( "Cannot initialize BeamToolForPythia8" , sc ) ;
  m_pythia -> setBeamShapePtr( m_pythiaBeamTool );

  // XML log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  return initializeGenerator() ;
}

//=============================================================================
// Part specific to generator initialization
//=============================================================================
StatusCode Pythia8Production::initializeGenerator( ) {
  bool success = true ;
  
  //set the mean beam momentum to the pythia instance   
  //pythia beam tool smearing is using difference wrt to nominal beam directions
  Gaudi::XYZVector pBeam1 , pBeam2 ;
  m_beamTool->getMeanBeams( pBeam1 , pBeam2 ) ;
  pBeam1 /= Gaudi::Units::GeV;
  pBeam2 /= Gaudi::Units::GeV;
  std::ostringstream momProj;
  momProj << pBeam1.X();
  m_pythia->readString("Beams:pxA = " + momProj.str());
  momProj.str("");
  momProj << pBeam1.Y();
  m_pythia->readString("Beams:pyA = " + momProj.str());
  momProj.str("");
  momProj << pBeam1.Z();
  m_pythia->readString("Beams:pzA = " + momProj.str());
  momProj.str("");
  momProj << pBeam2.X();
  m_pythia->readString("Beams:pxB = " + momProj.str());
  momProj.str("");
  momProj << pBeam2.Y();
  m_pythia->readString("Beams:pyB = " + momProj.str());
  momProj.str("");
  momProj << pBeam2.Z();
  m_pythia->readString("Beams:pzB = " + momProj.str());
  momProj.str("");

  // set default LHCb tuning options
  std::string optspath = "" ;
  if ( "UNKNOWN" != System::getEnv("LBPYTHIA8ROOT") ) {
    optspath  = System::getEnv( "LBPYTHIA8ROOT" ) ;
    success = m_pythia->readFile(optspath+"/options/"+m_tuningFile);
    if (!success)
      Warning ( "Cannot find  LBPYTHIA8ROOT/options/"+m_tuningFile+", thus default pythia8 options are parsed" ) ; 
  }
  else 
    Warning ( "Cannot find LBPYTHIA8ROOT/options/"+m_tuningFile+", thus default pythia8 options are parsed" ) ;
  
  // add user defined tuning options
  if (m_tuningUserFile!="")
    success = m_pythia->readFile(m_tuningUserFile);
  if (!success)
    Warning ( "Cannot find "+m_tuningUserFile+", thus default LHCb tune is not overwritten" ) ;
  

  // Check if there is a FORTRAN User Process tool
  if ( "" != m_fortranUPToolName ) {
    m_fortranUPTool = tool< ILHAupFortranTool >( m_fortranUPToolName , this ) ;
    if ( "UNKNOWN" != System::getEnv("LBPYTHIA8ROOT") ) {
      optspath  = System::getEnv( "LBPYTHIA8ROOT" ) ;
      success = m_pythia->readFile(optspath+"/options/"+m_LHAupOptionFile);
      if (!success)
        Warning ( "Cannot find  LBPYTHIA8ROOT/options/"+m_LHAupOptionFile+", thus default pythia8 options are parsed" ) ;
    }
    else
      Warning ( "Cannot find LBPYTHIA8ROOT/options/"+m_LHAupOptionFile+", thus default pythia8 options are parsed" ) ;
    m_pythia -> setLHAupPtr( ( Pythia8::LHAup * ) m_fortranUPTool -> getLHAupPtr() ) ;
  }


  // also read a vector of commands if any is provided, for backward compatibility
  // this will overwrite anything that is passed through a user tuning file
  // should be done as the last step of the reading of the options,
  // so that everything can be overwritten by the user.
  for (unsigned int count = 0; count<m_commandVector.size(); ++count) {  
    cout << m_commandVector[count] << endl;
    success = m_pythia->readString(m_commandVector[count]);
  }
  
  m_pythia->init();
  return StatusCode::SUCCESS;
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
  //TODO
  //maybe this can be moved to somewhere else, since not quite needed if generating MinBias
  //--
  m_event = m_pythia->event;  

  // Update event counter
  ++m_nEvents ;
  return toHepMC( theEvent, theCollision ) ;
}

//=============================================================================
// Set stable the given particle in Pythia8
//=============================================================================
void Pythia8Production::setStable( const LHCb::ParticleProperty * thePP ) {
  int pdgId = thePP -> pid().pid();
  m_pythia->particleData.mayDecay(pdgId, false);
}

//=============================================================================
// Update particle properties
//=============================================================================
void Pythia8Production::updateParticleProperties( const LHCb::ParticleProperty * 
                                                  thePP ) {
  
  //this can probalbly be passed by otpions instead. (and read through the readFile(...))
  
  int pythiaId = thePP -> pythiaID() ;
  int pdgId = thePP -> pid().pid();
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
  theCollision -> setProcessType( processCode() ) ;
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

  // Write in the XML file the cross-sections
  m_xmlLogTool -> addCrossSection( "",//how to pass the name here ?
                                   0 , 
                                   m_pythia -> info.nAccepted(0) , 
                                   m_pythia -> info.sigmaGen(0) ) ;
  

  for (int i = 1 ; i <= 500 ; ++i ) {
    if ( 0 != m_pythia -> info.nAccepted(i) ) {
      m_xmlLogTool -> addCrossSection( "",//same question have above ?
				       i ,
				       m_pythia -> info.nAccepted(i) ,
				       m_pythia -> info.sigmaGen(i) ) ;
    }
  }

  delete m_randomEngine;
  delete m_pythia ;
  if ( 0 != m_fortranUPTool ) release( m_fortranUPTool ) ;
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
  if (m_nEvents==0 && m_listAllParticles==true && msgLevel(MSG::DEBUG)) m_pythia->particleData.listAll();
  if (msgLevel(MSG::VERBOSE)) m_pythia->settings.listAll();
  else if (msgLevel(MSG::DEBUG)) m_pythia->settings.listChanged();
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
bool Pythia8Production::isSpecialParticle( const LHCb::ParticleProperty * thePP ) 
  const { 
  switch ( thePP -> pid().abspid() ) {
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
  debug() << "momentum unit is " << theEvent->momentum_unit() << endmsg;
  
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
