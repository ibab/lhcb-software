// $Id$
// Include files
#include <memory>

// from Gaudi
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "Event/Particle.h"
// local
#include "Kernel/MCParticleMakerBase.h"

/*-----------------------------------------------------------------------------
 * Implementation file for class : MCParticleMaker
 *
 * Description: This tool fills the particle class with information from
 * MCParticles and places it in the Transient Event Store
 *
 * 03/10/2002 : Gerhard Raven
 * May 2006: Gabriele Balbi & Stefania Vecchi
 *-----------------------------------------------------------------------------
 */

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
MCParticleMakerBase::MCParticleMakerBase
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  //
  , m_smearParticle   ( true  ) 
  , m_smearATPoT      ( false )
  , m_ipErrorC0       ( 0.0173 * Gaudi::Units::mm   ) 
  , m_ipErrorC1       ( 0.0265 * Gaudi::Units::mm   )
  , m_ipErrorZ        ( 0.01   * Gaudi::Units::mm   )
  , m_slopeError      ( 0.4    * Gaudi::Units::mrad ) 
  , m_momError        ( 0.004   )
  //
  // no default value!
  , m_outputTable     ()  ///< no default value!
  //
  , m_rho             ( ROOT::Math::SMatrixIdentity () )
  //
  , m_covSFsC0        ( 7 , 1.0 ) 
  , m_covSFsC1        ( 7 , 0.0 ) 
  //
  , m_BIASsC0         ( 7 , 0.0 ) 
  , m_BIASsC1         ( 7 , 0.0 )
  //
  , m_dualGaussSF     ( 7 , 1.0 ) 
  , m_dualGaussWeight ( 7 , 0.0 ) 
  //
{
  // Declare properties
  declareProperty ( "SmearParticle" , m_smearParticle  ) ;
  declareProperty ( "SmearATPoT"    , m_smearATPoT     ) ;
  declareProperty ( "IpErrorC0"     , m_ipErrorC0      ) ;  
  declareProperty ( "IpErrorC1"     , m_ipErrorC1      ) ;
  declareProperty ( "IpErrorZ"      , m_ipErrorZ       ) ;
  declareProperty ( "SlopeError"    , m_slopeError     ) ;
  declareProperty ( "MomError"      , m_momError       ) ;
  declareProperty ( "OutputTable"   , m_outputTable    ) ;
  // if true smears the MC truth information at PointOnTrack (minimum distance from beam line) else smears  at origin vertex 
  declareProperty ( "rhoxy"   , m_rho ( _X  , _Y  ) =  0.0  ) ; 
  declareProperty ( "rhoxz"   , m_rho ( _X  , _Z  ) =  0.0  ) ;  
  declareProperty ( "rhoxtx"  , m_rho ( _X  , _TX ) = -0.95 ) ;   
  declareProperty ( "rhoxty"  , m_rho ( _X  , _TY ) =  0.0  ) ;   
  declareProperty ( "rhoxp"   , m_rho ( _X  , _P  ) =  0.0  ) ;
  declareProperty ( "rhoyz"   , m_rho ( _Y  , _Z  ) =  0.0  ) ;
  declareProperty ( "rhoytx"  , m_rho ( _Y  , _TX ) =  0.0  ) ;   
  declareProperty ( "rhoyty"  , m_rho ( _Y  , _TY ) = -0.95 ) ;   
  declareProperty ( "rhoyp"   , m_rho ( _Y  ,  _P ) =  0.0  ) ;
  declareProperty ( "rhoztx"  , m_rho ( _Z  , _TX ) =  0.0  ) ;   
  declareProperty ( "rhozty"  , m_rho ( _Z  , _TY ) =  0.0  ) ;   
  declareProperty ( "rhozp"   , m_rho ( _Z  , _P  ) =  0.0  ) ;
  declareProperty ( "rhotxty" , m_rho ( _TX , _TY ) =  0.0  ) ;   
  declareProperty ( "rhotxp"  , m_rho ( _TX , _P  ) =  0.0  ) ;   
  declareProperty ( "rhotyp"  , m_rho ( _TY , _P  ) =  0.0  ) ;
  //
   // rescaling factor of covariance matrix 
  declareProperty( "ScaleFactorCovarianceC0", m_covSFsC0        ) ;
  //rescaling factor of covariance matrix momentum dependent
  declareProperty( "ScaleFactorCovarianceC1", m_covSFsC1        ) ; 
  // biases in cov units
  declareProperty( "MeasurementBiasC0"      , m_BIASsC0         ) ;
  // biases in cov units momentum dependent (GeV)
  declareProperty( "MeasurementBiasC1"      , m_BIASsC1         ) ;
  // SF second Gaussian's sigma
  declareProperty( "dualGaussSF"            , m_dualGaussSF     ) ;
  // relative weight second Gaussian
  declareProperty( "dualGaussW"             , m_dualGaussWeight ) ; 
}
// ============================================================================
/// initializationof the tool 
// ============================================================================
StatusCode MCParticleMakerBase::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  IRndmGenSvc* rndm = svc<IRndmGenSvc>("RndmGenSvc",true);
  sc = m_ranGauss.initialize ( rndm , Rndm::Gauss ( 0 , 1 ) ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to initialize Gauss distribution" , sc ) ; }
  
  sc = m_ranFlat.initialize  ( rndm , Rndm::Flat  ( 0 , 1 ) ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to initialize Flat  distribution" , sc ) ; }

  if ( !m_smearParticle ) 
  { Warning ( "No smearing is requiested!" , StatusCode::SUCCESS ) ; }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// finalization of the tool 
// ============================================================================
StatusCode MCParticleMakerBase::finalize () { return GaudiTool::finalize() ; }
//=========================================================================
// fill particles
//========================================================================= 
StatusCode 
MCParticleMakerBase::fillParticle
( const Gaudi::LorentzVector& mc       ,
  const Gaudi::XYZPoint&      point    ,
  const LHCb::ParticleID&     pid      , 
  const Gaudi::SymMatrix7x7&  cov      , 
  LHCb::Particle&             particle ) 
{ 
  using namespace Gaudi::Units ;
  
  Gaudi::LorentzVector mom = mc    ;
  Gaudi::XYZPoint      pos = point ;
  
  debug() << "Filling Particle with PID " << pid.pid() << endmsg ;
  
  verbose() << "Matrix is " << cov << endmsg ;
  // Start filling the particle:
  particle.setParticleID ( pid );
  particle.setConfLevel  ( 1 );

  double mass = mom.M() ;
  
  verbose() << "... and momentum " << mom << " mass = " << mass << endmsg ;
  
  // Definition of ScalingFactor and Bias (momentum and charge dependent)
  int sign=pid.pid()/pid.abspid();
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
    // RANDOM Generation according single or double Gaussian
    int flagDG[7];
    for (int i=0;i<7;++i) {      
      if(m_dualGaussWeight[i]!=0) {
        double ranFlat = m_ranFlat.shoot();
        if(ranFlat<m_dualGaussWeight[i]) flagDG[i]=1;
      }
    }
  
    Gaudi::SymMatrix7x7 D(cov);    
    Gaudi::Vector7 deviates; //  x,y,z,px,py,pz,E

    debug()<< "test DG smearing. Covariance Before" <<D<<endmsg;
    
    for (int i=0;i<7;++i) {
      if (D(i,i)<0)  {
        return Error("Smearing Failed: diagonal matrix elements negative");
      }
      for (int j=0;j<7;++j) {
        covSF(i,j) = cov(i,j)*SFCov[i]*SFCov[j];        
        if(flagDG[i]==1) D(i,j) = D(i,j)*m_dualGaussSF[i]; // remember  D is a SymMatrix!!
      }
      if(flagDG[i]==1) D(i,i) = D(i,i)*m_dualGaussSF[i];
      deviates(i)=BIAS[i]*sqrt(covSF(i,i));  //mean value

    }
    debug()<< "test DG smearing. Covariance AFTER" <<D<<endmsg;

    StatusCode sc=correlatedRandomVectorGenerator(D,deviates);
    if(sc.isFailure()) return sc;
    pos.SetX(  pos.x()  + deviates(0) );  //pseudo measurements
    pos.SetY(  pos.y()  + deviates(1) );
    pos.SetZ(  pos.z()  + deviates(2) );  

    double  m = mom.M();
    double px = mom.px() + deviates(3);
    double py = mom.py() + deviates(4);
    double pz = mom.pz() + deviates(5);

    //    double  E = mom.E() + deviates(6); /// @todo substitute with something clever for neutrals
    double  E = sqrt(px*px + py*py + pz*pz + m*m); 
    verbose() << "... new position -> " << pos << endmsg ;
    verbose() << "... MC momenta -> " << mom << endmsg ;
    mom = Gaudi::XYZTVector( px, py, pz, E);
    verbose() << "... new momenta -> " << mom << endmsg ;
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
  
  debug() << "Done Filling Particle with PID " << pid.pid() << endmsg ;
  return StatusCode::SUCCESS;
}

//=====================================================================
// generateCovariance
//=====================================================================
StatusCode MCParticleMakerBase::generateCovariance
( const Gaudi::LorentzVector& p   ,
  Gaudi::SymMatrix7x7&        ccc)
{
  using namespace Gaudi::Units ;
  
  debug()<<"Generate covariance"<<endmsg;

  ///  Replaced x,y,z,sx,sy,p parameterization by x,y,z,px,py,pz,E
  
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
  
  debug() << "Generation covariance matrix based on " << p.P() << " momenta and " << sip << " impact parameter" << endmsg ;

  (*c)(0,0)= sip*sip/2;
  (*c)(1,1)= (*c)(0,0);
  (*c)(2,2)= sZ*sZ; 
  
  (*c)(3,3)= m_slopeError*m_slopeError;  
  (*c)(4,4)= (*c)(3,3);
  (*c)(5,5)= m_momError*m_momError*p.P2();

  // account for correlation coefficient  
  for(int i=0;i<6;i++){
    for(int j=i;j<6;j++){
      (*c)(i,j)=m_rho(i,j)*sqrt((*c)(i,i)*(*c)(j,j));
    }
  }
  debug()<< "Covariance matrix x,y,z,Sx,Sy,p done" <<endmsg;

  ROOT::Math::SMatrix<double,7,6> *Jacob = new ROOT::Math::SMatrix<double,7,6>; // derivative matrix
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
  debug() << " Covariance Matrix x,y,z,px,py,pz,E done" << endmsg;

  for(int i=0;i<7;i++) {
    for(int j=i;j<7;j++) {
      ccc(i,j)=(*cc)(i,j);
    }
  } 
  delete Jacob;
  delete c;
  delete cc;
  
  return StatusCode::SUCCESS;
  
}
//=====================================================================
// correlatedRandomVectorGenerator
//=====================================================================
StatusCode MCParticleMakerBase::correlatedRandomVectorGenerator
( const  Gaudi::SymMatrix7x7& cov, Gaudi::Vector7& vector ) {
  
  debug()<<"correlatedRandomVectorGenerator covariance"<<endmsg;

  ///  This method allows to generate a vector of random numbers with correlated components
  ///  This methods handles also the cases in which the number of independent components is less
  ///  than the matrix covariance order, that happen when the diagonalized covadiance matrix is NOT
  ///  definite positive.
  ///
  ///  Return StatusCode::FAILURE if covariance rank is zero OR if the covariance matrix has not diagonal 
  ///  positive elements
  int order = 7;
  Gaudi::SymMatrix7x7 c(cov);
  Gaudi::Matrix7x7 b;
  std::vector<int> swap(7,0);
  int index[7];
  for(int i=0;i<7;i++)index[i]=i;	
  
  int rank=0;
  bool loop=true;
  while(loop) {
    //find maximal diagonal element
    swap[rank]=rank;
    for(int i=rank+1; i<order; ++i){
      if(c(index[i],index[i])>c(index[swap[i]],index[swap[i]]))swap[rank]=i;
    }
    // swap elements
    if(swap[rank]!=rank){
      int tmp=index[rank];
      index[rank]=index[swap[rank]];
      index[swap[rank]] = tmp;
    }
    // check diagonal element value
    if (c(index[rank], index[rank]) < 1.0e-12) { // ????
      if (rank == 0) {
        return Error("NULL Covariance Matrix rank !! ");
      }
      // check remaining diagonal elements
      for (int i = rank; i < order; ++i) {
        if (c(index[i], index[i]) < -1.0e-11) {
          // there is at least one sufficiently negative diagonal element,
          // the covariance matrix is wrong
          return Error("Negative Covariance Matrix diagonal element !! ");
        }
      }
      
      // all remaining diagonal elements are close to zero,
      // we consider we have found the rank of the covariance matrix
      ++rank;
      loop = false;
    } else {
      // transform the matrix
      double ssqrt = sqrt(c(index[rank], index[rank]));
      b(rank, rank)= ssqrt;
      double inverse = 1 / ssqrt;
      for (int i = rank + 1; i < order; ++i) {
        double e = inverse * c(index[i], index[rank]);
        b(i, rank)= e;
        c(index[i], index[i]) = c(index[i], index[i]) - e * e;
        for (int j = rank + 1; j < i; ++j) {
          double f = b(j, rank);
          c(index[i], index[j]) = c(index[i], index[j]) - e * f;  // remember c is SymMatrix
        }
      }    
      // prepare next iteration
      if(++rank >= order)loop=false;
    } 
  }
  debug()<< " rank of covariance matrix " << rank << endmsg;

  // build the root matrix (max size)
  Gaudi::Matrix7x7 root;
  for (int i = 0; i < order; ++i) {
    for (int j = 0; j < rank; ++j) {
      root(swap[i], j)= b(i, j);
    }
  }

  Gaudi::Vector7 normalized; // oversized ---but no problem
  for (int i = 0; i < 7; ++i) normalized[i]=m_ranGauss.shoot();
 
  // compute correlated vector
  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < rank; ++j) {
      vector[i] += root(i, j) * normalized[j];
    }
  }
  debug()<< " Correlated Random Vector created" << vector << endmsg;
  return   StatusCode::SUCCESS;
}

// ============================================================================
// The END 
// ============================================================================
