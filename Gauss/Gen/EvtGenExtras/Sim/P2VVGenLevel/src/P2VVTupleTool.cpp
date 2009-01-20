// $Id: P2VVTupleTool.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 

#include "Kernel/ParticleID.h"

#include "Event/HepMCEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"


#include <string>
#include <map>

#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "IP2VVAngleCalculator.h"
#include "P2VVTupleTool.h"
#include <boost/regex.hpp> 



//-----------------------------------------------------------------------------
// Implementation file for class : P2VVTupleTool
//
// 2008-06-11 : Thomas Blake
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( P2VVTupleTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
P2VVTupleTool::P2VVTupleTool( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ) //GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "NTupleName"   , m_name   = "P2VVTuple" ) ;
  declareProperty( "MotherID" , m_mother = 511 );
  declareProperty( "DaughterOne", m_daughterOne = 321 );
  declareProperty( "DaughterTwo", m_daughterTwo = 211 );
}
//=============================================================================
// Destructor
//=============================================================================
P2VVTupleTool::~P2VVTupleTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode P2VVTupleTool::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  debug() << "Initialising particle property svc" << endmsg;
  
  m_ppsvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc" , true ) ;
  
  m_candidates = 0;
  m_events = 0; 
  
  m_map.insert( std::make_pair(  -13, "muplus_" ) );
  m_map.insert( std::make_pair( 13, "muminus_" ) );
  
  std::string name = formatName( m_daughterOne );
  m_map.insert( std::make_pair(  m_daughterOne, name ) );
  name = formatName( m_daughterTwo );
  m_map.insert( std::make_pair( m_daughterTwo, name ) );
  
  std::map< int, std::string >::iterator it;
  
  for ( it = m_map.begin(); it != m_map.end() ; ++it ){
    info() << it->second << " ";
  }
  info() << endmsg;

  m_angletool = tool<IP2VVAngleCalculator>("P2VVAngleCalculator");
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode P2VVTupleTool::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  const LHCb::HepMCEvents* events =
    get<LHCb::HepMCEvents>( LHCb::HepMCEventLocation::Default );

    
  LHCb::HepMCEvents::const_iterator ievent;
  
  for ( ievent = events->begin(); ievent != events->end(); ++ievent ){
    HepMC::GenEvent::particle_const_iterator iparticle;
    
    for ( iparticle = (*ievent)->pGenEvt()->particles_begin();
          iparticle != (*ievent)->pGenEvt()->particles_end(); ++iparticle ){
      
      const HepMC::GenParticle* particle = (*iparticle);
      if ( m_mother == abs( particle->pdg_id() ) ) {
        m_candidates += fillCandidate( particle, (*ievent)->pGenEvt()->signal_process_id() );
      }
    }
    
  }
  m_events++;
  return StatusCode::SUCCESS;
}


bool P2VVTupleTool::isLongLivedParticle( const HepMC::GenParticle* particle ) const  {
  int pid = abs( particle->pdg_id() );
  return ( ( pid == 321 || pid == 211 || 
             pid == 11 || pid == 13 ) ? true : false );
}


void P2VVTupleTool::addDaughter( const HepMC::GenParticle* particle ) 
{
  int pid = particle->pdg_id();
  int absid = abs(pid);
  
  
  if ( absid == 13 ) {
    if ( pid == 13 ) m_muminus = particle;
    else m_muplus = particle;
  }
  else {
    if ( m_daughterOne != m_daughterTwo ) {
      if ( absid == m_daughterOne ) m_done = particle;
      else if ( absid == m_daughterTwo ) m_dtwo = particle;
    }
    else {
      if ( pid == m_daughterOne ) m_done = particle;
      else if ( pid == m_daughterTwo ) m_dtwo = particle;
    }
  }
  
  return;
}


int P2VVTupleTool::fillCandidate( const HepMC::GenParticle* parent, const int process_type ){
  HepMC::GenVertex *decayVertex = parent -> end_vertex();
  HepMC::GenVertex::particles_out_const_iterator idaughter, isub;
  
  m_muplus = 0;
  m_muminus = 0;
  m_done = 0;
  m_dtwo = 0;

  int pid = 0;

  info() << "Filling the candiate" << endmsg;

  if(!parent){
    info() << "The parent is null " << endmsg;
  }
  if(!decayVertex){
    info() << "The decayVertex is null "<< endmsg;
  }
  info() << "Parent PDG ID " << parent->pdg_id() << endmsg;

  for ( idaughter = decayVertex -> particles_out_const_begin() ;
        idaughter != decayVertex -> particles_out_const_end() ;	++idaughter ) {
    
    //pid = (*idaughter)->pdg_id();
    
    if ( abs(pid) == m_mother ) return fillCandidate( (*idaughter), process_type ) ;
    
    if ( isLongLivedParticle( *idaughter ) ) {
      addDaughter( *idaughter );
    }
    else {
      
      HepMC::GenVertex* daughterVertex = (*idaughter) -> end_vertex();
      
      if (!daughterVertex) continue;

      for ( isub = daughterVertex -> particles_out_const_begin() ;
            isub != daughterVertex -> particles_out_const_end() ;	++isub ) {
        // pid = (*isub)->pdg_id();
        addDaughter( *isub );
      }
    } 
  }

  // Check that the particles have been correctly found.

  info() << "MuPlus  : " << ( m_muplus ? m_muplus->pdg_id() : 0 ) << " " 
          << "MuMinus : " << ( m_muminus? m_muminus->pdg_id() : 0 ) << " " 
          << "DaughterOne : " << ( m_done ? m_done->pdg_id() : 0 ) << " " 
          << "DaughterTwo : " << ( m_dtwo ? m_dtwo->pdg_id() : 0 ) << endmsg;
  
  
  if ( !m_muplus || !m_muminus || !m_done || ( m_daughterTwo && !m_dtwo ) ) return 0;
  
  info() << "Candidate found" << endmsg;
  
  // Create and fill the ntuple
  Tuple tuple = nTuple( m_name );
  tuple->column( "HardProcess" , process_type );
  
  fillParticle( tuple, m_muplus );
  fillParticle( tuple, m_muminus );
  fillParticle( tuple, m_done );
  fillParticle( tuple, m_dtwo );
  
  
  
  Gaudi::XYZPoint production( parent->production_vertex()->point3d() );
  Gaudi::XYZPoint decay( parent->end_vertex()->point3d() );
  
  tuple->column("BID", parent->pdg_id() );
  tuple->column("DaughterID", m_done->pdg_id());
  
  tuple->column("Production_X", production.X() );
  tuple->column("Production_Y", production.Y() );
  tuple->column("Production_Z", production.Z() );
  tuple->column("Decay_X", decay.X() );
  tuple->column("Decay_Y", decay.Y() );
  tuple->column("Decay_Z", decay.Z() );

  fillP2VVAngles( tuple, parent );
  
  
  
  StatusCode sc = tuple->write();
  
  if (!sc.isSuccess()) {
    Exception("Error writing to ntuple !");
  }

  return 1;
}

void P2VVTupleTool::fillP2VVAngles( Tuple& tuple, const HepMC::GenParticle* parent ) const
{
  Gaudi::LorentzVector muplus_vec( m_muplus->momentum() );
  Gaudi::LorentzVector muminus_vec( m_muminus->momentum() );
  Gaudi::LorentzVector one_vec( m_done->momentum() );
  Gaudi::LorentzVector parent_vec( parent->momentum() );
  
  bool isbbar = ( parent->pdg_id() < 0 ? 1 : 0 );
  
  double thetal, thetak, phi;

  if ( m_daughterTwo ) {
    
    Gaudi::LorentzVector two_vec( m_dtwo->momentum() );
    if ( isbbar ) {
      thetal = m_angletool->calculatePolarAngle( parent_vec, muminus_vec, muplus_vec );
      phi = m_angletool->calculatePlaneAngle( parent_vec, muminus_vec, muplus_vec, one_vec, two_vec );
    }
    else {
      thetal = m_angletool->calculatePolarAngle( parent_vec, muplus_vec, muminus_vec );
      phi = m_angletool->calculatePlaneAngle( parent_vec, muplus_vec, muminus_vec, one_vec, two_vec );
    }
    
    thetak = m_angletool->calculatePolarAngle( parent_vec, one_vec, two_vec );
    thetak = Gaudi::Units::pi - thetak;
    
    tuple->column("ThetaL",thetal);
    tuple->column("ThetaK",thetak);
    tuple->column("Phi",phi);
  }
  else {
    if ( isbbar ) {
      thetal = m_angletool->calculatePolarAngle( parent_vec, muminus_vec, muplus_vec );
    }
    else {
      thetal = m_angletool->calculatePolarAngle( parent_vec, muplus_vec, muminus_vec );
    }
    tuple->column("ThetaL",thetal ) ;
  }


  Gaudi::LorentzVector dimuon = muplus_vec + muminus_vec;
  
  tuple->column("qsquare", dimuon.M2() );
  
  return;
}


void P2VVTupleTool::fillParticle( Tuple& tuple, const HepMC::GenParticle* particle ) const{
  
  const int pid   = particle->pdg_id();
  const int absid = abs(pid);
  
  std::string name;
  
  if ( (absid != 13) && (m_daughterOne != m_daughterTwo) ){
    name = m_map.find( absid )->second;
  }
  else {
    name = m_map.find( pid )->second;
  }
  
  tuple->column( name + "Mass", particle->momentum().m() );
  tuple->column( name + "E", particle->momentum().e() );
  tuple->column( name + "Px", particle->momentum().px() );
  tuple->column( name + "Py", particle->momentum().py() );
  tuple->column( name + "Pz", particle->momentum().pz() );
  
  return; 
}


std::string  P2VVTupleTool::formatName( const int pid ) const {
//const std::string& stdname) {

  ParticleProperty* pp = m_ppsvc->findByStdHepID( pid );
  std::string stdname = pp->particle();
  if ( stdname.size() == 0 ) return "";
  
  char* expr = "([[:word:]])|^(~)|([~])|(\\*)|([+])|([-])|(.)";
  char* replace;
  
  if ( m_daughterOne == m_daughterTwo ) {
    replace = "(?1$1)(?2anti)(?3~)(?4st)(?5plus)(?6minus)(?7_)";
  } else {
    replace = "(?1$1)(?2anti)(?3~)(?4st)(?5)(?6)(?7_)";
  }
  std::string name;

  boost::regex expression( expr );

  name = boost::regex_replace( stdname, expression, replace, 
			       boost::match_default | boost::format_all );

  
  expression.assign(  "(^_+)|(_+$)|(_{2,})" );
  
  return boost::regex_replace( name, expression, "(?1)(?2)(?3_)", 
                               boost::match_default | boost::format_all ) + "_";
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode P2VVTupleTool::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  info() << "Found " << m_candidates << " in " << m_events << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//Gaudi::LorentzVector mom( gen->momentum() ) ;
//const HepMC::GenVertex* vert  = gen->production_vertex();
//Gaudi::XYZPoint point( vert->point3d() );

// prod + decay vertex position

/// always have muons, add vector of other duaghters ... 
/// add parent ID ... 

/// Need vertex of B ... Need primary vertex ... Need Jpsi ... and daughter vertex ... 


/// HepMC::GenEvent* method signal_process_vertex() ...
/// JPsi Phi ? 
