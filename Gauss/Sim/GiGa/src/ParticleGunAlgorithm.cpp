
#include <string>
#include <vector>


#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartIF.h"

///
/// from LHCbEvent
#include "LHCbEvent/MCVertex.h"
#include "LHCbEvent/MCParticle.h"

///
///


///
/// Implementation of ParticleGunAlgorithm 
///
/// Author: Vanya Belyaev 
/// Date:   10 Aug 2000 
///


/// local 
#include "ParticleGunAlgorithm.h" 


// IMPORTANT: global declarations for factory.
static const  AlgFactory<ParticleGunAlgorithm>                               s_ParticleGunAlgorithmFactory;
const        IAlgFactory&                      ParticleGunAlgorithmFactory = s_ParticleGunAlgorithmFactory;


///
/// constructor 
/// 

ParticleGunAlgorithm::ParticleGunAlgorithm( const std::string& AlgorithmName  , 
					    ISvcLocator*       ServiceLocator )
  : Algorithm( AlgorithmName , ServiceLocator ) 
  ///
  , m_NameParticlePropertySvc          ( "ParticlePropertySvc" ) 
  , m_ParticlePropertySvc              ( 0 ) 
  /// 
  , m_NameOfOutputMCVertexContainer      ( "/Event/MC/Generator/MCVertices"  ) 
  , m_NameOfOutputMCParticleContainer    ( "/Event/MC/Generator/MCParticles" )
  ///
  , m_ParticleName                       ( "e+" ) 
  , m_ParticleProperty                   (  0   ) 
  , m_PrimaryVertexPosition              ()  
  , m_AuxVertexPosition                  () 
  , m_ParticleMomentum                   () 
  , m_AuxParticleMomentum                () 
  ///
{
  ///
  declareProperty( "NameOfParticlePropertyService"    , m_NameParticlePropertySvc         ) ; 
  ///
  declareProperty( "NameOfOutputMCVertexContainer"    , m_NameOfOutputMCVertexContainer   ) ; 
  declareProperty( "NameOfOutputMCParticleContainer"  , m_NameOfOutputMCParticleContainer ) ; 
  ///
  declareProperty( "ParticleName"                     , m_ParticleName                    ) ; 
  declareProperty( "PrimaryVertex"                    , m_AuxVertexPosition               ) ; 
  declareProperty( "ParticleMomentum"                 , m_AuxParticleMomentum             ) ; 
  ///
};


///
/// destructor
///

ParticleGunAlgorithm::~ParticleGunAlgorithm()
{
  if( 0 != ppSvc() ){ ppSvc()->release() ; m_ParticlePropertySvc = 0 ; } 
}; 


StatusCode ParticleGunAlgorithm::initialize() 
{

  StatusCode status(StatusCode::SUCCESS) ; 

  MsgStream log( msgSvc() , name() + ".initialize()" ) ; 

  setProperties(); 

  /// 
  /// locate ParticlePropery Service
  ///

  status = serviceLocator()->getService( m_NameParticlePropertySvc            , 
					 IID_IParticlePropertySvc             , 
					 (IInterface*&) m_ParticlePropertySvc ); 
  if ( status.isFailure() || 0 == m_ParticlePropertySvc ) 
    { return status.isFailure() ? status : status = StatusCode::FAILURE ; }  
  ///
  if( 0 == ppSvc()             ) 
    {
      log << MSG::ERROR << "Particle Property Service with name="  
	  << m_NameParticlePropertySvc << "\tcould not be located!" << endreq; 
      return StatusCode::FAILURE; 
    }
  ///
    
  ///
  /// check for particle name 
  ///    
 
  m_ParticleProperty = ppSvc()->find( m_ParticleName ); 
  if( 0 == m_ParticleProperty ) 
    { 
      log << MSG::ERROR 
	  << "Could not find properties for particle, named\t" << m_ParticleName << endreq ; 
      return StatusCode::FAILURE; 
    } 
  
  ///
  /// Extract Primary Vertex
  /// 
  
  while( m_AuxVertexPosition.size() < 3 ){ m_AuxVertexPosition.push_back( 0 ) ; } 
  
  m_PrimaryVertexPosition = HepPoint3D( m_AuxVertexPosition[0] , 
					m_AuxVertexPosition[1] , 
					m_AuxVertexPosition[2] ) ;  
  ///
  /// Extract Particle Momentum 
  /// 
  while( m_AuxParticleMomentum.size() < 3 ) 
    { m_AuxParticleMomentum.insert( m_AuxParticleMomentum.begin() , 0 ) ; } 

  m_ParticleMomentum      = HepVector3D( m_AuxParticleMomentum[0] , 
					 m_AuxParticleMomentum[1] , 
					 m_AuxParticleMomentum[2] ) ;
  ///
  
  log << MSG::INFO 
      << "ParticleGunAlgorithm/" << name() << "\tis initialized for"
      << "\tParticle="   << m_ParticleName  
    ///
      << "\tMomentum=(" 
      << m_ParticleMomentum.x() / GeV << ","    
      << m_ParticleMomentum.y() / GeV << ","    
      << m_ParticleMomentum.z() / GeV << ") " << "[GeV/c]"
    ///
      << "\tVertex=(" 
      << m_PrimaryVertexPosition.x() / mm << ","    
      << m_PrimaryVertexPosition.y() / mm << ","    
      << m_PrimaryVertexPosition.z() / mm << ") " << "[mm]"
    ///
      << endreq; 
  ///
  log << MSG::INFO 
      << "\tOutput Containers:" 
      << "\t" << m_NameOfOutputMCVertexContainer 
      << "\t" << m_NameOfOutputMCParticleContainer << endreq;  
  ///
  ///
  ///
  
  return StatusCode::SUCCESS; 
  
  ///
};


///
///
///

StatusCode ParticleGunAlgorithm::finalize()
{
  if ( 0 != ppSvc() ) { ppSvc()->release() ; m_ParticlePropertySvc = 0 ;} 
  return StatusCode::SUCCESS; 
}; 

///
///
///

StatusCode ParticleGunAlgorithm::execute()
{

  MsgStream log( msgSvc() , name()+".execute()" );
  ///
  if( 0 == m_ParticleProperty ) { return StatusCode::FAILURE; } 
  ///
  
  {
    std::string dir( m_NameOfOutputMCVertexContainer ) ; 
    std::string::size_type p0 = dir.find_last_of('/') ;
    if(std::string::npos != p0 ) { dir.erase(p0) ; }  
    else { log << MSG::ERROR << " Wrong name of output MCVertex Container=" << dir << endreq; return StatusCode::FAILURE; } 
    
    std::string::size_type pos = 0 ; 
    while( std::string::npos != pos )
      {	
	pos = dir.find_first_of('/', ++pos ); 
	std::string dir1( dir , 0 , pos ); 
	SmartDataPtr<IDataDirectory> d1( eventDataService() , dir1 );  
	if( 0 != d1 ) { continue ; } 
	DataObject* Obj = new DataObject; 
	StatusCode   sc = eventDataService()->registerObject( dir1 , Obj ) ; 
	if( sc.isFailure() ) { log << MSG::ERROR <<" could not create directory " << dir1 << endreq; return sc; } 
      }
  }
  
  {
    std::string dir( m_NameOfOutputMCParticleContainer ) ; 
    std::string::size_type p0 = dir.find_last_of('/') ;
    if(std::string::npos != p0 ) { dir.erase(p0) ; }  
    else { log << MSG::ERROR << " Wrong name of output MCVertex Container=" << dir << endreq; return StatusCode::FAILURE; } 
    std::string::size_type pos = 0 ; 
    while( std::string::npos != pos ) 
      {
	pos = dir.find_first_of('/',++pos); 
	std::string dir1( dir , 0 , pos ); 
	SmartDataPtr<IDataDirectory> d1( eventDataService() , dir1 );  
	if( 0 != d1 ) { continue ; }  
	DataObject* Obj = new DataObject; 
	StatusCode   sc = eventDataService()->registerObject( dir1 , Obj ) ; 
	if( sc.isFailure() ) { log << MSG::ERROR <<" could not create directory " << dir1 << endreq; return sc; } 
      } 
  }
  
  ///
  ///
  ///  
  ObjectVector<MCVertex>* vertices = new ObjectVector<MCVertex> ; 
  {
    StatusCode sc = eventDataService()->registerObject( m_NameOfOutputMCVertexContainer , vertices) ; 
    if( sc.isFailure() ) 
      { 
	if( 0 != vertices ) { delete vertices ; vertices = 0 ; } /// delete the container 
	log << MSG::ERROR << "Unable to register the output container=" << m_NameOfOutputMCVertexContainer << endreq; 
	return sc ;
      }
  }
  ///
  ///
  ///  
  ObjectVector<MCParticle>* particles = new ObjectVector<MCParticle> ; 
  {
    StatusCode sc = eventDataService()->registerObject( m_NameOfOutputMCParticleContainer , particles ) ; 
    if( sc.isFailure() ) 
      { 
	if( 0 != particles ) { delete particles ; particles = 0 ; } /// delete the container 
      log << MSG::ERROR << "Unable to register the output container=" << m_NameOfOutputMCParticleContainer << endreq; 
      return sc ;
      }
  }
  ///



  /// (1) create vertex and particle 
  MCVertex*    vertex    = new MCVertex   ; 
  MCParticle*  particle  = new MCParticle ; 
  
  /// (2) add them into corresponding containers  and add links between containers 
  vertices ->push_back( vertex   ) ; 
  particles->push_back( particle ) ; 
  long linkToMCParticle = vertices  ->addLink( particles ->fullpath() , particles ); 
  long linkToMCVertex   = particles ->addLink( vertices  ->fullpath() , vertices  ); 
  
  
  /// (3a) set correct paramters for vertex
  vertex->setMotherMCParticle       ( (MCParticle*) 0         ); 
  vertex->setPosition               ( m_PrimaryVertexPosition ); 
  vertex->setTimeOfFlight           ( 0                       ); 
  vertex->removeDaughterMCParticles (); 
  vertex->addDaughterMCParticle     ( SmartRef<MCParticle>( vertex , linkToMCParticle , 0 , particle ) );   
  /// (3b) set correct parameters for particle 
  particle->removeDecayMCVertices   (); 
  double mass   = m_ParticleProperty->mass(); 
  double Energy = sqrt( m_ParticleMomentum.mag2() + mass * mass ); 
  particle->setFourMomentum( HepLorentzVector( m_ParticleMomentum , Energy ) ); 
  particle->setOriginMCVertex        ( SmartRef<MCVertex> ( particle , linkToMCVertex , 0 , vertex ) ); 
  particle->setParticleID( ParticleID( m_ParticleProperty->geantID() ) ); 
  particle->setOscillationFlag       ( false );                                                  


  return StatusCode::SUCCESS; 
  
}; 

















