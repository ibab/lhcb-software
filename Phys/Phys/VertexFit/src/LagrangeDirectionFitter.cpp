// $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// Include files from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"


// from CLHEP 
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Random/Random.h" 


// local
#include "LagrangeDirectionFitter.h"

//--------------------------------------------------------------------
// Implementation file for class : LagrangeDirectionFitter
//
//  05/08/2002 : Benoit VIAUD
//
//--------------------------------------------------------------------

// Declaration of the Tool Factory
static ToolFactory<LagrangeDirectionFitter> s_factory;
const IToolFactory& LagrangeDirectionFitterFactory = s_factory;

//==================================================================
// Standard Constructor, initializes variables
//==================================================================
LagrangeDirectionFitter::LagrangeDirectionFitter(const std::string& type, 
                     const std::string& name,
                     const IInterface* parent) 
    : AlgTool( type, name, parent ) {

  declareInterface<IDirectionFitter>(this);

}

//==================================================================
// Performs the constrained fit that forces the momentum of a Particle
// to point into another vertex.
//==================================================================
StatusCode LagrangeDirectionFitter::fitWithDirection(Particle& particle,Vertex& vertex, 
                            Particle& fitParticle,Vertex& fitVtx ) 
                                              

{

  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << " I begin to implement the Bd pointing fit " << endreq;
  log << MSG::DEBUG << "                                          " << endreq;

 



  // a Few variables to initialize
 
  double Energinit = particle.momentum().e();
  Vertex* decvert =  particle.endVertex(); 
  SmartRefVector<Particle> PartProd = decvert->products();

 

  ///// Step 1 : get the inputs //////////////////////////////////////////  
  ////////////////////////////////////////////////////////////////////////

  // Get the Bd reco momentum, decay vertex, and covariance matrix :
  
  HepLorentzVector pBi = particle.momentum();    

  double Sx = particle.slopeX();
  double Sy = particle.slopeY();
  double P  = particle.p();


  HepPoint3D vBi = particle.pointOnTrack();
  
   
  HepSymMatrix covBi(6,0); 

  HepSymMatrix pointErr = particle.pointOnTrackErr();
  HepSymMatrix SlpMomCorrErr = particle.slopesMomErr();
  HepMatrix PointSlpMomCorrErr = particle.posSlopesCorr();
  
  covBi(1,1) = pointErr(1,1);             // x-x
  covBi(2,1) = pointErr(2,1);             // y-x  
  covBi(2,2) = pointErr(2,2);             // y-y 
  

  covBi(3,1) = pointErr(3,1);             // z-x
  covBi(3,2) = pointErr(3,2);             // z-y
    
  covBi(3,3) = pointErr(3,3);             // z-z 

  covBi(1,4)=PointSlpMomCorrErr(1,1);   // x-sx
  covBi(1,5)=PointSlpMomCorrErr(2,1);   // x-sy
  covBi(1,6)=PointSlpMomCorrErr(3,1);   // x-p
  covBi(2,4)=PointSlpMomCorrErr(1,2);   // y-sx
  covBi(2,5)=PointSlpMomCorrErr(2,2);   // y-sy
  covBi(2,6)=PointSlpMomCorrErr(3,2);   // y-p
  covBi(3,4)=PointSlpMomCorrErr(1,3);   // z-sx
  covBi(3,5)=PointSlpMomCorrErr(2,3);   // z-sy
  covBi(3,6)=PointSlpMomCorrErr(3,3);   // z-p

  covBi(4,4)=SlpMomCorrErr(1,1);       // sx-sx
  covBi(4,5)=SlpMomCorrErr(1,2);       // sx-sy
  covBi(4,6)=SlpMomCorrErr(3,1);       // sx-p
  covBi(5,5)=SlpMomCorrErr(2,2);       // sy-sy
  covBi(5,6)=SlpMomCorrErr(3,2);       // sy-p
  covBi(6,6)=SlpMomCorrErr(3,3);       // p-p 


  // Get the reco primary vertex and its covariance matrix :

  HepPoint3D fvertexi = vertex.position();  

  HepSymMatrix covfvertexi = vertex.positionErr();


  // Fills the working vector containing param to fit :
  // NB : the fit is performed in the (PxB,PyB,PzB,xB,yB,zB,xVP,yVP,zVP)
  // coordinate system.
  

  HepVector alpha0(9);
  
  alpha0(1)=pBi.px(); 
  alpha0(2)=pBi.py();
  alpha0(3)=pBi.pz();
  alpha0(4)=vBi.x(); 
  alpha0(5)=vBi.y(); 
  alpha0(6)=vBi.z(); 
  alpha0(7)=fvertexi.x(); 
  alpha0(8)=fvertexi.y();
  alpha0(9)=fvertexi.z();
  
  HepVector alphainit = alpha0; 

  HepMatrix FitCov(9,9);
  HepMatrix deltacov(9,9);

  // Fills the corresponding cov matrix in its input form :

  HepMatrix alpcovinit(9,9);
  int i,j ;


  for( i = 1 ; i !=7 ; i++ ) {
    for( j = 7 ; j !=10 ; j++ ) { 
      alpcovinit(i,j) = 0.0  ; 
     }
  }

  for( i = 1 ; i !=4 ; i++ ) {
    for( j = 1 ; j !=4 ; j++ ) { 
      alpcovinit(i,j) = covBi(i+3,j+3) ;
     }
  }

  for( i = 1 ; i !=4 ; i++ ) {
    for( j = 1 ; j !=4 ; j++ ) { 
      alpcovinit(i+3,j+3) = covBi(i,j) ;
     }
  }  

  for( i = 1 ; i !=4 ; i++ ) {
    for( j = 1 ; j !=4 ; j++ ) { 
      alpcovinit(i+6,j+6) = covfvertexi(i,j) ;
     }
  }   

  alpcovinit(1,4) = covBi(1,4);
  alpcovinit(1,5) = covBi(2,4);
  alpcovinit(1,6) = covBi(3,4);
  alpcovinit(2,4) = covBi(1,5);
  alpcovinit(2,5) = covBi(2,5);
  alpcovinit(2,6) = covBi(3,5);
  alpcovinit(3,4) = covBi(1,6); 
  alpcovinit(3,5) = covBi(2,6);
  alpcovinit(3,6) = covBi(3,6);

  for( i= 1 ; i !=10 ; i++ ) {
    for( j= 1 ; j !=10 ; j++ ) {
      if( i > j ) {
        alpcovinit(i,j)=alpcovinit(j,i) ;
      }

    }
  }


  // Jacobian to go from the (Slx, Sly, P) form to (Px, Py, Pz) :
 
  HepMatrix jacob(9,9);

  for( i= 1 ; i !=10 ; i++ ) {
    for( j= 1 ; j !=10 ; j++ ) {
       jacob(i,j)= 0.0 ;
    }
  }  

  for( i= 1 ; i !=10 ; i++ ) {
    for( j= 1 ; j !=10 ; j++ ) {
      if( i == j ) {     
       jacob(i,j)= 1.0 ;
      } 
    }
  }



  double aux = sqrt(Sx*Sx+Sy*Sy+1);
  double aux2 = Sx*Sx+Sy*Sy+1;
  double aux3 =aux*aux*aux;

  
  jacob(1,1)= (P/aux)*( 1 - (  Sx*Sx/aux2 ) ) ;
  jacob(1,2)= -Sx*Sy*P/aux3 ;
  jacob(1,3)= Sx/aux ; 
  jacob(2,1)= -Sx*Sy*P/aux3 ;  
  jacob(2,2)= (P/aux)*( 1 - (  Sy*Sy/aux2 ) ) ;  
  jacob(2,3)= Sy/aux ;    
  jacob(3,1)= -Sx*P/aux3 ;   
  jacob(3,2)= -Sy*P/aux3 ; 
  jacob(3,3)= 1/aux ;    

  // Jacobian's tranpose

  HepMatrix jacobT = jacob.T();

  // The corresponding cov matrix with the working form :
  // ie the cov mat in the (PxB,PyB,PzB,xB,yB,zB,xVP,yVP,zVP)
  // coordinate system.


  HepMatrix alpcovinitMjacobT( alpcovinit*jacobT ) ;
    
  HepMatrix alpcov( jacob*alpcovinitMjacobT ) ; 


  HepMatrix alpcovinitia = alpcov ;  

  HepVector d(3) ;
 
  d(1)=100000;
  d(2)=100000;
  d(3)=100000;

  double seuil = sqrt( d(1)*d(1)+d(2)*d(2)+d(3)*d(3) ) ; 

  // The constraint equations vector 

  d(1) = (alpha0(4) - alpha0(7) )*alpha0(3) - (alpha0(6) - alpha0(9) )*alpha0(1) ;
  d(2) = (alpha0(5) - alpha0(8) )*alpha0(1) - (alpha0(4) - alpha0(7) )*alpha0(2) ;
  d(3) = (alpha0(6) - alpha0(9) )*alpha0(2) - (alpha0(5) - alpha0(8) )*alpha0(3) ;

  double seuil0 = sqrt( d(1)*d(1)+d(2)*d(2)+d(3)*d(3) ) ; 
  

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!! Loop on the fit iterations begins here !!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    

  int ite;

  for( ite = 1 ; ite !=10 ; ite++ ){
  
    if( (seuil > 0.001) && ( (seuil0/P) < 1 )  ) { 

     // The seuil0/P < 1 mm condition is mandatory, a fortiori if several Primary 
     // vertices. 

   
     // The constraints Matrix and its transpose  

     HepMatrix D(3,9);

     D(1,1)= alpha0(9) - alpha0(6);
     D(1,2)= 0.0;
     D(1,3)= alpha0(4) - alpha0(7);
     D(1,4)= alpha0(3);
     D(1,5)= 0.0;
     D(1,6)= -alpha0(1);
     D(1,7)= -alpha0(3);
     D(1,8)= 0.0;
     D(1,9)= alpha0(1);
     D(2,1)= alpha0(5) - alpha0(8);
     D(2,2)= alpha0(7) - alpha0(4);
     D(2,3)= 0.0;
     D(2,4)= -alpha0(2);
     D(2,5)= alpha0(1);
     D(2,6)= 0.0;
     D(2,7)= alpha0(2);
     D(2,8)= -alpha0(1);
     D(2,9)= 0.0;
     D(3,1)= 0.0;
     D(3,2)= alpha0(6) - alpha0(9);
     D(3,3)= alpha0(8) - alpha0(5);
     D(3,4)= 0.0;
     D(3,5)= -alpha0(3);
     D(3,6)= alpha0(2);
     D(3,7)= 0.0;
     D(3,8)= alpha0(3);
     D(3,9)= -alpha0(2);        

     HepMatrix Dt = D.T();

  ///////// End of Step 1 ///////////////////////////////////////////////////////


  
  ///// Step 2 : Calculate the fitted parameters //////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////


  // Vd = ( D*alpcov*Dt )^(-1)

     int inv0;

     HepMatrix alpcovDt( alpcov*Dt );
     HepMatrix DalpcovDt( D*alpcovDt) ; 

     HepMatrix Vd = DalpcovDt.inverse(inv0); 


  // D*alpcov 

     HepMatrix  Dalpcov( D*alpcov );

  // Vd*D*alpcov 

     HepMatrix  VdDalpcov( Vd*Dalpcov );

  // Dt*Vd*D*alpcov 

     HepMatrix DtVd( Dt*VdDalpcov );
  
  // deltacov = alpcov*Dt*Vd*D*alpcov 

     deltacov =  alpcov*DtVd ;



  // Vd*d

     HepVector Vdd( Vd*d );
  
  // Dt*Vd*d
  
     HepVector DtVdd( Dt*Vdd );


  // deltaFit = alpcov*Dt*Vd*d
  
     HepVector deltafit( alpcov*DtVdd );


  // Fitted parameters alphaFit = alpha0 - deltafit

     HepVector alphaFit = alpha0 - deltafit ; 

  // for the next iteration

     alpha0 = alphaFit;

  // Constraint equations vector reevaluation, and threshold calculation : 

     d(1) = (alpha0(4) - alpha0(7) )*alpha0(3) - (alpha0(6) - alpha0(9) )*alpha0(1) ;
     d(2) = (alpha0(5) - alpha0(8) )*alpha0(1) - (alpha0(4) - alpha0(7) )*alpha0(2) ;
     d(3) = (alpha0(6) - alpha0(9) )*alpha0(2) - (alpha0(5) - alpha0(8) )*alpha0(3) ;


     double Psl = sqrt(alpha0(1)*alpha0(1)+alpha0(2)*alpha0(2)+ alpha0(3)*alpha0(3)); 
     seuil = sqrt( d(1)*d(1)+d(2)*d(2)+d(3)*d(3) )/Psl ; 


    }


  } // iteration loop       


  // Update the Covriance Matrix
  // The covariance matrix of the fit parameters  Fitcov = alpcov - deltacov 

  FitCov = alpcov - deltacov ;
  
  ///////// End of Step 2 ///////////////////////////////////////////////////////


  
  ///// Step 3 : Fills the output objects //////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
 
  
  // B momentum 

  HepVector pB(3);

  pB(1) = alpha0(1);  
  pB(2) = alpha0(2);    
  pB(3) = alpha0(3);
  
  // B decay vertex

  HepVector vB(3);

  vB(1) = alpha0(4);  
  vB(2) = alpha0(5);    
  vB(3) = alpha0(6);     

  // Primary vertex 

  HepVector fvertex(3);

  fvertex(1) = alpha0(7);
  fvertex(2) = alpha0(8);
  fvertex(3) = alpha0(9); 

  // B momentum covariance matrix


  HepSymMatrix  covpB(3,0); 

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      covpB(i,j) = FitCov(i,j) ;
    }
  }   

  // B decay vertex covariance matrix


  HepSymMatrix covvB(3,0);

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      covvB(i,j) = FitCov(i+3,j+3) ;
     }
  } 

  // Primary vertex covariance matrix

  HepSymMatrix covfvertex(3,0);

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      covfvertex(i,j) = FitCov(i+6,j+6) ;
    }
  }

  // Chi2 : for the moment like Juan....  

  double chi2 = 0 ;

  HepVector deltaalp = alpha0 - alphainit ;

  int inv1 ;
 
  HepMatrix invalpcovinitia = alpcovinitia.inverse(inv1);

  HepVector invcovdelta ( invalpcovinitia*deltaalp );

  chi2 = dot( deltaalp , invcovdelta);

  //  int ichi;

  //for ( ichi = 1 ; ichi !=10 ; ichi++ ){
  //  chi2 = chi2 + deltaalp(ichi)*invcovdelta(ichi) ;

  //}  

  double testinho = 1.0 ;

  if( testinho > 0 ) {
 
  /////// fill the OUTPUT vertex object

  //    position
  fitVtx.setPosition(HepPoint3D(alpha0(4),alpha0(5),alpha0(6))); 

  //    position error
  HepSymMatrix posErr = covvB;
  fitVtx.setPositionErr(posErr);

  //    chisq, degrees of freedom
  fitVtx.setChi2(chi2);
  fitVtx.setNDoF(1);

  //    Vertex::DecayWithMass 
  fitVtx.setType(Vertex::DecayWithMass);

  //    particles that compose the vertex

  SmartRefVector<Particle>::const_iterator it;

  for(it = PartProd.begin(); it != PartProd.end(); 
      it++) {
    fitVtx.addToProducts(*it);
  }

  //////// fill the OUTPUT  Fitparticle

  // calculate fit particle momentum
  HepLorentzVector fitHepVec;
  fitHepVec.setPz( alpha0(3) );
  fitHepVec.setPx( alpha0(1) );
  fitHepVec.setPy( alpha0(2) );
  fitHepVec.setE( Energinit );
  
  //fill the Fit particle
  //   momentum
  fitParticle.setMomentum(fitHepVec);

  //   point on track
  fitParticle.setPointOnTrack(HepPoint3D(alpha0(4),alpha0(5),alpha0(6)));

  //   point on track error (same as error vertex pos error)
  fitParticle.setPointOnTrackErr(posErr);

  //  Covariance matrix of the momentum   
  HepSymMatrix m_MomCorrelationError(4,0) ;

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      m_MomCorrelationError(i,j)=covpB(i,j);
    }
  }

  for( i= 1 ; i !=5 ; i++ ) {
      m_MomCorrelationError(i,4)=0;
  }


  fitParticle.setMomentumErr(m_MomCorrelationError);

  //   Covariance between the momentum, and the position of the decay vertex  

  HepMatrix m_posMomCorrelation(3,3);

  m_posMomCorrelation(1,1)=FitCov(4,1);         // x - Px
  m_posMomCorrelation(2,1)=FitCov(4,2);         // x - Py
  m_posMomCorrelation(3,1)=FitCov(4,3);         // x - Pz
  m_posMomCorrelation(1,2)=FitCov(5,1);         // y - Px
  m_posMomCorrelation(2,2)=FitCov(5,2);         // y - Py
  m_posMomCorrelation(3,2)=FitCov(5,3);         // y - Pz
  m_posMomCorrelation(1,3)=FitCov(6,1);         // z - Px
  m_posMomCorrelation(2,3)=FitCov(6,2);         // z - Py
  m_posMomCorrelation(3,3)=FitCov(6,3);         // z - Pz


  fitParticle.setPosMomCorr(m_posMomCorrelation);  

  //   particle id  
  fitParticle.setParticleID( particle.particleID() );
  //   confidence level
  fitParticle.setConfLevel( particle.confLevel());
  //   mass
  fitParticle.setMass(fitHepVec.m());
  // Decay vertex of the fitParticle
  fitParticle.setEndVertex(&fitVtx);


  }
  
  log << MSG::INFO <<"FitWithDirectionres seuil =  "<< seuil <<  endreq;
   
  return StatusCode::SUCCESS ;
}

//==================================================================
// Performs the constrained fit that forces the momentum of a Particle
// to point into another vertex. This returns the fitted primary vertex.
//==================================================================
StatusCode LagrangeDirectionFitter::fitWithDirection(Particle& particle,Vertex& vertex, 
                            Particle& fitParticle,Vertex& fitVtx, Vertex& fitPV ) 
                                              
{

  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << " I begin to implement the B pointing fit " << endreq;
  log << MSG::DEBUG << "                                          " << endreq;

 
  // a Few variables to initialize
 
  double Energinit = particle.momentum().e();
  Vertex* decvert =  particle.endVertex(); 
  SmartRefVector<Particle> PartProd = decvert->products();

 

  ///// Step 1 : get the inputs //////////////////////////////////////////  
  ////////////////////////////////////////////////////////////////////////

  // Get the Bd reco momentum, decay vertex, and covariance matrix :
  
  HepLorentzVector pBi = particle.momentum();    

  double Sx = particle.slopeX();
  double Sy = particle.slopeY();
  double P  = particle.p();


  HepPoint3D vBi = particle.pointOnTrack();
  
   
  HepSymMatrix covBi(6,0); 

  HepSymMatrix pointErr = particle.pointOnTrackErr();
  HepSymMatrix SlpMomCorrErr = particle.slopesMomErr();
  HepMatrix PointSlpMomCorrErr = particle.posSlopesCorr();
  
  covBi(1,1) = pointErr(1,1);             // x-x
  covBi(2,1) = pointErr(2,1);             // y-x  
  covBi(2,2) = pointErr(2,2);             // y-y 
  

  covBi(3,1) = pointErr(3,1);             // z-x
  covBi(3,2) = pointErr(3,2);             // z-y
    
  covBi(3,3) = pointErr(3,3);             // z-z 

  covBi(1,4)=PointSlpMomCorrErr(1,1);   // x-sx
  covBi(1,5)=PointSlpMomCorrErr(2,1);   // x-sy
  covBi(1,6)=PointSlpMomCorrErr(3,1);   // x-p
  covBi(2,4)=PointSlpMomCorrErr(1,2);   // y-sx
  covBi(2,5)=PointSlpMomCorrErr(2,2);   // y-sy
  covBi(2,6)=PointSlpMomCorrErr(3,2);   // y-p
  covBi(3,4)=PointSlpMomCorrErr(1,3);   // z-sx
  covBi(3,5)=PointSlpMomCorrErr(2,3);   // z-sy
  covBi(3,6)=PointSlpMomCorrErr(3,3);   // z-p

  covBi(4,4)=SlpMomCorrErr(1,1);       // sx-sx
  covBi(4,5)=SlpMomCorrErr(1,2);       // sx-sy
  covBi(4,6)=SlpMomCorrErr(3,1);       // sx-p
  covBi(5,5)=SlpMomCorrErr(2,2);       // sy-sy
  covBi(5,6)=SlpMomCorrErr(3,2);       // sy-p
  covBi(6,6)=SlpMomCorrErr(3,3);       // p-p 


  // Get the reco primary vertex and its covariance matrix :

  HepPoint3D fvertexi = vertex.position();  

  HepSymMatrix covfvertexi = vertex.positionErr();


  // Fills the working vector containing param to fit :
  // NB : the fit is performed in the (PxB,PyB,PzB,xB,yB,zB,xVP,yVP,zVP)
  // coordinate system.
  

  HepVector alpha0(9);
  
  alpha0(1)=pBi.px(); 
  alpha0(2)=pBi.py();
  alpha0(3)=pBi.pz();
  alpha0(4)=vBi.x(); 
  alpha0(5)=vBi.y(); 
  alpha0(6)=vBi.z(); 
  alpha0(7)=fvertexi.x(); 
  alpha0(8)=fvertexi.y();
  alpha0(9)=fvertexi.z();
  
  HepVector alphainit = alpha0; 

  HepMatrix FitCov(9,9);
  HepMatrix deltacov(9,9);

  // Fills the corresponding cov matrix in its input form :

  HepMatrix alpcovinit(9,9);
  int i,j ;


  for( i = 1 ; i !=7 ; i++ ) {
    for( j = 7 ; j !=10 ; j++ ) { 
      alpcovinit(i,j) = 0.0  ; 
     }
  }

  for( i = 1 ; i !=4 ; i++ ) {
    for( j = 1 ; j !=4 ; j++ ) { 
      alpcovinit(i,j) = covBi(i+3,j+3) ;
     }
  }

  for( i = 1 ; i !=4 ; i++ ) {
    for( j = 1 ; j !=4 ; j++ ) { 
      alpcovinit(i+3,j+3) = covBi(i,j) ;
     }
  }  

  for( i = 1 ; i !=4 ; i++ ) {
    for( j = 1 ; j !=4 ; j++ ) { 
      alpcovinit(i+6,j+6) = covfvertexi(i,j) ;
     }
  }   

  alpcovinit(1,4) = covBi(1,4);
  alpcovinit(1,5) = covBi(2,4);
  alpcovinit(1,6) = covBi(3,4);
  alpcovinit(2,4) = covBi(1,5);
  alpcovinit(2,5) = covBi(2,5);
  alpcovinit(2,6) = covBi(3,5);
  alpcovinit(3,4) = covBi(1,6); 
  alpcovinit(3,5) = covBi(2,6);
  alpcovinit(3,6) = covBi(3,6);

  for( i= 1 ; i !=10 ; i++ ) {
    for( j= 1 ; j !=10 ; j++ ) {
      if( i > j ) {
        alpcovinit(i,j)=alpcovinit(j,i) ;
      }

    }
  }


  // Jacobian to go from the (Slx, Sly, P) form to (Px, Py, Pz) :
 
  HepMatrix jacob(9,9);

  for( i= 1 ; i !=10 ; i++ ) {
    for( j= 1 ; j !=10 ; j++ ) {
       jacob(i,j)= 0.0 ;
    }
  }  

  for( i= 1 ; i !=10 ; i++ ) {
    for( j= 1 ; j !=10 ; j++ ) {
      if( i == j ) {     
       jacob(i,j)= 1.0 ;
      } 
    }
  }



  double aux = sqrt(Sx*Sx+Sy*Sy+1);
  double aux2 = Sx*Sx+Sy*Sy+1;
  double aux3 =aux*aux*aux;

  
  jacob(1,1)= (P/aux)*( 1 - (  Sx*Sx/aux2 ) ) ;
  jacob(1,2)= -Sx*Sy*P/aux3 ;
  jacob(1,3)= Sx/aux ; 
  jacob(2,1)= -Sx*Sy*P/aux3 ;  
  jacob(2,2)= (P/aux)*( 1 - (  Sy*Sy/aux2 ) ) ;  
  jacob(2,3)= Sy/aux ;    
  jacob(3,1)= -Sx*P/aux3 ;   
  jacob(3,2)= -Sy*P/aux3 ; 
  jacob(3,3)= 1/aux ;    

  // Jacobian's tranpose

  HepMatrix jacobT = jacob.T();

  // The corresponding cov matrix with the working form :
  // ie the cov mat in the (PxB,PyB,PzB,xB,yB,zB,xVP,yVP,zVP)
  // coordinate system.


  HepMatrix alpcovinitMjacobT( alpcovinit*jacobT ) ;
    
  HepMatrix alpcov( jacob*alpcovinitMjacobT ) ; 

  log << MSG::DEBUG << "COV of observables alpcov "<< alpcov  << endreq;


  HepMatrix alpcovinitia = alpcov ;  

  HepVector d(2) ;
 
  d(1)=100000;
  d(2)=100000;

  double seuil = sqrt( d(1)*d(1)+d(2)*d(2)) ; 

  // The constraint equations vector 

  d(1) = (alpha0(4) - alpha0(7) )*alpha0(3) - (alpha0(6) - alpha0(9) )*alpha0(1) ;
  d(2) = (alpha0(5) - alpha0(8) )*alpha0(1) - (alpha0(4) - alpha0(7) )*alpha0(2) ;

  double seuil0 = sqrt( d(1)*d(1)+d(2)*d(2) ) ; 
  

  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!! Loop on the fit iterations begins here !!!!!!!!!!!!!!!!!!!!!!!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  log<< MSG::DEBUG << "Initial alpha0 are " << alpha0 <<endmsg;
    

  int ite;

  for( ite = 1 ; ite !=10 ; ite++ ){
    if( (seuil > 0.001) && ( (seuil0/P) < 1 ) ) { 

     // The seuil0/P < 1 mm condition is mandatory, a fortiori if several Primary 
     // vertices. 

   
     // The constraints Matrix and its transpose  

     HepMatrix D(2,9);

     D(1,1)= alpha0(9) - alpha0(6);
     D(1,2)= 0.0;
     D(1,3)= alpha0(4) - alpha0(7);
     D(1,4)= alpha0(3);
     D(1,5)= 0.0;
     D(1,6)= -alpha0(1);
     D(1,7)= -alpha0(3);
     D(1,8)= 0.0;
     D(1,9)= alpha0(1);
     D(2,1)= alpha0(5) - alpha0(8);
     D(2,2)= alpha0(7) - alpha0(4);
     D(2,3)= 0.0;
     D(2,4)= -alpha0(2);
     D(2,5)= alpha0(1);
     D(2,6)= 0.0;
     D(2,7)= alpha0(2);
     D(2,8)= -alpha0(1);
     D(2,9)= 0.0;

     HepMatrix Dt = D.T();

  ///////// End of Step 1 ///////////////////////////////////////////////////////


  
  ///// Step 2 : Calculate the fitted parameters //////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////


  // Vd = ( D*alpcov*Dt )^(-1)

     int inv0;

     HepMatrix alpcovDt( alpcov*Dt );
     HepMatrix DalpcovDt( D*alpcovDt) ; 

     HepMatrix Vd = DalpcovDt.inverse(inv0); 


  // D*alpcov 

     HepMatrix  Dalpcov( D*alpcov );

  // Vd*D*alpcov 

     HepMatrix  VdDalpcov( Vd*Dalpcov );

  // Dt*Vd*D*alpcov 

     HepMatrix DtVd( Dt*VdDalpcov );
  
  // deltacov = alpcov*Dt*Vd*D*alpcov 

     deltacov =  alpcov*DtVd ;



  // Vd*d
  //modified by xieyh
  //   HepVector Vdd( Vd*d );
     HepVector Vdd( Vd*(d-D*alpha0+D*alphainit) );

  // Dt*Vd*d
  
     HepVector DtVdd( Dt*Vdd );


  // deltaFit = alpcov*Dt*Vd*d
  
     HepVector deltafit( alpcov*DtVdd );


  // Fitted parameters alphaFit = alpha0 - deltafit
  // modified by xieyh
  //   HepVector alphaFit = alpha0 - deltafit ; 
     HepVector alphaFit = alphainit - deltafit ;

  // for the next iteration

     alpha0 = alphaFit;


  // Constraint equations vector reevaluation, and threshold calculation : 

     d(1) = (alpha0(4) - alpha0(7) )*alpha0(3) - (alpha0(6) - alpha0(9) )*alpha0(1) ;
     d(2) = (alpha0(5) - alpha0(8) )*alpha0(1) - (alpha0(4) - alpha0(7) )*alpha0(2) ;


     double Psl = sqrt(alpha0(1)*alpha0(1)+alpha0(2)*alpha0(2)+ alpha0(3)*alpha0(3)); 
     seuil = sqrt( d(1)*d(1)+d(2)*d(2) )/Psl ; 

     log<< MSG::DEBUG << "alpha0 after iteration " << ite <<" ==> "<< alpha0 <<endmsg;


        double dirL=sqrt((alpha0(4)- alpha0(7))*(alpha0(4)- alpha0(7)) + 
                         (alpha0(5)- alpha0(8))*(alpha0(5)- alpha0(8)) +
                         (alpha0(6)- alpha0(9))*(alpha0(6)- alpha0(9))  );
        double dirfitBmom=sqrt(alpha0(1)*alpha0(1) + alpha0(2)*alpha0(2) +alpha0(3)*alpha0(3)  );
        double tau =(1/(picosecond*c_light)) * (particle.mass())
                 *(dirL/dirfitBmom);

        double cTau= tau/1000.*mm*c_light*ns;
        log << MSG::DEBUG<<" CTau= "<< cTau<<endreq;
        log << MSG::DEBUG<<" dirL= "<< dirL<<endreq;
        log << MSG::DEBUG<<" dirfitBmom= "<< dirfitBmom<<endreq;
        log << MSG::DEBUG<<" Bmass= "<< particle.mass()<<endreq;
        log << MSG::DEBUG<<" tau= "<< tau<<endreq;


    }


  } // iteration loop       


  // Update the Covriance Matrix
  // The covariance matrix of the fit parameters  Fitcov = alpcov - deltacov 
  FitCov = alpcov - deltacov ;

  ///////// End of Step 2 ///////////////////////////////////////////////////////


  
  ///// Step 3 : Fills the output objects //////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
 
  
  // B momentum 

  HepVector pB(3);

  pB(1) = alpha0(1);  
  pB(2) = alpha0(2);    
  pB(3) = alpha0(3);
  
  // B decay vertex

  HepVector vB(3);

  vB(1) = alpha0(4);  
  vB(2) = alpha0(5);    
  vB(3) = alpha0(6);     

  // Primary vertex 

  HepVector fvertex(3);

  fvertex(1) = alpha0(7);
  fvertex(2) = alpha0(8);
  fvertex(3) = alpha0(9); 

  // B momentum covariance matrix


  HepSymMatrix  covpB(3,0); 

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      covpB(i,j) = FitCov(i,j) ;
    }
  }   

  // B decay vertex covariance matrix


  HepSymMatrix covvB(3,0);

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      covvB(i,j) = FitCov(i+3,j+3) ;
     }
  } 

  // Primary vertex covariance matrix

  HepSymMatrix fitcovfvertex(3,0);

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      fitcovfvertex(i,j) = FitCov(i+6,j+6) ;
    }
  }

  // Chi2 : for the moment like Juan....  

  double chi2 = 0 ;

  HepVector deltaalp = alpha0 - alphainit ;

  int inv1 ;
 
  HepMatrix invalpcovinitia = alpcovinitia.inverse(inv1);

  HepVector invcovdelta ( invalpcovinitia*deltaalp );

  chi2 = dot( deltaalp , invcovdelta);

  //  int ichi;

  //for ( ichi = 1 ; ichi !=10 ; ichi++ ){
  //  chi2 = chi2 + deltaalp(ichi)*invcovdelta(ichi) ;

  //}  

  double testinho = 1.0 ;

  if( testinho > 0 ) {
 
  /////// fill the OUTPUT vertex object

  fitPV.setPosition(HepPoint3D(alpha0(7),alpha0(8),alpha0(9)));
  fitPV.setPositionErr(fitcovfvertex);


  //    position
  fitVtx.setPosition(HepPoint3D(alpha0(4),alpha0(5),alpha0(6))); 

  //    position error
  HepSymMatrix posErr = covvB;
  fitVtx.setPositionErr(posErr);

  //    chisq, degrees of freedom
  fitVtx.setChi2(chi2);
  fitVtx.setNDoF(1);

  //    Vertex::DecayWithMass 
  fitVtx.setType(Vertex::DecayWithMass);

  //    particles that compose the vertex

  SmartRefVector<Particle>::const_iterator it;

  for(it = PartProd.begin(); it != PartProd.end(); 
      it++) {
    fitVtx.addToProducts(*it);
  }

  //////// fill the OUTPUT  Fitparticle

  // calculate fit particle momentum
  HepLorentzVector fitHepVec;
  fitHepVec.setPz( alpha0(3) );
  fitHepVec.setPx( alpha0(1) );
  fitHepVec.setPy( alpha0(2) );
  fitHepVec.setE( Energinit );
  
  //fill the Fit particle
  //   momentum
  fitParticle.setMomentum(fitHepVec);

  //   point on track
  fitParticle.setPointOnTrack(HepPoint3D(alpha0(4),alpha0(5),alpha0(6)));

  //   point on track error (same as error vertex pos error)
  fitParticle.setPointOnTrackErr(posErr);

  //  Covariance matrix of the momentum   
  HepSymMatrix m_MomCorrelationError(4,0) ;

  for( i= 1 ; i !=4 ; i++ ) {
    for( j= 1 ; j !=4 ; j++ ) {
      m_MomCorrelationError(i,j)=covpB(i,j);
    }
  }

  for( i= 1 ; i !=5 ; i++ ) {
      m_MomCorrelationError(i,4)=0;
  }


  fitParticle.setMomentumErr(m_MomCorrelationError);

  //   Covariance between the momentum, and the position of the decay vertex  

  HepMatrix m_posMomCorrelation(3,3);

  m_posMomCorrelation(1,1)=FitCov(4,1);         // x - Px
  m_posMomCorrelation(2,1)=FitCov(4,2);         // x - Py
  m_posMomCorrelation(3,1)=FitCov(4,3);         // x - Pz
  m_posMomCorrelation(1,2)=FitCov(5,1);         // y - Px
  m_posMomCorrelation(2,2)=FitCov(5,2);         // y - Py
  m_posMomCorrelation(3,2)=FitCov(5,3);         // y - Pz
  m_posMomCorrelation(1,3)=FitCov(6,1);         // z - Px
  m_posMomCorrelation(2,3)=FitCov(6,2);         // z - Py
  m_posMomCorrelation(3,3)=FitCov(6,3);         // z - Pz


  fitParticle.setPosMomCorr(m_posMomCorrelation);  

  //   particle id  
  fitParticle.setParticleID( particle.particleID() );
  //   confidence level
  fitParticle.setConfLevel( particle.confLevel());
  //   mass
  fitParticle.setMass(fitHepVec.m());
  // Decay vertex of the fitParticle
  fitParticle.setEndVertex(&fitVtx);


  }
  

  log << MSG::INFO <<"FitWithDirectionres seuil =  "<< seuil <<  endreq;

   
  return StatusCode::SUCCESS ;
}


//=============================================================================



