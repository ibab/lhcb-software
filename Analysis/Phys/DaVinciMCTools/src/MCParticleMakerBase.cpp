// $Id: MCParticleMakerBase.cpp,v 1.1 2006-06-02 11:07:39 jpalac Exp $
// Include files
#include <memory>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmEngine.h"
#include "GaudiKernel/RndmGenerators.h"
#include "Event/Particle.h"
#include "LHCbMath/MatrixManip.h"
// local
#include "MCParticleMakerBase.h"

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
} ;
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
  
  return StatusCode::SUCCESS ;
} ;
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
  //  particle.setOrigin(&mc); /// @todo fill a relation table <Particle,MCParticle>
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

    double  m = mom.M();

    /*   Old Particle parametrization
         double sx = mom.px()/mom.pz() + deviates(3) + BIAS[3]*sqrt(covSF(3,3));
         double sy = mom.py()/mom.pz() + deviates(4) + BIAS[4]*sqrt(covSF(4,4));
         double  p = mom.Vect().R()  + deviates(5) + BIAS[5]*sqrt(covSF(5,5));
         double pz = p / sqrt(1+sx*sx+sy*sy);
         mom = Gaudi::XYZTVector( sx*pz,sy*pz,pz,sqrt(m*m+p*p) );
    */
    double px = mom.px() + deviates(3) + BIAS[3]*sqrt(covSF(3,3));
    double py = mom.py() + deviates(4) + BIAS[4]*sqrt(covSF(4,4));
    double pz = mom.pz() + deviates(5) + BIAS[5]*sqrt(covSF(5,5));
    //    double  E = mom.E() + deviates(6) + BIAS[6]*sqrt(covSF(6,6); //todo substitute for gamma
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
  Gaudi::SymMatrix7x7&        ccc )
{
  using namespace Gaudi::Units ;
  
  debug()<<"Generate covariance"<<endmsg;
  
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
  
  debug() << "Generation covariance matrix based on " << p.P() << " momenta and " << sip << " impact parameter" << endmsg ;

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
  debug() << " Covariance Matrix generated:" << endmsg;
  

  //////// Temporary !!!! TO DEFINE A DIAGONAL MATRIX ccc ///// otherwise return cc
  for(int i=0;i<7;i++) ccc(i,i)=(*cc)(i,i);
  delete Jacob;
  delete c;
  delete cc;
  
  return StatusCode::SUCCESS;
  
}

// ============================================================================
// The END 
// ============================================================================
