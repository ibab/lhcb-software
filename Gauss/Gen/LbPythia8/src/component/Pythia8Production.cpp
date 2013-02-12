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

  // XML log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  bool success = true ;
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

  // also read a vector of commands if any is provided
  // this will overwrite anything that is passed through a user tuning file
  // should be done as the last step of the reading of the options,
  // so that everything can be overwritten by the user.
  for (unsigned int count = 0; count<m_commandVector.size(); ++count) {
    debug() << m_commandVector[count] << endmsg;
    success = m_pythia->readString(m_commandVector[count]);
  }

  return success ;
}

//=============================================================================
// Part specific to generator initialization
//=============================================================================
StatusCode Pythia8Production::initializeGenerator( ) {
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
// Convert the PythiaID (stored in ParticleTable.txt, comes from pythia6) 
// and the PDGID in a Pythia8ID
// this can be change once Pythia6 is definitely retired.
//=============================================================================
int Pythia8Production::getPythia8ID( const LHCb::ParticleProperty * thePP ) {
  int pdgId = thePP -> pid().pid();
  int pythiaId = thePP -> pythiaID() ;

  int pythia8Id = 0;
  
  switch ( thePP -> pid().abspid() ) {
  case 9010221:
  case 10331:
  case 10111:
  case 10211:
  case 9000111:
  case 9000211:
  case 100113:
  case 100213:
  case 30313:
  case 30213:
  case 3124:
  case 13122:
  case 23122:
  case 33122:
    //for these guys the pythiaId is now corresponding to the pdgId
    pythia8Id = pdgId;
    break;
  case 30221:
    //the pdgID has changed, and the new pythiaId is the same as new pdg
    pythia8Id = pdgId>0 ? 10221 : -10221;
    break;
  case 104122:
    //the pdgID has changed, and the new pythiaId is the same as new pdg
    pythia8Id = pdgId>0 ? 4124 : -4124;
    break;
  default:
    //else pdgId=pythiaId=pythia8Id 
    //(except for unknown particles to pythia, for which pythiaId=pythia8Id=0)
    pythia8Id = pythiaId;
    break;
  }
  return pythia8Id;
}

//=============================================================================
// Set stable the given particle in Pythia8
//=============================================================================
void Pythia8Production::setStable( const LHCb::ParticleProperty * thePP ) {
  //  int pythiaId = thePP -> pythiaID() ;
  int pythiaId = getPythia8ID(thePP);
  m_pythia->particleData.mayDecay(pythiaId, false);
}

//=============================================================================
// Update particle properties
//=============================================================================
void Pythia8Production::updateParticleProperties( const LHCb::ParticleProperty * 
                                                  thePP ) {
  
  //  int pythiaId = thePP -> pythiaID() ;
  int pythiaId = getPythia8ID(thePP);
  //  int pdgId = thePP -> pid().pid();
  
  //  if (pythiaId!=pdgId)
  //  verbose() << "pythiaID: " << pythiaId << " pdgID " << pdgId << " PLEASE CHECK " << endmsg;

  double pwidth , lifetime ;
  if ( 0 != pythiaId ) {
    if ( 0 == thePP -> lifetime() ) pwidth = 0. ;
    else pwidth = ( Gaudi::Units::hbarc / ( thePP -> lifetime() * Gaudi::Units::c_light ) ) ;
    if ( pwidth < ( 1.5e-6 * Gaudi::Units::GeV ) ) pwidth = 0. ;
    
    lifetime =  thePP -> lifetime() * Gaudi::Units::c_light ;
    if ( ( lifetime <= 1.e-4 * Gaudi::Units::mm ) || ( lifetime >= 1.e16 * Gaudi::Units::mm ) ) 
      lifetime = 0. ;
    
    m_pythia -> particleData.m0(pythiaId, thePP -> mass() / Gaudi::Units::GeV) ;

    // For Higgs, top, Z and W: update only masses
    if ( ( 6 != abs(pythiaId) ) && ( 23 != abs(pythiaId) ) && ( 24 != abs(pythiaId) ) 
         && ( 25 != abs(pythiaId) ) ) {
      m_pythia -> particleData.mWidth(pythiaId, pwidth / Gaudi::Units::GeV) ;
      //      m_pythia -> particleData.mMax(pdgId, thePP -> maxWidth() / Gaudi::Units::GeV) ;
      // m_pythia -> particleData.mMax(pythiaId, (thePP -> maxWidth() + thePP -> mass())/ Gaudi::Units::GeV) ;  
      if (pwidth!=0) m_pythia -> particleData.mMin(pythiaId, (thePP -> mass() - thePP -> maxWidth())/ Gaudi::Units::GeV) ; 
      else m_pythia -> particleData.mMin(pythiaId, 0);
      m_pythia -> particleData.mMax(pythiaId, 0);
      m_pythia -> particleData.tau0(pythiaId, lifetime / Gaudi::Units::mm) ;
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

  for (int i = 0 ; i <= 500 ; ++i ) {
    if ( 0 != m_pythia -> info.nAccepted(i) ) {
      m_xmlLogTool -> addCrossSection( processName(i),
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

string Pythia8Production::processName( int i ) {
  switch (i) {
  case 0 : return "All";
  case 101 : return "minimum bias";
  case 102 : return "A B -> A B elastic";
  case 103 : return "A B -> X B single diffractive";
  case 104 : return "A B -> A X single diffractive";
  case 105 : return "A B -> X X double diffractive";
  default : return "unknown process";
  }
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
  //  m_pythia->event.list();
  if (!m_pythia->forceHadronLevel()) return StatusCode::FAILURE ;
  // m_pythia->event.list();
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

    //    verbose() << "Status " << status << " --> " << (*p) -> status() << " for particle" << (*p)->pdg_id() << endmsg;
    
    //convert the pdgId to a correct value.
    //consistency between pdgId and pythiaId was not present in the past
    //with pythia8 it is the case, but one takes the old particletable it creates an issue.
    //this should be removed once the new particle table is used.
    int pythia8id = (*p) -> pdg_id();
    int pdgId = pythia8id;
    switch (abs(pythia8id)) {
    case 10221:
      (*p) -> set_pdg_id(pythia8id>0 ? 30221 : -30221);
      break;
    case 4124:
      (*p) -> set_pdg_id(pythia8id>0 ? 104122 : -104122);
      break;
    }

    //if( status==LHCb::HepMCEvent::StableInProdGen) verbose() << "Status " << status << " --> " << (*p) -> status() << " for particle" << (*p)->pdg_id() << "(pythia8Id " << pythia8id << ")" << endmsg;
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
