// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// local
#include "NeutralParticleMaker.h"

// Declaration of the Tool Factory
static const  ToolFactory<NeutralParticleMaker>          s_factory ;
const        IToolFactory& NeutralParticleMakerFactory = s_factory ; 

/** @class NeutralParticleMaker NeutralParticleMaker.cpp
 *  Produces Neutral Particles from Neutral ProtoParticles
 *
 * @author Frederic Machefert
 * @date 24/07/02/2002 
*/

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NeutralParticleMaker::NeutralParticleMaker( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent ), m_ppSvc(0), m_EDS(0){
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Clean data members
  m_particleNames.clear();
  m_ids.clear();
  m_confLevels.clear();
  
  // Declare properties
  declareProperty( "InputProtoP", m_input = ProtoParticleLocation::Neutrals);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "ConfLevelCuts", m_confLevels );
  
}
//=============================================================================
// Destructor
//=============================================================================
NeutralParticleMaker::~NeutralParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode NeutralParticleMaker::initialize() {
  MsgStream logbk(msgSvc(), name());
  logbk << MSG::DEBUG << "==> NeutralParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  logbk << MSG::DEBUG << "    Looking for Particle Property Service." << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }  
  
 
  // Particle Names and ID : Neutral and merged Pi0
  ParticleProperty* gammaProp = m_ppSvc->find( "gamma" );

  if ( 0 == gammaProp )   {
    logbk << MSG::ERROR << "Cannot retrieve properties for gamma"<<endreq;
    return StatusCode::FAILURE;
  }
  m_gammaID=gammaProp->jetsetID();

  ParticleProperty* pi0Prop = m_ppSvc->find( "pi0" );
  if ( 0 == pi0Prop )   {
    logbk << MSG::ERROR << "Cannot retrieve properties for pi0"<<endreq;
    return StatusCode::FAILURE;
  }
  m_pi0ID=pi0Prop->jetsetID();

  // Test the ParticleNames Vector 
  if (m_particleNames.size() == 0) {
    logbk << MSG::ERROR << " ParticleNames is empty. "  
        << "Please, initialize it in your job options file" <<  endreq;
    return StatusCode::FAILURE;
  }

  if(m_particleNames[0] != "All" ){
    
    // Test the Confidence Level Cuts Vector 
    if (m_confLevels.size() == 0) {
      logbk << MSG::ERROR << "PhysDesktopConfLevelCuts is empty. " 
          << "Please, initialize it in your job options file " << endreq;
      return StatusCode::FAILURE;
    }
    
    // Test if the the Confidence Level Cuts Vector and ParticleNames 
    // have the same size
    if (m_confLevels.size() != m_particleNames.size() ) {
      logbk << MSG::ERROR << "PhysDesktopConfLevelCuts size is  " 
          << "different from  PhysDesktopParticleNames" << endreq;
      return StatusCode::FAILURE;
    }

    std::vector<std::string>::const_iterator ipartsName;
    std::vector<double>::const_iterator iConfLevel=m_confLevels.begin();
    for ( ipartsName = m_particleNames.begin(); 
          ipartsName != m_particleNames.end(); ++ipartsName ) {
      ParticleProperty* partProp = m_ppSvc->find( *ipartsName );
      if ( 0 == partProp )   {
        logbk << MSG::ERROR << "Cannot retrieve properties for particle \"" 
	      << *ipartsName << "\" " << endreq;
        return StatusCode::FAILURE;
      }
      if( partProp->charge() == 0 ){
        std::pair<int,double> id(partProp->jetsetID(),(*iConfLevel++));
        m_ids.push_back(id);
      }
      logbk << MSG::DEBUG << " Particle Requested: Name = " << (*ipartsName) 
	    << " PID = " << partProp->jetsetID() << endreq;
    }
    
  }  //if(m_particleNames[0] != "All")

  logbk<< MSG::INFO <<"No Selection applied yet !\n"
       <<"all ProtoParticles are converted to Gamma Particle"<<endreq;

  logbk << MSG::INFO <<"Vertex position is at (0.,0.,0.)"<<endreq;

  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* NeutralParticleMaker::eventSvc() const
{
  return m_EDS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode NeutralParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  logbk( msgSvc(), name() );
  
  logbk << MSG::DEBUG << "==> NeutralParticleMaker::makeParticles() is running." 
	<< endreq;
  
  int protoID;
  double protoCL;
  
  // make Neutral particles:
  int nGammaParticles=0;   // Counter of Gamma particles created.
  
  SmartDataPtr<ProtoParticles> candidates ( eventSvc(),m_input);
  if ( !candidates ) {
    logbk << MSG::ERROR << "No ProtoParticle container found in "  
          << m_input << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( 0 == candidates->size() ) { 
    logbk << MSG::INFO << "No ProtoParticles retrieved from "  
          << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Logbk number of ProtoPartCandidates retrieved
  logbk << MSG::DEBUG << "Number of Neutral ProtoParticles retrieved  = "
        << candidates->size() << endreq;
  
  if( "All" == m_particleNames[0] ){
    
    logbk << MSG::DEBUG << "Making all particles " << endreq;
    
    ProtoParticles::const_iterator icand = 0;  // Iterator on ProtoParticles.
    for(icand = candidates->begin(); icand != candidates->end(); icand++){
      
      std::vector< std::pair<int,double> >::const_iterator infoDet;
      for (infoDet=(*icand)->pIDDetectors().begin();
           infoDet != (*icand)->pIDDetectors().end() ; ++infoDet) {
        logbk<< MSG::DEBUG << "Info Detector=" << (*infoDet).first << " "
             << (*infoDet).second<<endreq;
        //protoCL = (*icand)->detPIDvalue(ProtoParticle::CaloTrMatch);
      }
      
      // selection to apply should stand here !!!
      // No ProtoParticle CL -> All ProtoParticles are Gamma

      protoID=m_gammaID;
      logbk<<MSG::DEBUG<<"protoID="<<protoID<<endreq;

      protoCL=1.;      
      logbk<<MSG::DEBUG<<"protoCL="<<protoCL<<endreq;
      
      Particle* particle = new Particle();
      
      StatusCode sc = fillParticle( *icand, protoID, protoCL, particle);
      if( sc.isFailure() ) {
        delete particle;
      }
      else {  
        // Insert particle in particle vector.
        parts.push_back(particle);
        nGammaParticles++;
      }  
    }
  }
  
  else {
    if( m_ids.size() !=0 ) {
      
      // Loop over all ProtoParticles and fill Particle if the ProtoParticle 
      // satisfies the PID and corresponding CL cut  :

      ProtoParticles::iterator icand = 0;  // Iterator on ProtoParticles.
      for(icand = candidates->begin();icand != candidates->end();icand++){  

        // Iterator on requested PID's

        std::vector<std::pair<int,double> >::iterator iWantedPID; 
        for ( iWantedPID = m_ids.begin();
              iWantedPID != m_ids.end(); 
              ++iWantedPID ) {

          std::vector< std::pair<int,double> >::const_iterator infoDet;
          for (infoDet=(*icand)->pIDDetectors().begin();
               infoDet != (*icand)->pIDDetectors().end() ; ++infoDet) {
            logbk<< MSG::DEBUG << "Info Detector=" << (*infoDet).first << " "
                 << (*infoDet).second<<endreq;
            //protoCL = (*icand)->detPIDvalue(ProtoParticle::CaloTrMatch);
          }

          // No ProtoParticle CL -> All ProtoParticles are Gamma
          protoID=m_gammaID;
          logbk<<MSG::DEBUG<<"protoID="<<protoID<<endreq;
            
          protoCL=1.;
          logbk<<MSG::DEBUG<<"protoCL="<<protoCL<<endreq;
            
          if ( protoID  == (*iWantedPID).first && 
               protoCL >= (*iWantedPID).second  ){ 
            // We have a good candidate to this iWantedPID:
            Particle* particle = new Particle();
            StatusCode sc = fillParticle(*icand, protoID, protoCL, particle);
            if( sc.isFailure() ) {
              delete particle;
            }
            else {  
              // Insert particle in particle vector.
              parts.push_back(particle);
              nGammaParticles++;
            }
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
  
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode NeutralParticleMaker::fillParticle( const ProtoParticle* protoCand,
                                               int protoID, double protoCL,
                                               Particle* particle ) {
  
  MsgStream  logbk( msgSvc(), name() );

  // Start filling the particle:     
  particle->setParticleID( protoID );
  particle->setConfLevel( protoCL );
  // Get mass from Particle Property Service to calculate the 4-momentum
  ParticleProperty* partProp = m_ppSvc->findByStdHepID( protoID );     
  double mass = (*partProp).mass();
  particle->setMass(mass);
  particle->setMassErr(0.0); // For the moment but already in constructor
  particle->setIsResonance(false);  // Already in constructor

  if( protoCand->calo().size() != 1 ) {
    logbk<<MSG::DEBUG<<"ProtoParticle has "<<protoCand->calo().size()
         <<" calohypo !"<<endreq;
    return StatusCode::FAILURE;
  }

  SmartRef<CaloHypo> hypo=*(protoCand->calo().begin());

  // set position: will need to set position at same place momentum is
  // given
  HepPoint3D position( hypo->position()->x(),
                       hypo->position()->y(),
                       hypo->position()->z() ) ;
  particle->setPointOnTrack( position );
  

  logbk << MSG::DEBUG << "position = " << position << endreq;

  // set four momentum
  HepLorentzVector quadriMomentum;
  quadriMomentum.setPx( hypo->momentum()->momentum().x() );
  quadriMomentum.setPy( hypo->momentum()->momentum().y() );
  quadriMomentum.setPz( hypo->momentum()->momentum().z() );
  quadriMomentum.setE( hypo->momentum()->momentum().e() );
  particle->setMomentum( quadriMomentum );
  logbk << MSG::DEBUG << "momentum = " << quadriMomentum << endreq;
  
  ////////////////////////////////////////////////////////////////////////
  // Retrieve position covariance matrix and set particle error matrices:
  // but here is at calorimeter while momentum is at z = 0

  const HepSymMatrix& posCov = hypo->position()->covariance();
    
  // Set pointOnTrackErr: (Error on x and y. No error on z!)
  HepSymMatrix pointOnTrackErr(3, 0);
  pointOnTrackErr(1,1) = posCov(CaloPosition::X,CaloPosition::X);
  pointOnTrackErr(1,3) = 0.0;
  pointOnTrackErr(2,1) = posCov(CaloPosition::Y,CaloPosition::X);
  pointOnTrackErr(2,2) = posCov(CaloPosition::Y,CaloPosition::Y);
  pointOnTrackErr(3,1) = 0.0;
  pointOnTrackErr(3,2) = 0.0;
  pointOnTrackErr(3,3) = 0.0;
  particle->setPointOnTrackErr(pointOnTrackErr);

  logbk << MSG::DEBUG << "pointOnTrackErr"           << endreq;
  logbk << MSG::DEBUG << particle->pointOnTrackErr()  << endreq;
  
  // Set Momentum Error:
  particle->setMomentumErr( hypo->momentum()->covariance() );
  logbk << MSG::DEBUG << "part momErr"           << endreq;
  logbk << MSG::DEBUG << particle->momentumErr() << endreq;
  
  // Set position-momentum correlation matrix. 
  // Position X Momentum correlation matrix also automatically set.
  // No correlation with Z
  HepMatrix posMomCorr(4, 4, 0);
  posMomCorr(4,1) = posCov(CaloPosition::E,CaloPosition::X);
  posMomCorr(4,2) = posCov(CaloPosition::E,CaloPosition::Y);
  posMomCorr(1,4) = posCov(CaloPosition::X,CaloPosition::E);
  posMomCorr(2,4) = posCov(CaloPosition::Y,CaloPosition::E);
  particle->setPosMomCorr(posMomCorr);
  
  logbk << MSG::DEBUG << "posMom correlation"   << endreq;
  logbk << MSG::DEBUG << particle->posMomCorr() << endreq;

  ///////////////////////////////////////////////////////////////////////////
  
  particle->setOrigin(protoCand);
  return StatusCode::SUCCESS;
} 
  
//==========================================================================
//  Finalize
//==========================================================================
StatusCode NeutralParticleMaker::finalize() {
    
  MsgStream logbk(msgSvc(), name());
  logbk << MSG::DEBUG << "==> NeutralParticleMaker::finalizing" << endreq;
  
  return StatusCode::SUCCESS;
}
//=========================================================================
