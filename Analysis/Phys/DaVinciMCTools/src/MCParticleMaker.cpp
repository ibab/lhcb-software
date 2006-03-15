// $Id: MCParticleMaker.cpp,v 1.14 2006-03-15 13:37:22 pkoppenb Exp $
// Include files 

#include <memory>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ParticleProperty.h"
#include "Event/Particle.h"

#include "Kernel/IMCDecayFinder.h"


// local
#include "MCParticleMaker.h"

/*-----------------------------------------------------------------------------
 * Implementation file for class : MCParticleMaker
 *
 * Description: This tool fills the particle class with information from
 * MCParticles and places it in the Transient Event Store
 *
 * 03/10/2002 : Gerhard Raven
 *-----------------------------------------------------------------------------
 */
/// Returns false if is the MCPArticle is one of these stable particles (mu+/-,e+/-,pi+/-,K+/-,p, gamma); true otherwise 
struct IsUnstable : std::unary_function<LHCb::MCParticle*,bool> {   
  bool operator()(const LHCb::MCParticle *p) const
  { int pid = p->particleID().pid();
  return (pid!= 11 && pid!= 13 && pid!= 211 && pid!= 321 && pid!=2212 && 
          pid!=-11 && pid!=-13 && pid!=-211 && pid!=-321 && pid!=22);
  }
};


// Declaration of the Tool Factory
static const  ToolFactory<MCParticleMaker>          s_factory ;
const        IToolFactory& MCParticleMakerFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleMaker::MCParticleMaker( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  ,m_onlyReconstructible(false)
  ,m_onlyReconstructed(false)
  ,m_ppSvc(0) 
  ,m_pMCDecFinder(0)
{
  // Declaring implemented interface  
  declareInterface<IParticleMaker>(this);

  std::vector < double > OneDefault,ZeroDefault;
  for(int daino=0; daino<6; daino++){
    OneDefault.push_back(1.);
    ZeroDefault.push_back(0.);
  }
  
  // Declare properties
  declareProperty( "InputLocation", m_input = LHCb::MCParticleLocation::Default);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "OnlyDecayProducts", m_onlyDecayProducts = false);
  declareProperty( "OnlyDecayProducts", m_onlyDecayProducts = false);
  // if true also m_onlyDecayProducts HAS to be true
  declareProperty( "OnlyStableDecayProducts", m_onlyStableDecayProducts = false);
  /// @todo: restore option OnlyReconstructible when method is implemented
  //  declareProperty( "OnlyReconstructible", m_onlyReconstructible = false );
  /// @todo: restore option OnlyReconstructed when method is implemented
  //  declareProperty( "OnlyReconstructed",   m_onlyReconstructed   = false );
  declareProperty( "UseReconstructedCovariance", m_useReconstructedCovariance = false );
  declareProperty( "SmearParticle",   m_smearParticle = true );
  declareProperty( "IpErrorC0",   m_ipErrorC0 = 0.0173*mm );  
  declareProperty( "IpErrorC1",   m_ipErrorC1 = 0.0265*mm );
  declareProperty( "SlopeError",  m_slopeError = 0.4*mrad ); //K*K* 0.20-0.35 mrad
  declareProperty( "MomError",    m_momError = 0.004 );  // K*K* 0.0025-0.003

  // if true smears the MC truth information at PointOnTrack (minimum distance from beam line) else smears  at origin vertex 
  declareProperty( "SmearATPoT", m_smearATPoT = false);
  declareProperty( "rhoxy",   m_rhoxy = 0.0 ); 
  declareProperty( "rhoxz",   m_rhoxz = 0.0 );  
  declareProperty( "rhoyz",   m_rhoyz = 0.0 );  
  declareProperty( "rhoxtx",   m_rhoxtx = -0.95 );   
  declareProperty( "rhoxty",   m_rhoxty = 0.0 );   
  declareProperty( "rhoxp",   m_rhoxp = 0.0 );
  declareProperty( "rhoytx",   m_rhoytx = 0.0 );   
  declareProperty( "rhoyty",   m_rhoyty = -0.95 );   
  declareProperty( "rhoyp",   m_rhoyp = 0.0 );
  declareProperty( "rhoztx",   m_rhoztx = 0.0 );   
  declareProperty( "rhozty",   m_rhozty = 0.0 );   
  declareProperty( "rhozp",   m_rhozp = 0.0 );
  declareProperty( "rhotxty",   m_rhotxty = 0.0 );   
  declareProperty( "rhotxp",   m_rhotxp = 0.0 );   
  declareProperty( "rhotyp",   m_rhotyp = 0.0 );   
  declareProperty( "dualGaussSF",    m_dualGaussSF=OneDefault);   // SF second Gaussian's sigma
  declareProperty( "dualGaussW", m_dualGaussWeight=ZeroDefault ); // relative weight second Gaussian
  declareProperty( "ScaleFactorCovarianceC0", m_covSFsC0=OneDefault );// rescaling factor of covariance matrix 
  declareProperty( "ScaleFactorCovarianceC1", m_covSFsC1=ZeroDefault ); //rescaling factor of covariance matrix momentum dependent
  declareProperty( "MeasurementBiasC0",       m_BIASsC0=ZeroDefault );// biases in cov units
  declareProperty( "MeasurementBiasC1",       m_BIASsC1=ZeroDefault );// biases in cov units momentum dependent (GeV)
}
//=============================================================================
// Destructor
//=============================================================================
MCParticleMaker::~MCParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCParticleMaker::initialize() {
  
  debug() << "==> MCParticleMaker:Initialising" << endmsg;

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  debug() << "Looking for Particle Property Service." << endmsg;
  
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc",true);
  if( !m_ppSvc ) {
    fatal() << "Unable to locate Particle Property Service"<< endmsg;
    return StatusCode::FAILURE ;
  }  
   
  IRndmGenSvc* r = svc<IRndmGenSvc>("RndmGenSvc",true);
  if( !r || m_ranGauss.initialize(r,Rndm::Gauss(0,1)).isFailure()){
    fatal() << "Unable to locate RndmGenSvc Gauss distribution" << endmsg;
    return StatusCode::FAILURE;
  }  
  IRndmGenSvc* rr = svc<IRndmGenSvc>("RndmGenSvc",true);
  if( !rr || m_ranFlat.initialize(rr,Rndm::Flat(0,1)).isFailure()){
    fatal() << "Unable to locate RndmGenSvc Flat distribution" << endmsg;
    return StatusCode::FAILURE;
  }
  /// @todo Restore MCDecayFinder
  /*
  m_pMCDecFinder = tool<IMCDecayFinder("MCDecayFinder", this);
  if(!m_pMCDecFinder){
    fatal() << "Unable to retrieve MCDecayFinder tool" << endmsg;
    return StatusCode::FAILURE;
  }
  */
  // check for consistentcy of options
  if (m_useReconstructedCovariance && !m_onlyReconstructed ) {
    fatal() << "Instructed to use covariance matrix of"
            << " reconstructed protoParticle"
            << " but told to also make particles wich are not reconstructed "
            << " please fix you configuration " << endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_onlyDecayProducts && m_onlyStableDecayProducts ) {
    fatal() << "Instructed to use only Stable Decay Products"
            << " and NOT Decay Products. "
            << " please fix you configuration " << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_particleNames.size() == 0) {
    err() << " ParticleNames is empty. "  
          << "Please, initialize it in your job options file" <<  endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<std::string>::const_iterator iName;
  for ( iName = m_particleNames.begin(); 
        iName != m_particleNames.end(); ++iName ) {
    ParticleProperty* partProp = m_ppSvc->find( *iName );
    if ( 0 == partProp )   {
      err() << "Cannot retrieve properties for particle \"" 
            << *iName << "\" " << endmsg;
      return StatusCode::FAILURE;
    }
    m_ids.push_back(partProp->jetsetID());
    // Print Debug message:
    debug() << " Particle Requested: Name = " << (*iName) 
            << " PID = " << partProp->jetsetID() << endmsg;
  }
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleMaker::makeParticles( LHCb::Particle::ConstVector & parts ) {
  
  debug() 
    << "==> MCParticleMaker::makeParticles() is running." 
    << endmsg;
  
  LHCb::ConstMCParticles* candidates = get<LHCb::ConstMCParticles>( m_input);
  if ( !candidates || (0 == candidates->size()) ) { 
    debug() << "    No MCParticles retrieved from" << m_input << endmsg;
    return StatusCode::SUCCESS;
  }
  
  debug() << "    Number of MCParticles retrieved   = " << candidates->size() << endmsg;

  std::vector<const LHCb::MCParticle*> list;
  if( m_onlyDecayProducts) {
    const LHCb::MCParticle* imc = 0;
    if( m_onlyStableDecayProducts) {
      while( m_pMCDecFinder -> findDecay (*candidates, imc) )  list = getFinalState(*imc);  
    } else {
      while( m_pMCDecFinder -> findDecay (*candidates, imc) ) list.push_back(imc);
    }
  } else {
    LHCb::ConstMCParticles::const_iterator icand;
    for(icand = candidates->begin(); icand != candidates->end(); icand++){
      list.push_back(*icand);    
    }
  }

  debug() << "    List contains   = " << list.size() << " MCParticles" << endmsg;
      
  std::vector<const LHCb::MCParticle*>::const_iterator icand;
  for(icand = list.begin(); icand != list.end(); icand++){
    debug() << " New candidate of pid " << (*icand)->particleID().pid() << endmsg ;
    if ( std::find(m_ids.begin(),m_ids.end(),(*icand)->particleID().pid() ) == m_ids.end()) continue;
    if ( m_onlyReconstructible && !reconstructible(**icand) )  continue;
    // covariance is in (x,y,z,px,py,pz,m) order
    Gaudi::SymMatrix7x7* covariance = NULL ;
    if ( m_onlyReconstructed) {
      const LHCb::Particle *measurement = reconstructed(**icand);
      if (measurement == 0)  continue;
      if ( m_useReconstructedCovariance ) {
        covariance = fetchCovariance(*measurement);
      }
    }
    if (NULL==covariance){
      covariance = generateCovariance((*icand)->momentum()) ;
    }
    debug() << "Got covariance matrix" << endmsg ;
    std::auto_ptr<LHCb::Particle> particle( new LHCb::Particle() );
    StatusCode sc = fillParticle( **icand, *particle, *covariance);
    if(sc.isFailure()) continue;
    parts.push_back(particle.release());
    debug() << "Done candidate of pid " << (*icand)->particleID().pid() << endmsg ;
  }
  debug() << " ==> MCParticleMaker created " << parts.size() << " particle in the desktop " << endmsg;
  return StatusCode::SUCCESS;
    
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode 
MCParticleMaker::fillParticle( const LHCb::MCParticle& mc,
                               LHCb::Particle& particle,
                               const Gaudi::SymMatrix7x7& cov){

  debug() << "Filling Particle with PID " << mc.particleID().pid() << endmsg ;

  verbose() << "Matrix is " << cov << endmsg ;
  
  // Start filling the particle:
  //  particle.setOrigin(&mc); /// @todo fill a relation table <Particle,MCParticle>
  particle.setParticleID( mc.particleID() );
  particle.setConfLevel( 1 );
  
  // Set pointOnTrackErr: take typical errors at vertex...
  Gaudi::XYZTVector mom = mc.momentum();

  verbose() << "... and momentum " << mom << endmsg ;

  // Definition of ScalingFactor and Bias (momentum and charge dependent)
  int sign=mc.particleID().pid()/mc.particleID().abspid();
  double  BIAS[6], SFCov[6];   // Vector of Biases and SF (momentum and charge dependent)

  for (int i=0;i<5;i++) {
    BIAS[i]  = sign*(m_BIASsC0[i]  +  m_BIASsC1[i]*mom.Vect().R()/GeV);// chosen parametrization bias= q*(C0 + C1 * p) x,y,tx,ty
    SFCov[i] = 1./(m_covSFsC0[i]   + m_covSFsC1[i]*mom.Vect().R()/GeV);// chosen parametrization SF= 1./(C0 + C1 * p)
  }
  BIAS[5]  = (m_BIASsC0[5]     +  m_BIASsC1[5]*mom.Vect().R()/GeV); // chosen parametrization bias=  (C0 + C1 * p) momenta
  SFCov[5] = 1./(m_covSFsC0[5] + m_covSFsC1[5]*mom.Vect().R()/GeV);

  verbose() << "... BIAS: " << BIAS[0] << " " << BIAS[1] << " "<< BIAS[2] << " "
            << BIAS[3] << " "<< BIAS[4] << " "<< BIAS[5] << " " << endmsg ;
  verbose() << "... SFCov: " << SFCov[0] << " " << SFCov[1] << " " << SFCov[2] << " " 
            << SFCov[3] << " " << SFCov[4] << " " << SFCov[5] << " " << endmsg ;

  const LHCb::MCVertex* vtx = mc.originVertex();
  Gaudi::XYZPoint pos = vtx->position();

  verbose() << "... Position: " << pos << endmsg ;

  // Smearing at Point On Track   defined as minimum distance to the beam line
  if(m_smearATPoT){          
    verbose()<<"Smear @ PoT "<<endmsg;   
    double sx = mom.px()/mom.pz();
    double sy = mom.py()/mom.pz();

    double zPoT = pos.z() - (pos.x()*sx + pos.y()*sy)/(pow(sx,2)+pow(sy,2));
    double xPoT = pos.x() + sx * (zPoT - pos.z());
    double yPoT = pos.y() + sy * (zPoT - pos.z());
    
    pos.SetX( xPoT );
    pos.SetY( yPoT );
    pos.SetZ( zPoT );
  }
  verbose() << "... -> gets " << pos << endmsg ;

  Gaudi::SymMatrix7x7 covSF(cov);  
  if (m_smearParticle) {
    
    verbose() << "... -> smearing by " << cov << endmsg ;

    Gaudi::SymMatrix7x7 D(cov);    
    //    Gaudi::Matrix6x6 U = diagonalize(&D);  /// @todo Diagonalize matrix
    Gaudi::Matrix7x7 U = D;  /// @todo Diagonalize matrix
    Gaudi::Vector7 deviates; //  x,y,z,sx,sy,p

    for (int i=0;i<7;++i) {
      for (int j=0;j<7;++j) {
        covSF(i,j) = cov(i,j)*SFCov[i]*SFCov[j];
      }
    }
    verbose()<<"covSF "<< covSF  <<endmsg;
    
    for (int i=0;i<7;i++) {
      if (D(i,i)<0)  {
        error() << "Smearing Failed" << endmsg;
        return StatusCode::FAILURE;
      }
    }

    verbose() << "Inverting" << endmsg ;
    
    Gaudi::Matrix7x7 invU = U;
    if ( ! ROOT::Math::Inverter< 7 >::Dinv(invU) ) {
      err() << "Could no invert " << U << endmsg ;
      return StatusCode::FAILURE;
  	}

    
    // Random Generation according single or double Gaussian
    for (int i=0;i<7;++i) {      
      double ranGauss= m_ranGauss.shoot();
      if(m_dualGaussWeight[i]!=0) {
        double ranFlat = m_ranFlat.shoot();
        if(ranFlat<m_dualGaussWeight[i]) deviates(i) = m_dualGaussSF[i]*ranGauss*sqrt(D(i,i));// double Gaussian smearing
      } else { 
        deviates(i) = ranGauss*sqrt(D(i,i));// single Gaussian smearing
      }
    }
    
    deviates = U*deviates;

    pos.SetX(  pos.x()  + deviates(0) + BIAS[0]*sqrt(covSF(0,0)));  //smears and add the BIAS
    pos.SetY(  pos.y()  + deviates(1) + BIAS[1]*sqrt(covSF(1,1)));
    //  pos.setZ(  pos.z()  + deviates(3) + BIAS[2]*sqrt(covSF(3,3)));  
    pos.SetZ(  pos.z() );                                           // z unchanged
    double sx = mom.px()/mom.pz() + deviates(3) + BIAS[3]*sqrt(covSF(3,3));
    double sy = mom.py()/mom.pz() + deviates(4) + BIAS[4]*sqrt(covSF(4,4));
    double  p = mom.Vect().R()  + deviates(5) + BIAS[5]*sqrt(covSF(5,5));
    double  m = mom.M();
    double pz = p / sqrt(1+sx*sx+sy*sy);
    mom = Gaudi::XYZTVector( sx*pz,sy*pz,pz,sqrt(m*m+p*p) );
  }
    
  // NOTE: must set position and momentum before covariance matrix
  //       as otherwise the conversions on the covariance cannot be done
  particle.setReferencePoint( pos );
  particle.setMomentum(mom);
  particle.setMeasuredMass(mom.M());
  particle.setMeasuredMassErr(0.0); // For the moment but already in constructor

  particle.setPosCovMatrix(covSF.Sub<3,3>(0,0)); /// @todo This is wrong, correct 
  particle.setMomCovMatrix(covSF.Sub<4,4>(3,3));
  particle.setPosMomCovMatrix(covSF.Sub<4,3>(3,0));
    
  debug() << "Done Filling Particle with PID " << mc.particleID().pid() << endmsg ;
  return StatusCode::SUCCESS;
}

//=====================================================================
// reconstructible
//=====================================================================
bool 
MCParticleMaker::reconstructible(const LHCb::MCParticle& icand) const
{
  /// @todo Use Olivier Callot's associator in reconstructible method
  verbose() << "reconstructible?" << endmsg ;
  return (icand.momentum().rho()>0);
}

//=====================================================================
// reconstructed
//=====================================================================
const LHCb::Particle *
MCParticleMaker::reconstructed(const LHCb::MCParticle& icand) const
{
  /// @todo Use Olivier Callot's associator in reconstructed method
  verbose() << "reconstructed?" << endmsg ;
  if (icand.momentum().rho()>0) return NULL;
  else  return NULL;
}

//=====================================================================
// fetchCovariance
//=====================================================================
Gaudi::SymMatrix7x7 *
MCParticleMaker::fetchCovariance(const LHCb::Particle& p ) const{

  Gaudi::SymMatrix7x7 *c = new Gaudi::SymMatrix7x7();
  c->Place_at<3,3>(p.posCovMatrix(),0,0);
  c->Place_at<4,4>(p.momCovMatrix(),3,3);
  c->Place_at<4,3>(p.posMomCovMatrix(),0,3); /// @todo Check what happens with lower half

  return c;
}
//=====================================================================
// generateCovariance
//=====================================================================
Gaudi::SymMatrix7x7 *
MCParticleMaker::generateCovariance(const Gaudi::XYZTVector& p) const{

  ///  @todo Take care of 7th column
  ///  @todo Replace sx,sy,p parameterization by px,py,pz
  ///  @todo Check what happens with lower column

  Gaudi::SymMatrix7x7 *c = new Gaudi::SymMatrix7x7();
  // Set Covariance Matrix based on the following parametrization
  double pperp = p.Pt();
  if (pperp<0.01*MeV) pperp = 0.01*MeV; // to avoid junk

  double sip = m_ipErrorC0 + m_ipErrorC1/(pperp/GeV);

  verbose() << "Generation covariance matrix based on " << p << " " << sip << endmsg ;

  (*c)(0,0)= sip*sip/2;
  (*c)(1,1)= (*c)(0,0);
  (*c)(2,2)= (*c)(0,0); 
  
  (*c)(3,3)= m_slopeError*m_slopeError;
  (*c)(4,4)= (*c)(3,3);
  (*c)(5,5)= m_momError*m_momError*p.Vect().mag2();
  // account for correlation coefficient  
  (*c)(0,1)=m_rhoxy*sqrt((*c)(0,0)*(*c)(1,1));
  (*c)(0,2)=m_rhoxz*sqrt((*c)(0,0)*(*c)(2,2));
  (*c)(3,0)=m_rhoxtx*sqrt((*c)(0,0)*(*c)(3,3));
  (*c)(4,0)=m_rhoxty*sqrt((*c)(0,0)*(*c)(4,4));
  (*c)(5,0)=m_rhoxp*sqrt((*c)(0,0)*(*c)(5,5));

  (*c)(2,1)=m_rhoyz*sqrt((*c)(2,2)*(*c)(1,1));
  (*c)(3,1)=m_rhoytx*sqrt((*c)(1,1)*(*c)(3,3));
  (*c)(4,1)=m_rhoyty*sqrt((*c)(1,1)*(*c)(4,4));
  (*c)(5,1)=m_rhoyp*sqrt((*c)(1,1)*(*c)(5,5));

  (*c)(3,2)=m_rhoztx*sqrt((*c)(2,2)*(*c)(3,3));
  (*c)(4,2)=m_rhozty*sqrt((*c)(2,2)*(*c)(4,4));
  (*c)(5,2)=m_rhozp*sqrt((*c)(2,2)*(*c)(5,5));

  (*c)(4,3)=m_rhotxty*sqrt((*c)(3,3)*(*c)(4,4));
  (*c)(5,3)=m_rhotxp*sqrt((*c)(3,3)*(*c)(5,5));

  (*c)(5,4)=m_rhotyp*sqrt((*c)(4,4)*(*c)(5,5));
  verbose() << " Covariance Matrix generated:" << endmsg;
  verbose() << (*c) << endmsg ;
  
  return c;
}

//=====================================================================
// getFinalState
//=====================================================================
/// method to retrieve the list of the STABLE DECAY PRODUCTS of the decay channel defined in JobOption
std::vector<const LHCb::MCParticle*> MCParticleMaker::getFinalState(const LHCb::MCParticle& m){
  typedef std::vector<const LHCb::MCParticle*> PV;
  PV pv;
  pv.push_back(&m);
  for (;;) {
    PV::iterator i = find_if(pv.begin(),pv.end(),IsUnstable());
    if ( i == pv.end() ) break;
    typedef const SmartRefVector<LHCb::MCVertex> SRVV;
    SRVV& vv = (*i)->endVertices(); // non zero by construction
    pv.erase(i);
    for (SRVV::const_iterator j=vv.begin();j!=vv.end();++j) {
      typedef const SmartRefVector<LHCb::MCParticle> SRVP;
      SRVP& vp = (*j)->products();
      for (SRVP::const_iterator k=vp.begin();k!=vp.end();++k) {
        pv.push_back(*k);
      }
    }
  }
  return pv;
}
