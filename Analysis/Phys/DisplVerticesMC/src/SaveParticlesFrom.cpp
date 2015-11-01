// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

//MC Check
#include "Event/MCParticle.h"
#include "Event/Particle.h"

// local
#include "SaveParticlesFrom.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : SaveParticlesFrom
//
// 2008-april-2 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SaveParticlesFrom )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  SaveParticlesFrom::SaveParticlesFrom( const std::string& name,
                                        ISvcLocator* pSvcLocator)
    : DaVinciAlgorithm ( name , pSvcLocator )
    , m_pLinker()
    , m_nEvents(0)
{
  declareProperty( "Preys", m_Preys );
  declareProperty( "Daughters", m_Dgts = true );
  declareProperty( "OnlyCharged", m_Charged = true );
  declareProperty( "SameVtx", m_Vtx = false );
}

//=============================================================================
// Destructor
//=============================================================================
SaveParticlesFrom::~SaveParticlesFrom() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode SaveParticlesFrom::initialize() {

  StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize the SaveParticlesFrom algorithm" << endmsg;

  //Retrieve MC linker tools
  m_pLinker = new Particle2MCLinker(this,Particle2MCMethod::Links,
                                    vector<string>(1,"") );

  //Sanity checks
  if( m_Preys.size()== 0 ){
    err() <<"Please give at least one particle ID to be able to "
          <<"select related particles"<<endmsg;
    return StatusCode::FAILURE;
  }

  info() << "--------------------------------------------------------"<<endmsg;
  info() << "SaveParticlesFrom will select ";
  for( vector<string>::const_iterator i = m_Preys.begin();
       i != m_Preys.end(); i++ ){
    const ParticleProperty* Prey = ppSvc()->find( *i );
    if ( !Prey ) { //
      err() << "Cannot find particle property for " << *i << endmsg ;
      return StatusCode::FAILURE;
    }
    //int id = Prey->pdgID(); //??? Does not work !
    int id = Prey->pythiaID();
    m_PreyIDs.push_back( id );
    info()<< *i <<" ("<< id <<"), ";
  }
  if(m_Dgts){
    info()<<"and their ";
    if(m_Charged) info()<<"charged ";
    info()<<"daughters";
    if(m_Vtx) info()<<" that were produced at the mother prey's decay vertex";
  }
  info()<<"."<<endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SaveParticlesFrom::execute() {

  //See v1r0 for the indirect method.
  ++m_nEvents;
  debug() << "==> Execute the SaveParticlesFrom algorithm, event "
          << m_nEvents << endmsg;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.


  //Get the MC Particles
  MCParticles* mcparts = get<MCParticles>(MCParticleLocation::Default );
  if( !mcparts ){
    warning() << "Unable to find MC particles at '"
              << MCParticleLocation::Default << "'" << endmsg;
    return StatusCode::FAILURE;
  }
  debug() << "There are " <<  mcparts->size() << " MC particles" << endmsg;

  //Get Particles from TES
  Particle::ConstVector preys = this->i_particles();
  debug() << "There are " << preys.size() <<" parts in TES !" << endmsg;

  //Loop over all particles
  for( Particle::ConstVector::const_iterator i = preys.begin();
       i!= preys.end(); i++ ){
    const Particle *p = *i;
    const MCParticle *mcp = m_pLinker->firstMCP( p );
    if( NULL!=mcp ){
      debug() <<"Particle ( id "<< p->particleID().pid() <<", key "<< p->key()
              <<" ) associated to MC part ( id " << mcp->particleID().pid()
              <<", key "     << mcp->key() <<" )"<< endmsg; }
    else {
      debug() << "Particle ( id " << p->particleID().pid() <<", key "
              << p->key() <<" ) is not associated !"<< endmsg;
      continue;
    }
    if( IsaPrey( mcp ) ){
      this->markTree( p );
      setFilterPassed(true);
      debug()<<"Particle will be saved !"<<endmsg;
    }

    //Go on with daughters ?
    if( !m_Dgts ) continue;

    //Continue with charged ?
    if( m_Charged && p->particleID().threeCharge() == 0 ) continue;

    //Is it from a prey ?
    if( mcp->mother() == NULL ) continue;
    if( !IsItFromaPrey( mcp, mcp->mother()->endVertices().at(0)->position()))
      continue;

    //Save it !
    this->markTree( p );
    setFilterPassed(true);
    debug()<<"Particle will be saved !"<<endmsg;
  }

  //Save all particles in the Desktop to the TES
  // if( !(desktop()->saveDesktop()) )
  //  return Error("Unable to save Particles to the TES !");

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SaveParticlesFrom::finalize() {

  debug() << "==> Finalize SaveParticlesFrom" << endmsg;

  delete m_pLinker ;

  return DaVinciAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=============================================================================
//  Is the MCParticle one of the prey ?
//=============================================================================
bool SaveParticlesFrom::IsaPrey( const MCParticle * p ) {
  int pid = p->particleID().pid();
  for( vector<int>::const_iterator i = m_PreyIDs.begin();
       i != m_PreyIDs.end(); i++ ){
    if( pid == (*i) ) return true;
  }
  return false;
}

//=============================================================================
//  Is the MCParticle a descendant of the prey ?
//=============================================================================
bool SaveParticlesFrom::IsItFromaPrey( const LHCb::MCParticle * p,
                                       const Gaudi::XYZPoint &  mcpos ) {

  if( !p->mother() ) return false;
  const MCParticle * mother = p->mother();
  if( IsaPrey(mother) ) {

    if( m_Vtx ){
      //check that mcp originated from the neut decay vtx
      double dist = VertDistance( mcpos,
                                  mother->endVertices().at(0)->position() );
      if( dist < 0.08 ){ return true; } else { return false; }
    } else { return true; }
  } else {
    return IsItFromaPrey( mother, mcpos );
  }
}

//============================================================================
// Compute distance between two vertices
//============================================================================
double SaveParticlesFrom::VertDistance( const Gaudi::XYZPoint & v1,
                                        const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}
