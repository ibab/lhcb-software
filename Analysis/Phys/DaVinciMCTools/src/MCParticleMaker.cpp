// $Id: MCParticleMaker.cpp,v 1.10 2005-11-22 10:06:52 pkoppenb Exp $
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

#include "MCTools/IMCDecayFinder.h"

// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

// local
#include "MCParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCParticleMaker
//
// Description: This tool fills the particle class with information from
// MCParticles and places it in the Transient Event Store
// 03/10/2002 : Gerhard Raven
//-----------------------------------------------------------------------------
/// Returns false if is the MCPArticle is one of these stable particles (mu+/-,e+/-,pi+/-,K+/-,p, gamma); true otherwise 
struct IsUnstable : std::unary_function<MCParticle*,bool> {   
  bool operator()(const MCParticle *p) const
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
  : GaudiTool ( type, name , parent ), 
    m_ppSvc(0), 
    m_pMCDecFinder(0)
{
  // Declaring implemented interface  
  declareInterface<IParticleMaker>(this);

  std::vector < double > OneDefault,ZeroDefault;
  for(int daino=0; daino<6; daino++){
    OneDefault.push_back(1.);
    ZeroDefault.push_back(0.);
  }
  
  // Declare properties
  declareProperty( "InputLocation", m_input = MCParticleLocation::Default);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "OnlyDecayProducts", m_onlyDecayProducts = false);
  declareProperty( "OnlyDecayProducts", m_onlyDecayProducts = false);
  // if true also m_onlyDecayProducts HAS to be true
  declareProperty( "OnlyStableDecayProducts", m_onlyStableDecayProducts = false);
  // @ todo: restore option OnlyReconstructable when method is implemented
  //  declareProperty( "OnlyReconstructable", m_onlyReconstructable = false );
  // @ todo: restore option OnlyReconstructed when method is implemented
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

  m_pMCDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this);
  if(!m_pMCDecFinder){
    fatal() << "Unable to retrieve MCDecayFinder tool" << endmsg;
    return StatusCode::FAILURE;
  }

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
StatusCode MCParticleMaker::makeParticles( ParticleVector & parts ) {
  
  debug() 
    << "==> MCParticleMaker::makeParticles() is running." 
    << endmsg;
  
  MCParticles* candidates = get<MCParticles>( m_input);
  if ( !candidates || (0 == candidates->size()) ) { 
    debug() << "    No MCParticles retrieved from" << m_input << endmsg;
    return StatusCode::SUCCESS;
  }
  
  debug() << "    Number of MCParticles retrieved   = " << candidates->size() << endmsg;

  std::vector<const MCParticle*> list;
  if( m_onlyDecayProducts) {
    const MCParticle* imc = 0;
    if( m_onlyStableDecayProducts) {
      while( m_pMCDecFinder -> findDecay (*candidates, imc) )  list = getFinalState(*imc);  
    } else {
      while( m_pMCDecFinder -> findDecay (*candidates, imc) ) list.push_back(imc);
    }
  } else {
    MCParticles::const_iterator icand;
    for(icand = candidates->begin(); icand != candidates->end(); icand++){
      list.push_back(*icand);    
    }
  }

      
  std::vector<const MCParticle*>::const_iterator icand;
  for(icand = list.begin(); icand != list.end(); icand++){
    if ( std::find(m_ids.begin(),m_ids.end(),(*icand)->particleID().pid() ) == m_ids.end()) continue;
    if ( m_onlyReconstructable && !reconstructable(**icand) )  continue;
    // covariance is in (x,y,z,sx,sy,p) order
    std::auto_ptr<HepSymMatrix> covariance(0);
    if ( m_onlyReconstructed) {
      const Particle *measurement = reconstructed(**icand);
      if (measurement == 0)  continue;
      if ( m_useReconstructedCovariance ) {
#ifdef WIN32
        covariance=std::auto_ptr<HepSymMatrix>(fetchCovariance(*measurement));
#else
        covariance.reset( fetchCovariance(*measurement) );
#endif
      }
    }
    if (covariance.get() == 0)
#ifdef WIN32
      covariance = std::auto_ptr<HepSymMatrix>( generateCovariance((*icand)->momentum()) );
#else
    covariance.reset( generateCovariance((*icand)->momentum()) );
#endif
    std::auto_ptr<Particle> particle( new Particle() );
    StatusCode sc = fillParticle( **icand, *particle, *covariance);
    if(sc.isFailure()) continue;
    parts.push_back(particle.release());
  }
  debug() << " ==> MCParticleMaker created " << parts.size() << " particle in the desktop " << endmsg;
  return StatusCode::SUCCESS;
    
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode 
MCParticleMaker::fillParticle( const MCParticle& mc,
                               Particle& particle,
                               const HepSymMatrix& cov)
{

  // Start filling the particle:     
  particle.setOrigin(&mc);
  particle.setParticleID( mc.particleID() );
  particle.setConfLevel( 1 );
  particle.setIsResonance(false);  // Already in constructor
  
  // Set pointOnTrackErr: take typical errors at vertex...
  HepLorentzVector mom = mc.momentum();

  // Definition of ScalingFactor and Bias (momentum and charge dependent)
  int sign=mc.particleID().pid()/mc.particleID().abspid();
  double  BIAS[6], SFCov[6];   // Vector of Biases and SF (momentum and charge dependent)

  for (int i=0;i<5;i++) {
    BIAS[i]  = sign*(m_BIASsC0[i]  +  m_BIASsC1[i]*mom.vect().mag()/GeV);// chosen parametrization bias= q*(C0 + C1 * p) x,y,tx,ty
    SFCov[i] = 1./(m_covSFsC0[i]   + m_covSFsC1[i]*mom.vect().mag()/GeV);// chosen parametrization SF= 1./(C0 + C1 * p)
  }
  BIAS[5]  = (m_BIASsC0[5]     +  m_BIASsC1[5]*mom.vect().mag()/GeV); // chosen parametrization bias=  (C0 + C1 * p) momenta
  SFCov[5] = 1./(m_covSFsC0[5] + m_covSFsC1[5]*mom.vect().mag()/GeV);


  const MCVertex* vtx = mc.originVertex();
  HepPoint3D pos = vtx->position();

  // Smearing at Point On Track   defined as minimum distance to the beam line
  if(m_smearATPoT){          debug()<<"Smear @ PoT "<<endmsg;   
  double sx = mom.px()/mom.pz();
  double sy = mom.py()/mom.pz();

  double zPoT = pos.z() - (pos.x()*sx + pos.y()*sy)/(pow(sx,2)+pow(sy,2));
  double xPoT = pos.x() + sx * (zPoT - pos.z());
  double yPoT = pos.y() + sy * (zPoT - pos.z());

  pos.setX(  xPoT );
  pos.setY(  yPoT );
  pos.setZ(  zPoT );
  }

  HepSymMatrix covSF(cov);  
  if (m_smearParticle) {
    HepSymMatrix D(cov);    
    HepMatrix U = diagonalize(&D);  // needed if correlation is present
    HepVector deviates(6,0); //  x,y,z,sx,sy,p

    for (int i=0;i<cov.num_row();++i) {
      for (int j=0;j<cov.num_col();++j) {
        covSF(1+i,1+j) = cov(1+i,1+j)*SFCov[i]*SFCov[j];
      }
    }
    debug()<<"covSF "<< covSF  <<endmsg;
    
    for (int i=1;i<7;i++) {
      if (D(i,i)<0)  {
        error() << "Smearing Failed" << endmsg;
        return StatusCode::FAILURE;
      }
    }


    // Shuffling indexes, needed to recall the original order of ScalingFactors
    HepMatrix quadra(6,6,0);
    for (int i=1;i<=6;++i)  quadra(i,i)=i;	
    
    int ck=0;
    HepMatrix invU=U.inverse(ck);
    if(ck!=0) return StatusCode::FAILURE;
    
    HepMatrix pip =U*(quadra*invU);	
    int newindex[6];	
    
    for (int i=1;i<=6;++i) {		
      newindex[i-1]=(int)(rint(pip(i,i))); 
      debug() << newindex[i-1] <<" ";
    }
    debug() << endmsg;

    
    // Random Generation according single or double Gaussian
    for (int i=1;i<=6;++i) {      
      int ii=newindex[i-1];
      double ranGauss= m_ranGauss.shoot();
      if(m_dualGaussWeight[ii-1]!=0) {
        double ranFlat = m_ranFlat.shoot();
        if(ranFlat<m_dualGaussWeight[ii-1]) deviates(i) = m_dualGaussSF[ii-1]*ranGauss*sqrt(D(i,i));// double Gaussian smearing
      } else { 
        deviates(i) = ranGauss*sqrt(D(i,i));// single Gaussian smearing
      }
    }
    
    deviates = U*deviates;

    pos.setX(  pos.x()  + deviates(1) + BIAS[0]*sqrt(covSF(1,1)));  //smears and add the BIAS
    pos.setY(  pos.y()  + deviates(2) + BIAS[1]*sqrt(covSF(2,2)));
    //  pos.setZ(  pos.z()  + deviates(3) + BIAS[2]*sqrt(covSF(3,3)));  
    pos.setZ(  pos.z() );                                           // z unchanged
    double sx = mom.px()/mom.pz() + deviates(4) + BIAS[3]*sqrt(covSF(4,4));
    double sy = mom.py()/mom.pz() + deviates(5) + BIAS[4]*sqrt(covSF(5,5));
    double  p = mom.vect().mag()  + deviates(6) + BIAS[5]*sqrt(covSF(6,6));
    double  m = mom.m();
    double pz = p / sqrt(1+sx*sx+sy*sy);
    mom = HepLorentzVector( sx*pz,sy*pz,pz,sqrt(m*m+p*p) );
  }
    
  // NOTE: must set position and momentum before covariance matrix
  //       as otherwise the conversions on the covariance cannot be done
  particle.setPointOnTrack( pos );
  particle.setMomentum(mom);
  particle.setMass(mom.m());
  particle.setMassErr(0.0); // For the moment but already in constructor

  particle.setPointOnTrackErr(covSF.sub(1,3));
  particle.setSlopesMomErr(covSF.sub(4,6));
  HepMatrix c(3,3);
  for (int i=0;i<c.num_row();++i) {
    for (int j=0;j<c.num_col();++j) {
      c(1+i,1+j) = covSF(1+i,4+j);
    }
  }
  particle.setPosSlopesCorr(c);
    
  return StatusCode::SUCCESS;
}

//=====================================================================
// reconstructable
//=====================================================================
bool 
MCParticleMaker::reconstructable(const MCParticle& icand) const
{
  /// @todo Use Olivier Callot's associator in reconstructable method
  return true;
}

//=====================================================================
// reconstructed
//=====================================================================
const Particle *
MCParticleMaker::reconstructed(const MCParticle& icand) const
{
  // @todo Use Olivier Callot's associator in reconstructed method
  return 0;
}

//=====================================================================
// fetchCovariance
//=====================================================================
HepSymMatrix *
MCParticleMaker::fetchCovariance(const Particle& p ) const
{
  HepSymMatrix *c = new HepSymMatrix(6);
  c->sub(1,p.pointOnTrackErr());
  c->sub(4,p.momentumErr());
  HepMatrix m = p.posSlopesCorr();
  for (int i=0;i<m.num_row();++i) {
    for (int j=0;j<m.num_col();++j) {
      (*c)(1+i,4+j) = m(1+i,1+j);
    }
  }
  return c;
}
//=====================================================================
// generateCovariance
//=====================================================================
HepSymMatrix *
MCParticleMaker::generateCovariance(const HepLorentzVector& p) const
{
  HepSymMatrix *c = new HepSymMatrix(6);
  // Set Covariance Matrix based on the following parametrization
  double sip = m_ipErrorC0 + m_ipErrorC1/(p.perp()/GeV); 
  (*c)(1,1) = sip*sip/2;
  (*c)(2,2) = (*c)(1,1);
  (*c)(3,3) = (*c)(1,1); 
  
  (*c)(4,4) = m_slopeError*m_slopeError;
  (*c)(5,5) = (*c)(4,4);
  (*c)(6,6) = m_momError*m_momError*p.vect().mag2();
  // account for correlation coefficient  
  (*c)(1,2) =m_rhoxy*sqrt((*c)(1,1)*(*c)(2,2));
  (*c)(1,3) =m_rhoxz*sqrt((*c)(1,1)*(*c)(3,3));
  (*c)(4,1)=m_rhoxtx*sqrt((*c)(1,1)*(*c)(4,4));
  (*c)(5,1)=m_rhoxty*sqrt((*c)(1,1)*(*c)(5,5));
  (*c)(6,1)=m_rhoxp*sqrt((*c)(1,1)*(*c)(6,6));

  (*c)(3,2)=m_rhoyz*sqrt((*c)(3,3)*(*c)(2,2));
  (*c)(4,2)=m_rhoytx*sqrt((*c)(2,2)*(*c)(4,4));
  (*c)(5,2)=m_rhoyty*sqrt((*c)(2,2)*(*c)(5,5));
  (*c)(6,2)=m_rhoyp*sqrt((*c)(2,2)*(*c)(6,6));

  (*c)(4,3)=m_rhoztx*sqrt((*c)(3,3)*(*c)(4,4));
  (*c)(5,3)=m_rhozty*sqrt((*c)(3,3)*(*c)(5,5));
  (*c)(6,3)=m_rhozp*sqrt((*c)(3,3)*(*c)(6,6));

  (*c)(5,4)=m_rhotxty*sqrt((*c)(4,4)*(*c)(5,5));
  (*c)(6,4)=m_rhotxp*sqrt((*c)(4,4)*(*c)(6,6));

  (*c)(6,5)=m_rhotyp*sqrt((*c)(5,5)*(*c)(6,6));
  debug()<< " Covariance Matrix generated" << endmsg;
  
  return c;
}

//=====================================================================
// getFinalState
//=====================================================================
/// method to retrieve the list of the STABLE DECAY PRODUCTS of the decay channel defined in JobOption
std::vector<const MCParticle*> MCParticleMaker::getFinalState(const MCParticle& m){
  typedef std::vector<const MCParticle*> PV;
  PV pv;
  pv.push_back(&m);
  for (;;) {
    PV::iterator i = find_if(pv.begin(),pv.end(),IsUnstable());
    if ( i == pv.end() ) break;
    typedef const SmartRefVector<MCVertex> SRVV;
    SRVV& vv = (*i)->endVertices(); // non zero by construction
    pv.erase(i);
    for (SRVV::const_iterator j=vv.begin();j!=vv.end();++j) {
      typedef const SmartRefVector<MCParticle> SRVP;
      SRVP& vp = (*j)->products();
      for (SRVP::const_iterator k=vp.begin();k!=vp.end();++k) {
        pv.push_back(*k);
      }
    }
  }
  return pv;
}
