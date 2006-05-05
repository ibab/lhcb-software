// $Id: MCParticleMaker.cpp,v 1.21 2006-05-05 17:48:23 pkoppenb Exp $
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
#include "Kernel/MatrixManip.h"
// local
#include "MCParticleMaker.h"
//#include "Linker/LinkerWithKey.h"
/*-----------------------------------------------------------------------------
 * Implementation file for class : MCParticleMaker
 *
 * Description: This tool fills the particle class with information from
 * MCParticles and places it in the Transient Event Store
 *
 * 03/10/2002 : Gerhard Raven
 *-----------------------------------------------------------------------------
 */
/// Returns false if is the MCPArticle is one of these stable particles (mu+/-,e+/-,pi+/-,K+/-,p/antip, gamma); true otherwise 
struct IsUnstable : std::unary_function<LHCb::MCParticle*,bool> {   
  bool operator()(const LHCb::MCParticle *p) const
  { int pid = p->particleID().abspid();
  return (pid!= 11 && pid!= 13 && pid!= 211 && pid!= 321 && pid!=2212 && pid!=22);
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

  std::vector< double > OneDefault(7,1.);
  std::vector< double > ZeroDefault(7,0.);
  
  // Declare properties
  declareProperty( "InputLocation", m_input = LHCb::MCParticleLocation::Default);
  declareProperty( "ParticleNames", m_particleNames );
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
  declareProperty( "IpErrorZ",   m_ipErrorZ = 0.01*mm );
  declareProperty( "SlopeError",  m_slopeError = 0.4*mrad );
  declareProperty( "MomError",    m_momError = 0.004 );  

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
// Finalisation. Check parameters
//=============================================================================
StatusCode MCParticleMaker::finalize() {
  debug()<<"MCParticleMaker is finalising"<<endreq;
  return GaudiTool::finalize();
}
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCParticleMaker::initialize() {
  
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc ;
  debug() << "==> MCParticleMaker:Initialising" << endmsg;

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  debug() << "Looking for Particle Property Service." << endmsg;
  
  m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc",true);
   
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

  m_pMCDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this);
  
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

  m_rho=ROOT::Math::SMatrixIdentity();

  m_rho(0,1)=m_rhoxy;
  m_rho(0,2)=m_rhoxz;
  m_rho(0,3)=m_rhoxtx;
  m_rho(0,4)=m_rhoxty;
  m_rho(0,5)=m_rhoxp;
  m_rho(1,2)=m_rhoyz;
  m_rho(1,3)=m_rhoytx;
  m_rho(1,4)=m_rhoyty;
  m_rho(1,5)=m_rhoyp;
  m_rho(2,3)=m_rhoztx;
  m_rho(2,4)=m_rhozty;
  m_rho(2,5)=m_rhozp;
  m_rho(3,4)=m_rhotxty;
  m_rho(3,5)=m_rhotxp;
  m_rho(4,5)=m_rhotyp;
  debug()<< "Correlation matrix rho "<<m_rho<<endmsg;
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode MCParticleMaker::makeParticles( LHCb::Particle::ConstVector & parts ) {

  debug() << "==> MCParticleMaker::makeParticles() is running." << endmsg;
  
  LHCb::MCParticle::Container* candidates = get<LHCb::MCParticle::Container>( m_input);
  //  LHCb::Particle::Container* Part_candidates = get<LHCb::Particle::Container>(LHCb::MCParticleMakerLocation::Default);
  if ( !candidates || (0 == candidates->size()) ){//|| !Part_candidates ) { 
    debug() << "    No MCParticles retrieved from" << m_input << " or Particle contained error" << endmsg;
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
    list.resize( candidates->size() );
    std::copy(candidates->begin(), candidates->end(), list.begin() );
  }

  debug() << "    List contains   = " << list.size() << " MCParticles" << endmsg;

  //  LinkerWithKey<LHCb::MCParticle,LHCb::Particle> myLink(evtSvc(), msgSvc(), Part_candidates);
  // LinkerWithKey<LHCb::Particle,LHCb::MCParticle> myLink(evtSvc(), msgSvc(), LHCb::MCParticleLocation::Default);
      
  std::vector<const LHCb::MCParticle*>::const_iterator icand;
  for(icand = list.begin(); icand != list.end(); icand++){

    debug() << " New candidate of pid " << (*icand)->particleID().pid() << endmsg ;

    if ( std::find(m_ids.begin(),m_ids.end(),(*icand)->particleID().pid() ) == m_ids.end()) continue;
    if ( m_onlyReconstructible && !reconstructible(**icand) )  continue;

    // covariance is in (x,y,z,px,py,pz,m) order
    debug()<< "Build Covariance Matrix "<<endmsg;
    Gaudi::SymMatrix7x7* covariance = NULL ;
    if ( m_onlyReconstructed) {
      const LHCb::Particle *measurement = reconstructed(**icand);
      if (measurement == 0)  continue;
      if ( m_useReconstructedCovariance ) {
        covariance = fetchCovariance(*measurement);
      }
    }
    if (NULL==covariance){
      debug()<<"Generate covariance"<<endmsg;
      covariance = generateCovariance((*icand)->momentum()) ;
    }
    debug() << "Got covariance matrix" << endmsg ;
    std::auto_ptr<LHCb::Particle> particle( new LHCb::Particle() );
    StatusCode sc = fillParticle( **icand, *particle, *covariance);
    if(sc.isFailure()) continue;
    parts.push_back(particle.release());
    debug() << "Done candidate of pid " << (*icand)->particleID().pid() << endmsg ;
    //          myLink.link((*icand),particle, 1.);
    //      myLink.link(particle,(*icand), 1.);
    debug()<< " Created Linker MCParticle-Particle"<<endmsg;
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

  double mass = mom.M() ;

  verbose() << "... and momentum " << mom << " mass = " << mass << endmsg ;

  // Definition of ScalingFactor and Bias (momentum and charge dependent)
  int sign=mc.particleID().pid()/mc.particleID().abspid();
  double  BIAS[7], SFCov[7];   // Vector of Biases and SF (momentum and charge dependent)

  for (int i=0;i<6;i++) {
    BIAS[i]  = sign*(m_BIASsC0[i]  +  m_BIASsC1[i]*mom.P()/GeV);// chosen parametrization bias= q*(C0 + C1 * p) x,y,tx,ty
    SFCov[i] = 1./(m_covSFsC0[i]   + m_covSFsC1[i]*mom.P()/GeV);// chosen parametrization SF= 1./(C0 + C1 * p)
  }
  BIAS[6]  = (m_BIASsC0[6]     +  m_BIASsC1[6]*mom.P()/GeV); // chosen parametrization bias=  (C0 + C1 * p) momenta
  SFCov[6] = 1./(m_covSFsC0[6] + m_covSFsC1[6]*mom.P()/GeV);

  verbose() << "... BIAS: " << BIAS[0] << " " << BIAS[1] << " "<< BIAS[2] << " "
            << BIAS[3] << " "<< BIAS[4] << " "<< BIAS[5] << " " << BIAS[6] <<endmsg ;
  verbose() << "... SFCov: " << SFCov[0] << " " << SFCov[1] << " " << SFCov[2] << " " 
            << SFCov[3] << " " << SFCov[4] << " " << SFCov[5] << " " << SFCov[6] <<endmsg ;

  const LHCb::MCVertex* vtx = mc.originVertex();
  Gaudi::XYZPoint pos = vtx->position();

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
  verbose() << "... MC truth Position: " << pos << endmsg ;

  Gaudi::SymMatrix7x7 covSF(cov);  
  if (m_smearParticle) {
    
    //    Gaudi::SymMatrix7x7 D(cov);      ///@todo commentout when diagonalization is done
    Gaudi::SymMatrix7x7 D(0);              /// todo @cancel when diagonalization is done
    for (int i=0;i<7;i++) D(i,i)=cov(i,i); /// todo @cancel when diagonalization is done
    
    //    Gaudi::Matrix7x7 U = diagonalize(&D);  /// @todo commentout when Diagonalize matrix
    Gaudi::Matrix7x7 U(0);  /// @todo cancel when Diagonalize matrix
    Gaudi::Vector7 deviates; //  x,y,z,sx,sy,p

    for (int i=0;i<7;++i) {
      for (int j=0;j<7;++j) {
        covSF(i,j) = cov(i,j)*SFCov[i]*SFCov[j];        
      }
      U(i,i)=1; /// @todo Remove when Diagonalization is implemented
    }

    verbose() << "... -> smearing by " << cov << endmsg ;
    
    for (int i=0;i<7;i++) {
      if (D(i,i)<0)  {
        error() << "Smearing Failed" << endmsg;
        return StatusCode::FAILURE;
      }
    }

    /*    
// Shuffling indexes////////////////////////////////////////////
    verbose() << "Inverting" << endmsg ;
    
    Gaudi::Matrix7x7 invU = U;
    if ( ! ROOT::Math::Inverter< 7 >::Dinv(invU) ) {
      err() << "Could no invert " << U << endmsg ;
      return StatusCode::FAILURE;
  	}
    debug()<< " Matrix U"<<endmsg;
    debug()<< " Matrix invU"<<endmsg;

    Gaudi::Matrux7x7 quadra(0);
    for (int i=0;i<7;++i)  quadra(i,i)=i;	
    
    
    Gaudi::Matrix7x7 pip =U*(quadra*invU);	
    int newindex[7];	
    
    for (int i=0;i<7;++i) {		
      newindex[i]=(int)(rint(pip(i,i))); 
      debug() << newindex[i] <<" ";
    }
    debug() << endreq;

///////////////////////////////////////////////////////
*/    
    int newindex[7];	
    for (int i=0;i<7;++i) newindex[i]=i; 

    // Random Generation according single or double Gaussian
    for (int i=0;i<7;++i) {      
      int ii=newindex[i];
      double ranGauss= m_ranGauss.shoot();
      if(m_dualGaussWeight[ii]!=0) {
        double ranFlat = m_ranFlat.shoot();
        if(ranFlat<m_dualGaussWeight[ii]) deviates(i) = m_dualGaussSF[ii]*ranGauss*sqrt(D(i,i));// double Gaussian smearing
      } else { 
        deviates(i) = ranGauss*sqrt(D(i,i));                        // single Gaussian smearing
      }
    }
    
    deviates = U*deviates;

    pos.SetX(  pos.x()  + deviates(0) + BIAS[0]*sqrt(covSF(0,0)));  //smears and add the BIAS
    pos.SetY(  pos.y()  + deviates(1) + BIAS[1]*sqrt(covSF(1,1)));
    pos.SetZ(  pos.z()  + deviates(2) + BIAS[2]*sqrt(covSF(2,2)));  
    //pos.SetZ(  pos.z() );                                           // z unchanged

    /*   Old Particle parametrization
         double sx = mom.px()/mom.pz() + deviates(3) + BIAS[3]*sqrt(covSF(3,3));
         double sy = mom.py()/mom.pz() + deviates(4) + BIAS[4]*sqrt(covSF(4,4));
         double  p = mom.Vect().R()  + deviates(5) + BIAS[5]*sqrt(covSF(5,5));
         double  m = mom.M();
         double pz = p / sqrt(1+sx*sx+sy*sy);
         mom = Gaudi::XYZTVector( sx*pz,sy*pz,pz,sqrt(m*m+p*p) );
    */
    double px = mom.px() + deviates(3) + BIAS[3]*sqrt(covSF(3,3));
    double py = mom.py() + deviates(4) + BIAS[4]*sqrt(covSF(4,4));
    double pz = mom.pz() + deviates(5) + BIAS[5]*sqrt(covSF(5,5));
    //    double  E = mom.E() + deviates(6) + BIAS[6]*sqrt(covSF(6,6); //todo substitute for gamma
    double  E = sqrt(px*px + py*py + pz*pz + mass*mass); 
    verbose() << "... new position -> " << pos << endmsg ;
    verbose() << "... MC momenta -> " << mom << endmsg ;
    mom = Gaudi::XYZTVector( px, py, pz, E);
    verbose() << "... new momenta -> " << mom << "  New mass = " << mom.M() << endmsg ;

  }
    
  // NOTE: must set position and momentum before covariance matrix
  //       as otherwise the conversions on the covariance cannot be done
  particle.setReferencePoint( pos );
  particle.setMomentum(mom);
  particle.setMeasuredMass(mom.M());
  particle.setMeasuredMassErr(0.0); // For the moment but already in constructor

  particle.setPosCovMatrix(covSF.Sub<Gaudi::SymMatrix3x3>(0,0));
  particle.setMomCovMatrix(covSF.Sub<Gaudi::SymMatrix4x4>(3,3));
  particle.setPosMomCovMatrix(covSF.Sub<Gaudi::Matrix4x3>(3,0));
    
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

  Gaudi::Matrix7x7 cTmp = Gaudi::Matrix7x7();
  cTmp.Place_at(p.posCovMatrix(),0,0);
  cTmp.Place_at(p.momCovMatrix(),3,3);
  cTmp.Place_at(p.posMomCovMatrix(), 0, 3);
  cTmp.Place_at(ROOT::Math::Transpose(p.posMomCovMatrix()), 3, 0); /// @todo Check that this actually works!
  Gaudi::SymMatrix7x7 *c = new Gaudi::SymMatrix7x7();
  *c = LHCb::MatrixManip::Symmetrize(cTmp);

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

  Gaudi::SymMatrix6x6 *c  = new Gaudi::SymMatrix6x6();    //(x,y,z,Sx,Sy,p)
  Gaudi::SymMatrix7x7 *cc = new Gaudi::SymMatrix7x7();    //(x,y,z,px,py,pz,E)
  // Set Covariance Matrix based on the following parametrization
  double pperp = p.Pt();
  if (pperp<0.01*MeV) pperp = 0.01*MeV; // to avoid junk

  double sip = m_ipErrorC0 + m_ipErrorC1/(pperp/GeV);
  double sZ = m_ipErrorZ ;

  double PsuE=p.P()/p.E();
  double Tx = p.Px()/p.Pz();
  double Ty = p.Py()/p.Pz();
  double Factor = sqrt(1.+Tx*Tx+Ty*Ty);
  
  debug() << "Generation covariance matrix based on " << p.P() << " " << sip << endmsg ;

  (*c)(0,0)= sip*sip/2;
  (*c)(1,1)= (*c)(0,0);
  (*c)(2,2)= sZ*sZ; 
  
  (*c)(3,3)= m_slopeError*m_slopeError;  // todo
  (*c)(4,4)= (*c)(3,3);
  (*c)(5,5)= m_momError*m_momError*p.P2();

  // account for correlation coefficient  
  for(int i=0;i<6;i++){
    for(int j=i;j<6;j++){
      (*c)(i,j)=m_rho(i,j)*sqrt((*c)(i,i)*(*c)(j,j));
    }
  }
  debug()<< "Covariance matrix x,y,z,Sx,Sy,p done" <<endmsg;
  ROOT::Math::SMatrix<double,7,6> *Jacob = new ROOT::Math::SMatrix<double,7,6>;
  //Gaudi::Matrix7x6 *Jacob  = new Gaudi::Matrix7x6(0);    //(x,y,z,Sx,Sy,p,E)
  
  (*Jacob)(0,0)=1;
  (*Jacob)(1,1)=1;
  (*Jacob)(2,2)=1;
  (*Jacob)(3,3)= p.P()*(1.+Ty*Ty)/pow(Factor,3);
  (*Jacob)(3,4)=-p.P()*Tx*Ty/pow(Factor,3);
  (*Jacob)(3,5)= Tx/Factor;
  (*Jacob)(4,3)=-p.P()*Tx*Ty/pow(Factor,3);
  (*Jacob)(4,4)= p.P()*(1.+Tx*Tx)/pow(Factor,3);
  (*Jacob)(4,5)= Ty/Factor;
  (*Jacob)(5,3)=-p.P()*Tx/pow(Factor,3);
  (*Jacob)(5,4)=-p.P()*Ty/pow(Factor,3);
  (*Jacob)(5,5)= 1./Factor;
  (*Jacob)(6,5)=PsuE;

  for(int i=0;i<7;i++){
    for(int j=0;j<6;j++){
      for(int k=0;k<6;k++){
        for(int l=i;l<7;l++){
          (*cc)(i,l) += (*Jacob)(i,j)*(*c)(j,k)*(*Jacob)(l,k);
        }
      }
    }
  }
  
  
  verbose() << " Covariance Matrix generated:" << endmsg;
  
  //  return cc;
  //////// Temporary !!!! TO DEFINE A DIAGONAL MATRIX /////
  Gaudi::SymMatrix7x7 *ccc = new Gaudi::SymMatrix7x7(0);    //(x,y,z,px,py,pz,E)
  for(int i=0;i<7;i++) (*ccc)(i,i)=(*cc)(i,i);
  return ccc;
  
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

