// $Id: ParticleStuffer.cpp,v 1.7 2004-03-25 16:46:00 pkoppenb Exp $ 
// Include files  
 
//  
#include <math.h> 
// from CLHEP 
#include "CLHEP/Geometry/Point3D.h" 
#include "CLHEP/Matrix/SymMatrix.h" 
#include "CLHEP/Matrix/Matrix.h" 
#include "CLHEP/Matrix/Vector.h" 
#include "CLHEP/Units/PhysicalConstants.h" 
#include "CLHEP/Vector/ThreeVector.h"  

// from Gaudi 
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/MsgStream.h"  
#include "GaudiKernel/IToolSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 
#include "GaudiKernel/GaudiException.h"
//
#include "GaudiKernel/Algorithm.h"

#include "Kernel/CLHEPStreams.h"

// Include files from Event
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local 
#include "ParticleStuffer.h" 
#include "DaVinciTools/IParticleTransporter.h"

//----------------------------------------------------------------------------- 
// Implementation file for class : ParticleStuffer 
// 
// 15/03/2002 : Paul Colrain 
//----------------------------------------------------------------------------- 

// Declaration of the Tool Factory 
static const  ToolFactory<ParticleStuffer>          s_factory ; 
const        IToolFactory& ParticleStufferFactory = s_factory ;  


//============================================================================= 
// Standard constructor, initializes variables 
//============================================================================= 
ParticleStuffer::ParticleStuffer( const std::string& type, 
                                  const std::string& name, 
                                  const IInterface* parent ) 
  : AlgTool ( type, name , parent ) 
    , m_ppSvc(0) 
    , m_pTransporter(0)  
    , m_transporterType("CombinedTransporter"){ 
 
  // declare additional Interface 
  declareInterface<IParticleStuffer>(this); 
  declareProperty("Transporter", m_transporterType); 
} 

//============================================================================= 

StatusCode ParticleStuffer::initialize() { 
  MsgStream log( msgSvc(), name() ); 
  
  // This tool needs to use internally the ParticlePropertySvc to retrieve the 
  // mass to be used 
  StatusCode sc = StatusCode::FAILURE; 
  sc = service( "ParticlePropertySvc", m_ppSvc ); 
  if( sc.isFailure ()) { 
    log << MSG::FATAL << "ParticlePropertySvc Not Found" << endreq; 
    return StatusCode::FAILURE; 
  } 
  sc = toolSvc()->retrieveTool(m_transporterType, m_pTransporter, this); 
  if(sc.isFailure()) { 
    log << MSG::FATAL << "Unable to retrieve " << m_transporterType 
        << "tool" << endreq; 
    return sc; 
  } 
  
  return StatusCode::SUCCESS; 
  
} 


//============================================================================= 
// Stuffer 
//============================================================================= 
StatusCode ParticleStuffer::fillParticle( const Vertex& vtx, Particle& part,  
                                          const ParticleID& pid ){ 
  
  
  MsgStream log( msgSvc(), name() );
 
  int stdHepID = pid.pid();
  
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(stdHepID  );
  if((*partProp).lifetime()*pow(10,-9) < pow(10,-15)) {
    part.setIsResonance(true);
  }
  
  
  // Set the ParticleID.
  part.setParticleID( pid ); 
  
  double zVtxPos = vtx.position().z();
  
  // Set the four-momentum.
  HepLorentzVector MotherlorVec( 0., 0., 0., 0. );
  HepSymMatrix meMat( 4, 0 ); 
  HepSymMatrix MothermeMat( 4, 0 ); 
  
  Particle transParticle;
  int const SIIDIM =  vtx.products().size() ;
  
  log << MSG::DEBUG << " ntracks = " << SIIDIM << endreq;
  SmartRefVector<Particle>::const_iterator it;  
  for (it = vtx.products().begin(); it != vtx.products().end(); it++ ) {
    
    if ( (*it)->pointOnTrack().z() > 950.) {
      log << MSG::DEBUG << "Position > 950 " 
          << (*it)->pointOnTrack().z() << endreq;
    }
    
    if ((*it)->isResonance()){
      MotherlorVec += (*it)->momentum();
    }
    else {    
      // Transport Particle parameters to the vertex position	 
      StatusCode sctrans = m_pTransporter->transport(*(*it),
                                                     zVtxPos,
                                                     transParticle);
      if ( !sctrans.isSuccess() ) {
        log << MSG::DEBUG << "Track extrapolation failed" << endreq;
        return sctrans;
      }
      
      MotherlorVec += transParticle.momentum(); 
    }   
  } //for (it = vtx.products )
  
  
  
  
  HepVector pzvec(SIIDIM,0) ;
  HepVector SII(SIIDIM,0)  ;
  HepVector sumh(6,0)  ;
  HepVector sum(3,0)  ;
  double sumpz =  0.0 ;
  double sum1pz =  0.0   ;
  double sum2pz =  0.0   ;
  double par1 =  0.0   ;
  double par2 =   0.0  ;
  
  
  HepMatrix sumhDeriv(36,SIIDIM,0)   ;
  HepMatrix parDeriv(8,SIIDIM,0)   ;
  HepMatrix sumpzDeriv(15,SIIDIM,0)   ;
  HepMatrix  Te(6,5*SIIDIM,0)   ;
  HepSymMatrix Ce(5*SIIDIM, 0)   ;
  HepSymMatrix Cx(6,0);
  HepVector e(5*SIIDIM,0);
  HepMatrix posErrTemp(3,SIIDIM,0);
  
  StatusCode sc_calc = calcCeEPosErr( vtx,
                                      Ce, e, posErrTemp ); 
  if (sc_calc ) {
    calcPar1Par2( SIIDIM, posErrTemp, e, sumh, sum, par1,  par2 );     
    calcSumhDeriv( SIIDIM, posErrTemp, e, sumhDeriv );   
    
    calcParDeriv( SIIDIM, sum, sumh, sumhDeriv, 
                  parDeriv ); 
    
    
    
    calcSumpz( SIIDIM, e, pzvec, SII, sumpz, sum1pz,  sum2pz );
    
    
    calcSumpzDeriv( SIIDIM, e, SII, pzvec, sumpzDeriv );        
    
    
    
    buildTeMatrix( SIIDIM, sumhDeriv, sumpzDeriv, parDeriv, sumh, sum,
                   par1, par2, sumpz,sum1pz, sum2pz, Te );    
    
  }
  else {return StatusCode::FAILURE;}
  
  
  
  // Make the matrix multiplication to obtain Cx 
  //  Cx =Te*Ce*Te.T;
  Cx = Ce.similarity(Te);
  
  
  //    position error
  HepSymMatrix posErr=Cx.sub(1,3);
  
  
  //   error on track slopes and prop to momentum
  HepSymMatrix m_slpMomCorrelationError=Cx.sub(4,6);
  part.setSlopesMomErr(m_slpMomCorrelationError);  
  
  
  //   corr err slopes point 
  HepMatrix CxCopy=Cx;
  HepMatrix m_posSlpMomCorrelationError=CxCopy.sub(4,6,1,3);
  part.setPosSlopesCorr(m_posSlpMomCorrelationError);                                                        
  
  
  part.setMomentum( MotherlorVec );
  
  // Set the measured mass.
  part.setMass( part.momentum().mag() ); 
  
  // Set the error on the measured mass.
  HepMatrix derivs(1,4);
  HepSymMatrix massErrSqd(1,1);
  derivs(1,1) = - part.momentum().px() / part.mass();
  derivs(1,2) = - part.momentum().py() / part.mass();
  derivs(1,3) = - part.momentum().pz() / part.mass();
  derivs(1,4) =   part.momentum().e()  / part.mass();
  
  massErrSqd = part.momentumErr().similarity( derivs );  
  
  double massErr = sqrt( massErrSqd(1,1) );
  part.setMassErr( massErr ); 
  
  // Set the point on track.
  part.setPointOnTrack( vtx.position() ); 
  
  // Set the point on track error matrix.
  part.setPointOnTrackErr( vtx.positionErr() ); 
  
  // Set the point - four-momentum error matrix.
  //  HepMatrix pmeMat( 4, 3, 0 );  
  //  part.setPosMomCorr( pmeMat );
  
  // Set the end vertex reference.
  part.setEndVertex( &vtx ); 
  
  // Set the point - slopes+momentum error matrix. Already done internally
  // by Particle via setPosMomCorr
  //    HepMatrix pseMat( 3, 3, 0 );  
  //    part.setPosSlopesCorr( pseMat );
  
  
  return StatusCode::SUCCESS; 
} 
//============================================================================= 



// This method calculates the numbers sumpz, sum1pz and sum2pz and the vectors
// pzvec and SII
// 
// pzvec = pi/sqrt(SII) 
// SII = (xi')^2 + (yi')^2 + 1 
// sumpz = sum(pzvec) 
// sum1pz = sum(xi'*pzvec) 
// sum2pz = sum(yi'*pzvec) 

void ParticleStuffer::calcSumpz( int const SIIDIM , HepVector &e, HepVector 
                                 &pzvec,  
                                 HepVector &SII, 
                                 double &sumpz, double &sum1pz, double 
                                 &sum2pz ){ 
  
  MsgStream log( msgSvc(), name() );
  
  
  
  
  int i=1; 
  
  log << MSG::DEBUG << " SIIDIM = " << SIIDIM << endreq;
  log << MSG::DEBUG << " e = " <<  endreq;
  log << MSG::DEBUG <<  e << endreq;
  
  for( i=1 ; i<=SIIDIM; i++ ){
    
    int index = 5*i;        // pi index 
    int index1 = 5*(i-1)+3; // xi' index 
    int index2 = 5*(i-1)+4; // yi' index 
    
    log << MSG::DEBUG << "i " << i << endreq;
    log << MSG::DEBUG << "index " << index << endreq;
    log << MSG::DEBUG << "index1 " << index1 << endreq;
    log << MSG::DEBUG << "index2 " << index2 << endreq;
    
    SII(i) = pow(e(index1),2) + pow(e(index2),2) + 1; 
    pzvec(i) = e(index)/sqrt(SII(i));	 
    
    sumpz += pzvec(i); 
    sum1pz += e(index1)*pzvec(i); 
    sum2pz += e(index2)*pzvec(i); 
    
  }//for( i=1 ; i<=SIIDIM; i++ )
  
  log << MSG::DEBUG << " sumpz = " << sumpz << endreq; 
  
  log << MSG::DEBUG << " sum1pz = " << sum1pz << endreq; 
  log << MSG::DEBUG << " sum2pz = " << sum2pz << endreq; 
  log << MSG::DEBUG << " SII = " << endreq; 
  log << MSG::DEBUG << SII << endreq;
  log << MSG::DEBUG << " pzvec = " << endreq; 
  log << MSG::DEBUG << pzvec << endreq;
  
  
  
} 









//This method  calculates the numbers par1 and par2 and the vectors sumh and sum


void ParticleStuffer::calcPar1Par2( int const SIIDIM , HepMatrix &posErrTemp, 
                                    HepVector &e, HepVector &sumh,  
                                    HepVector &sum, 
                                    double &par1, double &par2 ){ 
  
  MsgStream log( msgSvc(), name() );	
  
 
  
  log << MSG::DEBUG << " SIIDIM = " << SIIDIM << endreq;
  log << MSG::DEBUG << " e = " <<  endreq;
  log << MSG::DEBUG <<  e << endreq;
  log << MSG::DEBUG << " posErrTemp = " <<  endreq;
  log << MSG::DEBUG <<  posErrTemp << endreq;
  
  int i=1;
  
  double sum1 = 0;  
  double sum2 = 0;  
  double sum3 = 0;  
  
  double sumh1 = 0;  
  double sumh2 = 0;  
  double sumh3 = 0;  
  double sumh4 = 0;  
  double sumh5 = 0;  
  double sumh6 = 0;  
  
  for(i=1;i<=SIIDIM;i++){  
    
    int index1 = 5*(i-1)+3; // xi' index 
    int index2 = 5*(i-1)+4; // yi' index 
    int index3 = 5*(i-1)+1; // xi index 
    int index4 = 5*(i-1)+2; // yi index 
    
    
    log << MSG::DEBUG << "i " << i << endreq;
    log << MSG::DEBUG << "index1 " << index1 << endreq;
    log << MSG::DEBUG << "index2 " << index2 << endreq;
    log << MSG::DEBUG << "index3 " << index3 << endreq;
    log << MSG::DEBUG << "index4 " << index4 << endreq;
    
    
    sum1 += posErrTemp(1,i)*e(index3) + posErrTemp(2,i)*e(index4) ; 
    sum2 += posErrTemp(3,i)*e(index4) + posErrTemp(2,i)*e(index3) ; 
    sum3 += - (posErrTemp(1,i)*e(index3)*e(index1) + 
               posErrTemp(2,i)*e(index3)*e(index2) +  
               posErrTemp(2,i)*e(index4)*e(index1) 
               + posErrTemp(3,i)*e(index4)*e(index2) );	 
    
    sumh1 += posErrTemp(1,i) ;     
    sumh2 += posErrTemp(2,i) ;    
    sumh3 += -( posErrTemp(1,i) *e(index1) +  posErrTemp(2,i) *e(index2)  );    
    sumh4 += posErrTemp(3,i) ;    
    sumh5 +=  -( posErrTemp(3,i) *e(index2) + posErrTemp(2,i) *e(index1)  ) ;    
    sumh6 +=  posErrTemp(1,i) *pow(e(index1),2) +  
      posErrTemp(3,i) *pow(e(index2),2) +  
      2*posErrTemp(2,i) *e(index1)*e(index2)  ;    
    
  }// for(i=1;i<=SIIDIM;i++)     
  
  sum(1) = sum1 ; 
  sum(2) = sum2; 
  sum(3) = sum3; 
  
  sumh(1) = sumh1; 
  sumh(2) = sumh2; 
  sumh(3) = sumh3; 
  sumh(4) = sumh4; 
  sumh(5) = sumh5; 
  sumh(6) = sumh6;      
  
  par1 = sumh(1) - sumh(3)*(sumh(2)/sumh(5)) +  
    ( sumh(2) - sumh(3)*(sumh(4)/sumh(5))  )* 
    ( - sumh(3) + sumh(2)*(sumh(6)/sumh(5)) ) / 
    ( sumh(5) - sumh(6)*(sumh(4)/sumh(5)) )  ; 
  
  par2 = sum(1) - sum(3)*(sumh(2)/sumh(5)) +
    ( sum(2) - sum(3)*(sumh(4)/sumh(5))  )*
    ( - sumh(3) + sumh(2)*(sumh(6)/sumh(5)) ) /  
    ( sumh(5) - sumh(6)*(sumh(4)/sumh(5)) )   ; 
  
  
  log << MSG::DEBUG << " par1 = " << par1 << endreq;
  log << MSG::DEBUG << " par2 = " << par2 << endreq; 
  log << MSG::DEBUG << " sum = " << endreq; 
  log << MSG::DEBUG << sum << endreq;
  log << MSG::DEBUG << " sumh = " << endreq; 
  log << MSG::DEBUG << sumh << endreq;
  
  
  
  
} 



 








// This method builds the sumhDeriv matrix with the derivatives of the 
// vectors sumh and sum 
//

void ParticleStuffer::calcSumhDeriv( int const SIIDIM , HepMatrix &posErrTemp,
                                     HepVector &e,   
                                     HepMatrix &sumhDeriv ){ 
  
  MsgStream log( msgSvc(), name() );
  
 
  log << MSG::DEBUG << " SIIDIM = " << SIIDIM << endreq;
  log << MSG::DEBUG << " e = " <<  endreq;
  log << MSG::DEBUG <<  e << endreq;
  log << MSG::DEBUG << " posErrTemp = " <<  endreq;
  log << MSG::DEBUG <<  posErrTemp << endreq;
  
  int i;
  
  HepVector dsum1x( SIIDIM,0);  
  HepVector dsum1y( SIIDIM,0);  
  HepVector dsum1xs( SIIDIM,0);  
  HepVector dsum1ys( SIIDIM,0);  
  HepVector dsum2x( SIIDIM,0);  
  HepVector dsum2y( SIIDIM,0);  
  HepVector dsum2xs( SIIDIM,0);  
  HepVector dsum2ys( SIIDIM,0);  
  HepVector dsum3x( SIIDIM,0);  
  HepVector dsum3y( SIIDIM,0);  
  HepVector dsum3xs( SIIDIM,0);  
  HepVector dsum3ys( SIIDIM,0);  
  HepVector dsumh1x( SIIDIM,0);  
  HepVector dsumh1y( SIIDIM,0);  
  HepVector dsumh1xs( SIIDIM,0);  
  HepVector dsumh1ys( SIIDIM,0);  
  HepVector dsumh2x( SIIDIM,0);  
  HepVector dsumh2y( SIIDIM,0);  
  HepVector dsumh2xs( SIIDIM,0);  
  HepVector dsumh2ys( SIIDIM,0);  
  HepVector dsumh3x( SIIDIM,0);  
  HepVector dsumh3y( SIIDIM,0);  
  HepVector dsumh3xs( SIIDIM,0);  
  HepVector dsumh3ys( SIIDIM,0);  
  HepVector dsumh4x( SIIDIM,0);  
  HepVector dsumh4y( SIIDIM,0);  
  HepVector dsumh4xs( SIIDIM,0);  
  HepVector dsumh4ys( SIIDIM,0);  
  HepVector dsumh5x( SIIDIM,0);  
  HepVector dsumh5y( SIIDIM,0);  
  HepVector dsumh5xs( SIIDIM,0);  
  HepVector dsumh5ys( SIIDIM,0);  
  HepVector dsumh6x( SIIDIM,0);  
  HepVector dsumh6y( SIIDIM,0);  
  HepVector dsumh6xs( SIIDIM,0);  
  HepVector dsumh6ys( SIIDIM,0);  
  
  
  for(i=1;i<=SIIDIM;i++){  
    
    int index1 = 5*(i-1)+3; // xi' index 
    int index2 = 5*(i-1)+4; // yi' index 
    int index3 = 5*(i-1)+1; // xi index 
    int index4 = 5*(i-1)+2; // yi index 
    
    
    log << MSG::DEBUG << "i " << i << endreq;
    log << MSG::DEBUG << "index1 " << index1 << endreq;
    log << MSG::DEBUG << "index2 " << index2 << endreq;
    log << MSG::DEBUG << "index3 " << index3 << endreq;
    log << MSG::DEBUG << "index4 " << index4 << endreq;
    
    
    dsum1x(i) =  posErrTemp(1,i); 
    dsum1y(i) =  posErrTemp(2,i); 
    dsum2x(i) =  posErrTemp(2,i); 
    dsum2y(i) =  posErrTemp(3,i); 
    dsum3x(i) = - ( posErrTemp(1,i)*e(index1) + posErrTemp(2,i)*e(index2)  ) ; 
    dsum3y(i) = - ( posErrTemp(2,i)*e(index1) + posErrTemp(3,i)*e(index2)  ) ; 
    dsum3xs(i) = - ( posErrTemp(1,i)*e(index3) + posErrTemp(2,i)*e(index4)  ) ; 
    dsum3ys(i) = - ( posErrTemp(2,i)*e(index3) + posErrTemp(3,i)*e(index4)  ) ; 
    dsumh3xs(i) =  - posErrTemp(1,i); 
    dsumh3ys(i) =  - posErrTemp(2,i); 
    dsumh5xs(i) =  - posErrTemp(2,i); 
    dsumh5ys(i) =  - posErrTemp(3,i); 
    dsumh6xs(i) =  2*( posErrTemp(1,i)*e(index1) + 
                       posErrTemp(2,i)*e(index2) )   ; 
    dsumh6ys(i) = 2*(  posErrTemp(3,i)*e(index2) + 
                       posErrTemp(2,i)*e(index1)  ) ; 
    
    sumhDeriv(1,i) =    dsum1x(i) ; 
    sumhDeriv(2,i) =    dsum1y(i); 
    sumhDeriv(3,i) =    dsum1xs(i); 
    sumhDeriv(4,i) =    dsum1ys(i); 
    sumhDeriv(5,i) =    dsum2x(i); 
    sumhDeriv(6,i) =    dsum2y(i); 
    sumhDeriv(7,i) =    dsum2xs(i); 
    sumhDeriv(8,i) =    dsum2ys(i); 
    sumhDeriv(9,i) =    dsum3x(i); 
    sumhDeriv(10,i) =   dsum3y(i); 
    sumhDeriv(11,i) =   dsum3xs(i); 
    sumhDeriv(12,i) =   dsum3ys(i); 
    sumhDeriv(13,i) =   dsumh1x(i); 
    sumhDeriv(14,i) =   dsumh1y(i); 
    sumhDeriv(15,i) =   dsumh1xs(i); 
    sumhDeriv(16,i) =   dsumh1ys(i); 
    sumhDeriv(17,i) =   dsumh2x(i); 
    sumhDeriv(18,i) =   dsumh2y(i); 
    sumhDeriv(19,i) =   dsumh2xs(i); 
    sumhDeriv(20,i) =   dsumh2ys(i); 
    sumhDeriv(21,i) =   dsumh3x(i); 
    sumhDeriv(22,i) =   dsumh3y(i); 
    sumhDeriv(23,i) =   dsumh3xs(i); 
    sumhDeriv(24,i) =   dsumh3ys(i); 
    sumhDeriv(25,i) =   dsumh4x(i); 
    sumhDeriv(26,i) =   dsumh4y(i); 
    sumhDeriv(27,i) =   dsumh4xs(i); 
    sumhDeriv(28,i) =   dsumh4ys(i); 
    sumhDeriv(29,i) =   dsumh5x(i); 
    sumhDeriv(30,i) =   dsumh5y(i); 
    sumhDeriv(31,i) =   dsumh5xs(i); 
    sumhDeriv(32,i) =   dsumh5ys(i); 
    sumhDeriv(33,i) =   dsumh6x(i); 
    sumhDeriv(34,i) =   dsumh6y(i); 
    sumhDeriv(35,i) =   dsumh6xs(i); 
    sumhDeriv(36,i) =   dsumh6ys(i); 
    
  }// for(i=1;i<=SIIDIM;i++)   
  
  
  log << MSG::DEBUG << " sumhDeriv = "  << endreq;
  log << MSG::DEBUG << sumhDeriv << endreq;
  
  
  
} 









// This method builds the parDeriv matrix with the derivatives of the 
// vectors par1 and par2 
// 


void ParticleStuffer::calcParDeriv( int const SIIDIM , HepVector& sum,
                                    HepVector& sumh,  
                                    HepMatrix &sumhDeriv, HepMatrix 
                                    &parDeriv ){ 
  
  MsgStream log( msgSvc(), name() );
  
 
  log << MSG::DEBUG << " SIIDIM = " << SIIDIM << endreq;
  log << MSG::DEBUG << " sum = " <<  endreq;
  log << MSG::DEBUG <<  sum << endreq;
  log << MSG::DEBUG << " sumh = " <<  endreq;
  log << MSG::DEBUG <<  sumh << endreq;
  log << MSG::DEBUG << " sumhDeriv = " <<  endreq;
  log << MSG::DEBUG <<  sumhDeriv << endreq;
  
  int i;
  
  HepVector dpar1x( SIIDIM,0);  
  HepVector dpar1y( SIIDIM,0);  
  HepVector dpar1xs( SIIDIM,0);  
  HepVector dpar1ys( SIIDIM,0);  
  HepVector dpar2x( SIIDIM,0);  
  HepVector dpar2y( SIIDIM,0);  
  HepVector dpar2xs( SIIDIM,0);  
  HepVector dpar2ys( SIIDIM,0);  
  
  
  for(i=1;i<=SIIDIM;i++){  
    
    log << MSG::DEBUG << " i = " << i << endreq;
    
    
    
    dpar1xs(i) = - (sumh(2)/sumh(5))*sumhDeriv(23,i) +  
      (sumh(2)*sumh(3)/pow(sumh(5),2))*sumhDeriv(31,i) - 
      pow(    ( sumh(5) - sumh(4)*(sumh(6)/sumh(5))  ), -2) * 
      ( sumhDeriv(31,i) - (sumh(4)/sumh(5))*sumhDeriv(35,i) +
        (sumh(4)*sumh(6)/pow(sumh(5),2))*sumhDeriv(31,i)  )* 
      ( sumh(2) - (sumh(3)*sumh(4)/sumh(5))  )*(- sumh(3) +
                                                (sumh(2)*sumh(6)/sumh(5))  ) +
      pow(    ( sumh(5) - sumh(4)*(sumh(6)/sumh(5))  ), -1) * 
      ( ( - (sumh(4)/sumh(5))*sumhDeriv(23,i) +  
          (sumh(3)*sumh(4)/pow(sumh(5),2))*sumhDeriv(31,i) )  *
	      ( - sumh(3) + (sumh(2)*sumh(6)/sumh(5)) ) + 
	      (  sumh(2) - (sumh(3)*sumh(4)/sumh(5))   ) * 
        ( (sumh(2)/sumh(5))*sumhDeriv(35,i) - 
          (sumh(2)*sumh(6)/pow(sumh(5),2))*sumhDeriv(31,i) -
          sumhDeriv(23,i)  )       );
    
    
    
    
    
    dpar1ys(i) =   - (sumh(2)/sumh(5))*sumhDeriv(24,i)   +  
      (sumh(2)*sumh(3)/pow(sumh(5),2))*sumhDeriv(32,i) -
      pow(    ( sumh(5) - sumh(4)*(sumh(6)/sumh(5))  ), -2) * 
      ( sumhDeriv(32,i) - (sumh(4)/sumh(5))*sumhDeriv(36,i) +
        (sumh(4)*sumh(6)/pow(sumh(5),2))*sumhDeriv(32,i)   )*
      ( sumh(2) - (sumh(3)*sumh(4)/sumh(5))  )*
      ( - sumh(3) + (sumh(2)*sumh(6)/sumh(5))   ) +
      pow(    ( sumh(5) - sumh(4)*(sumh(6)/sumh(5))  ), -1) * 
      ( (  - (sumh(4)/sumh(5))*sumhDeriv(24,i) + 
           (sumh(3)*sumh(4)/pow(sumh(5),2))*sumhDeriv(32,i)  )*
        (  - sumh(3) + (sumh(2)*sumh(6)/sumh(5)) ) + 
        ( sumh(2) - (sumh(3)*sumh(4)/sumh(5))  )*
        ( (sumh(2)/sumh(5))*sumhDeriv(36,i) - 
          (sumh(2)*sumh(6)/pow(sumh(5),2))*sumhDeriv(32,i) -
          sumhDeriv(24,i)  )           );
    
    dpar2x(i) =  sumhDeriv(1,i) - (sumh(2)/sumh(5))*sumhDeriv(9,i) +
      pow ( ( sumh(5) - (sumh(4)*sumh(6)/sumh(5))  ), -1)* 
      ( -sumh(3) + sumh(2)*sumh(6)/sumh(5) )*
      ( sumhDeriv(5,i) - (sumh(4)/sumh(5))*sumhDeriv(9,i)  );
    
    dpar2y(i) = sumhDeriv(2,i) - (sumh(2)/sumh(5))*sumhDeriv(10,i) + 
      pow ( ( sumh(5) - (sumh(4)*sumh(6)/sumh(5))  ), -1)*
      ( -sumh(3) + (sumh(2)*sumh(6)/sumh(5))  ) *
      ( sumhDeriv(6,i) - (sumh(4)/sumh(5))*sumhDeriv(10,i)  ) ; 
    
    
    
    
    
    
    
    dpar2xs(i) =  - (sumh(2)/sumh(5))*sumhDeriv(11,i) + 
      (sumh(2)*sum(3)/pow(sumh(5),2))*sumhDeriv(31,i) -
      pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5))  ,-2)*
      ( sumhDeriv(31,i) - (sumh(4)/sumh(5))*sumhDeriv(35,i) + 
        (sumh(4)*sumh(6)/pow(sumh(5),2))*sumhDeriv(31,i) )*
      ( sum(2) - (sumh(4)*sum(3)/sumh(5)) )*
      (  -sumh(3) + (sumh(2)*sumh(6)/sumh(5)) ) + 
      pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5))  ,-1)*
      ( ( - (sumh(4)/sumh(5))*sumhDeriv(11,i) +  
          (sumh(4)*sum(3)/pow(sumh(5),2))*sumhDeriv(31,i) )*
        (-sumh(3) + (sumh(2)*sumh(6)/sumh(5))  ) + 
        ( sum(2) - (sumh(4)*sum(3)/sumh(5))  )*
        ( (sumh(2)/sumh(5))*sumhDeriv(35,i) - 
          (sumh(2)*sumh(6)/pow(sumh(5),2))*sumhDeriv(31,i) - 
          sumhDeriv(23,i)  )   );
    
    
    dpar2ys(i) = - (sumh(2)/sumh(5))*sumhDeriv(12,i) + 
      (sumh(2)*sum(3)/pow(sumh(5),2))*sumhDeriv(32,i) -
      pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5))  ,-2)*
      (  sumhDeriv(32,i) - (sumh(4)/sumh(5))*sumhDeriv(36,i) + 
	       (sumh(4)*sumh(6)/pow(sumh(5),2))*sumhDeriv(32,i)  )*
      ( sum(2) - (sumh(4)*sum(3)/sumh(5))  )*
      (  -sumh(3) + (sumh(2)*sumh(6)/sumh(5)) ) + 
      pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5))  ,-1)*
      ( ( -(sumh(4)/sumh(5))*sumhDeriv(12,i) + 
          (sumh(4)*sum(3)/pow(sumh(5),2))*sumhDeriv(32,i)  )*
        (  -sumh(3) + (sumh(2)*sumh(6)/sumh(5)) ) + 
        ( sum(2) - (sumh(4)*sum(3)/sumh(5)) )*
        ( (sumh(2)/sumh(5))*sumhDeriv(36,i) - 
          (sumh(2)*sumh(6)/pow(sumh(5),2))*sumhDeriv(32,i) - 
          sumhDeriv(24,i) )      );
    
    parDeriv(3,i) =   dpar1xs(i); 
    parDeriv(4,i) =   dpar1ys(i); 
    parDeriv(5,i) =   dpar2x(i); 
    parDeriv(6,i) =   dpar2y(i); 
    parDeriv(7,i) =   dpar2xs(i); 
    parDeriv(8,i) =   dpar2ys(i); 
    
  }// for(i=1;i<=SIIDIM;i++)    
  
  log << MSG::DEBUG << " parDeriv = "  << endreq;
  log << MSG::DEBUG << parDeriv << endreq;
  
  
} 















// This method builds the sumpzDeriv matrix with the derivatives of the 
// vectors SII and pz and the numbers sumpz, sum1pz and sum2pz 
// 


void ParticleStuffer::calcSumpzDeriv( int const SIIDIM , HepVector &e, 
                                      HepVector& SII,
                                      HepVector& pzver, 
                                      HepMatrix &sumpzDeriv){     
  
  MsgStream log( msgSvc(), name() );
  
 
  log << MSG::DEBUG << " SIIDIM = " << SIIDIM << endreq;
  log << MSG::DEBUG << " e = " <<  endreq;
  log << MSG::DEBUG <<  e << endreq;
  log << MSG::DEBUG << " SII = " <<  endreq;
  log << MSG::DEBUG <<  SII << endreq;
  log << MSG::DEBUG << " pzver = " <<  endreq;
  log << MSG::DEBUG <<  pzver << endreq;
  
  int i;
  
  HepVector dSIIxs(SIIDIM,0); 
  HepVector dSIIys(SIIDIM,0); 
  HepVector dSIIp(SIIDIM,0); 
  HepVector dpzxs(SIIDIM,0); 
  HepVector dpzys(SIIDIM,0); 
  HepVector dpzp(SIIDIM,0); 
  HepVector dsumpzxs(SIIDIM,0); 
  HepVector dsumpzys(SIIDIM,0); 
  HepVector dsumpzp(SIIDIM,0); 
  HepVector dsum1pzxs(SIIDIM,0); 
  HepVector dsum1pzys(SIIDIM,0); 
  HepVector dsum1pzp(SIIDIM,0); 
  HepVector dsum2pzxs(SIIDIM,0); 
  HepVector dsum2pzys(SIIDIM,0); 
  HepVector dsum2pzp(SIIDIM,0); 
  
  for(i=1;i<=SIIDIM;i++){  
    
    int index = 5*i;        // pi index 
    int index1 = 5*(i-1)+3; // xi' index 
    int index2 = 5*(i-1)+4; // yi' index 
    
    
    log << MSG::DEBUG << "i " << i << endreq;
    log << MSG::DEBUG << "index " << index << endreq;
    log << MSG::DEBUG << "index1 " << index1 << endreq;
    log << MSG::DEBUG << "index2 " << index2 << endreq;
    
    
    dSIIxs(i) = 2*e(index1) ; 
    dSIIys(i) = 2*e(index2); 
    dpzxs(i) = - (e(index)/(2*pow(SII(i),1.5)))*dSIIxs(i)  ; 
    dpzys(i) = - (e(index)/(2*pow(SII(i),1.5)))*dSIIys(i)  ; 
    dpzp(i) = 1/sqrt(SII(i))  ; 
    dsumpzxs(i) =  dpzxs(i); 
    dsumpzys(i) =  dpzys(i); 
    dsumpzp(i) =  dpzp(i); 
    dsum1pzxs(i) = pzver(i) + e(index1)*dpzxs(i); 
    dsum1pzys(i) =  e(index1)*dpzys(i); 
    dsum1pzp(i) =  e(index1)*dpzp(i); 
    
    dsum2pzxs(i) =  e(index2)*dpzxs(i); 
    dsum2pzys(i) =  pzver(i) + e(index2)*dpzys(i); 
    dsum2pzp(i) =  e(index2)*dpzp(i);   
    
    sumpzDeriv(1,i) =  dSIIxs(i); 
    sumpzDeriv(2,i) =  dSIIys(i); 
    sumpzDeriv(4,i) =  dpzxs(i); 
    sumpzDeriv(5,i) = dpzys(i) ; 
    sumpzDeriv(6,i) =  dpzp(i); 
    sumpzDeriv(7,i) =  dsumpzxs(i); 
    sumpzDeriv(8,i) =  dsumpzys(i); 
    sumpzDeriv(9,i) =  dsumpzp(i); 
    sumpzDeriv(10,i) =  dsum1pzxs(i); 
    sumpzDeriv(11,i) =  dsum1pzys(i); 
    sumpzDeriv(12,i) =  dsum1pzp(i); 
    sumpzDeriv(13,i) =  dsum2pzxs(i); 
    sumpzDeriv(14,i) =  dsum2pzys(i); 
    sumpzDeriv(15,i) =  dsum2pzp(i); 
    
    
  }// for(i=1;i<=SIIDIM;i++)
  
  log << MSG::DEBUG << " sumpzDeriv = "  << endreq;
  log << MSG::DEBUG << sumpzDeriv << endreq;
  
  
} 
















// This method builds the Te matrix with the derivatives of the 
// mother particle parameters 
// 

void ParticleStuffer::buildTeMatrix( int const SIIDIM , HepMatrix &sumhDeriv, 
                                     HepMatrix &sumpzDeriv, 
                                     HepMatrix &parDeriv, 
                                     HepVector &sumh, HepVector &sum, 
                                     double par1, double par2, double sumpz, 
                                     double sum1pz, double sum2pz, 
                                     HepMatrix &Te ){ 
  
  MsgStream log( msgSvc(), name() );
  
 
  log << MSG::DEBUG << " SIIDIM = " << SIIDIM << endreq;
  log << MSG::DEBUG << " sumhDeriv = " <<  endreq;
  log << MSG::DEBUG <<  sumhDeriv << endreq;
  log << MSG::DEBUG << " sumpzDeriv = " <<  endreq;
  log << MSG::DEBUG <<  sumpzDeriv << endreq;
  log << MSG::DEBUG << " parDeriv = " <<  endreq;
  log << MSG::DEBUG <<  parDeriv << endreq;
  log << MSG::DEBUG << " sumh = " <<  endreq;
  log << MSG::DEBUG <<  sumh << endreq;
  log << MSG::DEBUG << " sum = " <<  endreq;
  log << MSG::DEBUG <<  sum << endreq;
  log << MSG::DEBUG << " par1 = " << par1 << endreq;
  log << MSG::DEBUG << " par2 = " << par2 << endreq;
  log << MSG::DEBUG << " sumpz = " << sumpz << endreq;
  log << MSG::DEBUG << " sum1pz = " << sum1pz << endreq;
  log << MSG::DEBUG << " sum2pz = " << sum2pz << endreq;
  
  
  int i;
  
  HepVector dxx(SIIDIM,0)   ; 
  HepVector dxy(SIIDIM,0)   ; 
  HepVector dxxs(SIIDIM,0)   ; 
  HepVector dxys(SIIDIM,0)   ; 
  HepVector dxp(SIIDIM,0)   ; 
  HepVector dyx(SIIDIM,0)   ; 
  HepVector dyy(SIIDIM,0)  ; 
  HepVector dyxs(SIIDIM,0)   ; 
  HepVector dyys(SIIDIM,0)   ; 
  HepVector dyp(SIIDIM,0)   ; 
  HepVector dzx(SIIDIM,0)   ; 
  HepVector dzy(SIIDIM,0)  ; 
  HepVector dzxs(SIIDIM,0)   ; 
  HepVector dzys(SIIDIM,0)   ; 
  HepVector dzp(SIIDIM,0)   ; 
  HepVector dxsx(SIIDIM,0)   ; 
  HepVector dxsy(SIIDIM,0)   ; 
  HepVector dxsxs(SIIDIM,0)   ; 
  HepVector dxsys(SIIDIM,0)   ; 
  HepVector dxsp(SIIDIM,0)   ; 
  HepVector dysx(SIIDIM,0)   ; 
  HepVector dysy(SIIDIM,0)   ; 
  HepVector dysxs(SIIDIM,0)   ; 
  HepVector dysys(SIIDIM,0)   ; 
  HepVector dysp(SIIDIM,0)   ; 
  HepVector dpx(SIIDIM,0)   ; 
  HepVector dpy(SIIDIM,0)   ; 
  HepVector dpxs(SIIDIM,0)   ; 
  HepVector dpys(SIIDIM,0)   ; 
  HepVector dpp(SIIDIM,0)   ; 
  
  
  for(i=1;i<=SIIDIM;i++){  
    
    int index1 = 5*(i-1)+1; // xi index 
    int index2 = 5*(i-1)+2; // yi index 
    int index3 = 5*(i-1)+3; // xi' index 
    int index4 = 5*(i-1)+4; // yi' index 
    int index5 = 5*i;       // pi index  
    
    
    log << MSG::DEBUG << "i " << i << endreq;
    log << MSG::DEBUG << "index1 " << index1 << endreq;
    log << MSG::DEBUG << "index2 " << index2 << endreq;
    log << MSG::DEBUG << "index3 " << index3 << endreq;
    log << MSG::DEBUG << "index4 " << index4 << endreq;
    log << MSG::DEBUG << "index5 " << index5 << endreq;
    
    
    dxx(i) = (parDeriv(5,i) /par1); 
    
    dxy(i) = (parDeriv(6,i) /par1); 
    
    dxxs(i) = (parDeriv(7,i) /par1) - (par2/pow(par1,2))*parDeriv(3,i) ; 
    
    dxys(i) = (parDeriv(8,i) /par1) - (par2/pow(par1,2))*parDeriv(4,i) ; 
    
    dyx(i) = (1/sumh(5))*( sumhDeriv(9,i) - sumh(3)*parDeriv(5,i) /par1 -
                           sumh(6)* pow( sumh(5) 
                                         - (sumh(4)*sumh(6)/sumh(5)) , -1)*
                           (  sumhDeriv(5,i) - (sumh(4)/sumh(5))*sumhDeriv(9,i)
                              - ( parDeriv(5,i)/par1) *(sumh(2)
                                                        - (sumh(3)*sumh(4)/ 
                                                           sumh(5)) ) ) );
    
    dyy(i) = (1/sumh(5))*( sumhDeriv(10,i) - sumh(3)*parDeriv(6,i)/par1   - 
                           sumh(6)* pow( sumh(5) - 
                                         (sumh(4)*sumh(6)/sumh(5)) ,-1)*
                           ( sumhDeriv(6,i) - (sumh(4)/sumh(5))*
                             sumhDeriv(10,i)-   (parDeriv(6,i)/par1)*
                             ( sumh(2)  -(sumh(3)*sumh(4)/sumh(5)) ) ) );
    
    dyxs(i) =   - (1/pow(sumh(5),2))*sumhDeriv(31,i)*
      (sum(3)- sumh(3)*(par2/par1) - sumh(6)*
       pow( sumh(5) -  (sumh(4)*sumh(6)/sumh(5)) ,-1)*
       ( sum(2) - (sumh(4)/sumh(5))*sum(3) -
         (par2/par1)*(sumh(2) - sumh(4)*(sumh(3)/sumh(5)) )  ) ) +  
	    (1/sumh(5))*(sumhDeriv(11,i) - 
                   ( sumhDeriv(23,i)*(par2/par1) + sumh(3)*
                     (parDeriv(7,i)/par1 - parDeriv(3,i)* 
                      (par2/pow(par1,2)) ) ) - sumhDeriv(35,i)*
                   pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
                   ( sum(2) -   sum(3)*(sumh(4)/sumh(5)) -  (par2/par1)*
                     ( sumh(2) -  sumh(3)*
                       (sumh(4)/sumh(5)) )  ) - sumh(6)*
                   (- pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-2)*
                    (  sumhDeriv(31,i) - (sumh(4)/sumh(5))*sumhDeriv(35,i) + 
                       (sumh(4)/pow(sumh(5),2))*sumh(6)*sumhDeriv(31,i)  )*
                    ( sum(2) - sum(3)*(sumh(4)/sumh(5)) -  (par2/par1)*
                      ( sumh(2) -  sumh(3)*(sumh(4)/sumh(5)) )  ) +  
                    pow( sumh(5) -(sumh(4)*sumh(6)/sumh(5)) ,-1)*
                    ( -  (sumh(4)/sumh(5))*sumhDeriv(11,i) + 
                      (sumh(4)/pow(sumh(5),2))*sum(3)*sumhDeriv(31,i) - 
                      (  (par2/par1)*(  -(sumh(4)/sumh(5))*
                                        sumhDeriv(23,i) +   
                                        (sumh(4) /pow(sumh(5),2))*sumh(3)*
                                        sumhDeriv(31,i)  )  + 
                         ( sumh(2) - (sumh(4)/sumh(5))*sumh(3)  )*
                         ( parDeriv(7,i)/par1 - 
                           (par2/pow(par1,2))*parDeriv(3,i)  ))  ) ));
    
    dyys(i) =  - (1/pow(sumh(5),2))*sumhDeriv(32,i)*
      (sum(3)  - sumh(3)*(par2/par1)  
       -sumh(6)*pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
       ( sum(2) -   (sumh(4)/sumh(5))*sum(3) - (par2/par1)*
         (sumh(2) -  sumh(4)*(sumh(3)/sumh(5)) )  ))  +  (1/sumh(5))*
      (sumhDeriv(12,i) - 
       ( sumhDeriv(24,i)* (par2/par1) + sumh(3)*
         (parDeriv(8,i)/par1 -parDeriv(4,i)*
          (par2/pow(par1,2)) ) ) - sumhDeriv(36,i)*
       pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
       ( sum(2) -sum(3)*(sumh(4)/sumh(5)) - (par2/par1)*
         ( sumh(2) -  sumh(3)*(sumh(4)/sumh(5)) )  ) - sumh(6)*
       ( - pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-2)*
         ( sumhDeriv(32,i) -(sumh(4)/sumh(5))*
           sumhDeriv(36,i) + (sumh(4)/pow(sumh(5),2))*
           sumh(6)*sumhDeriv(32,i)  )
         *(  sum(2) - sum(3)*(sumh(4)/sumh(5)) - (par2/par1)*
             ( sumh(2) - sumh(3)*(sumh(4)/sumh(5)) )  )
         +  pow( sumh(5) -(sumh(4)*sumh(6)/sumh(5)) ,-1)*
         ( - (sumh(4)/sumh(5))*sumhDeriv(12,i)
           + (sumh(4)/pow(sumh(5),2))*sum(3)*sumhDeriv(32,i) -
           ( (par2/par1)*( -(sumh(4)/sumh(5))*sumhDeriv(24,i) +   
                           (sumh(4)/pow(sumh(5),2))*sumh(3)*
                           sumhDeriv(32,i)  )  + 
             ( sumh(2) - (sumh(4)/sumh(5))*
               sumh(3)  )*( parDeriv(8,i)/par1 -
                            (par2/pow(par1,2))*parDeriv(4,i)  ))  ) ));
    
    
    dzx(i) = pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
      (  sumhDeriv(5,i) - (sumh(4)*sumhDeriv(9,i)/sumh(5)) - 
         (parDeriv(5,i)/par1)*(sumh(2) -(sumh(3)*sumh(4)/sumh(5)))  );
    
    dzy(i) =   pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
      ( sumhDeriv(6,i) -   (sumh(4)*sumhDeriv(10,i)/sumh(5)) - 
        (parDeriv(6,i)/par1)*(sumh(2) -
                              (sumh(3)*sumh(4)/sumh(5))) ) ;
    
    dzxs(i) = - pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-2)*
      ( sumhDeriv(31,i) -   (sumh(4)*sumhDeriv(35,i)/sumh(5)) + 
        (sumh(4)/pow(sumh(5),2))*sumh(6)*sumhDeriv(31,i))*
      (  sum(2) - (sumh(4)*sum(3)/sumh(5)) - (par2/par1)*
         ( sumh(2) -(sumh(3)*sumh(4)/sumh(5)))  ) + 
      pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
      ( - (sumh(4)/sumh(5))*  sumhDeriv(11,i) + (sumh(4)/pow(sumh(5),2))*
        sum(3)*sumhDeriv(31,i) -  ( (par2/par1)*
                                    ( - (sumh(4)/sumh(5))*sumhDeriv(23,i) +  
                                      (sumh(4)/pow(sumh(5),2))*sumh(3)*
                                      sumhDeriv(31,i)  )
                                    + ( sumh(2) - (sumh(4)/sumh(5))*
                                        sumh(3) )*( (parDeriv(7,i)/par1) - 
                                                    (par2/pow(par1,2))*
                                                    parDeriv(3,i))  )  );   
    
    dzys(i) =  - pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-2)*
      ( sumhDeriv(32,i) -(sumh(4)*sumhDeriv(36,i)/sumh(5)) + 
        (sumh(4)/pow(sumh(5),2))*
        sumh(6)*sumhDeriv(32,i))* 
      (  sum(2) - (sumh(4)*sum(3)/sumh(5)) - (par2/par1)*
         ( sumh(2) -(sumh(3)*sumh(4)/sumh(5)))  ) + 
      pow( sumh(5) - (sumh(4)*sumh(6)/sumh(5)) ,-1)*
      ( - (sumh(4)/sumh(5))*sumhDeriv(12,i) + 
        (sumh(4)/pow(sumh(5),2))*sum(3)*sumhDeriv(32,i) - 
        ( (par2/par1)*( - (sumh(4)/sumh(5))*
                        sumhDeriv(24,i) +  (sumh(4)/pow(sumh(5),2))*sumh(3)*
                        sumhDeriv(32,i)  )
          + ( sumh(2) - (sumh(4)/sumh(5))*
              sumh(3) )*( (parDeriv(8,i)/par1) - (par2/pow(par1,2))*
                          parDeriv(4,i)))  );   
    
    dxsxs(i) = (sumpzDeriv(10,i)/sumpz) - (sum1pz/pow(sumpz,2))*
      sumpzDeriv(7,i)  ; 
    
    dxsys(i) = (sumpzDeriv(11,i)/sumpz) - (sum1pz/pow(sumpz,2))*
      sumpzDeriv(8,i)  ; 
    
    dxsp(i) = (sumpzDeriv(12,i)/sumpz) - (sum1pz/pow(sumpz,2))*
      sumpzDeriv(9,i)  ; 
    
    dysxs(i) = (sumpzDeriv(13,i)/sumpz) - (sum2pz/pow(sumpz,2))*
      sumpzDeriv(7,i)  ;
    
    dysys(i) = (sumpzDeriv(14,i)/sumpz) - (sum2pz/pow(sumpz,2))*
      sumpzDeriv(8,i)  ;
    
    dysp(i) = (sumpzDeriv(15,i)/sumpz) - (sum2pz/pow(sumpz,2))*
      sumpzDeriv(9,i)  ;
    
    dpxs(i) = ( sum1pz*sumpzDeriv(10,i) + sum2pz*sumpzDeriv(13,i) +
                sumpz*sumpzDeriv(7,i) )/sqrt( pow(sum1pz,2) + pow(sum2pz,2) +
                                              pow(sumpz,2) )  ;
    
    dpys(i) = ( sum1pz*sumpzDeriv(11,i) + sum2pz*sumpzDeriv(14,i) +
                sumpz*sumpzDeriv(8,i) )/sqrt( pow(sum1pz,2) + pow(sum2pz,2) +
                                              pow(sumpz,2) )  ;
    
    dpp(i) = ( sum1pz*sumpzDeriv(12,i) + sum2pz*sumpzDeriv(15,i) +
               sumpz*sumpzDeriv(9,i) )/sqrt( pow(sum1pz,2) + pow(sum2pz,2) +
                                             pow(sumpz,2) )  ;
    
    
    Te(1,index1) = dxx(i)  ;
    Te(1,index2) = dxy(i)  ;
    Te(1,index3) = dxxs(i)  ;
    Te(1,index4) = dxys(i)  ;
    Te(1,index5) = dxp(i)  ;
    Te(2,index1) = dyx(i)  ;
    Te(2,index2) = dyy(i)  ;
    Te(2,index3) = dyxs(i)  ;
    Te(2,index4) = dyys(i)  ;
    Te(2,index5) = dyp(i)  ;
    Te(3,index1) = dzx(i)  ;
    Te(3,index2) = dzy(i)  ;
    Te(3,index3) = dzxs(i)  ;
    Te(3,index4) = dzys(i)  ;
    Te(3,index5) = dzp(i)  ;
    Te(4,index1) = dxsx(i)  ;
    Te(4,index2) = dxsy(i)  ;
    Te(4,index3) = dxsxs(i)  ;
    Te(4,index4) = dxsys(i)  ;
    Te(4,index5) = dxsp(i)  ;
    Te(5,index1) = dysx(i)  ;
    Te(5,index2) = dysy(i)  ;
    Te(5,index3) = dysxs(i)  ;
    Te(5,index4) = dysys(i)  ;
    Te(5,index5) = dysp(i)  ;
    Te(6,index1) = dpx(i)  ;
    Te(6,index2) = dpy(i)  ;
    Te(6,index3) = dpxs(i)  ;
    Te(6,index4) = dpys(i)  ;
    Te(6,index5) = dpp(i)  ;
    
    
    log << MSG::DEBUG << " Te = "  << endreq;
    log << MSG::DEBUG << Te << endreq;
    
    
  }// for(i=1;i<=SIIDIM;i++) 
  
  
} 
















// This method builds the Ce matrix with the derivatives of daughter particles
// parameters and the e vector with the variables x, y, x',y' and p of the
// daughter particles.
// 

StatusCode ParticleStuffer::calcCeEPosErr( const Vertex& vtx,  
                                           HepSymMatrix &Ce, HepVector &e,  
                                           HepMatrix &posErrTemp ){ 
  
  MsgStream log( msgSvc(), name() );
  
  
  
  SmartRefVector<Particle>::const_iterator it;  	
  int embedded_pos = 1;
  int column = 1;
  
  Particle transParticle; 
  
  double zVtxPos = vtx.position().z();
  
  log << MSG::DEBUG << " zVtxPos = " << zVtxPos << endreq;
  
  for (it = vtx.products().begin(); it != vtx.products().end(); it++ ) {
    if ( (*it)->pointOnTrack().z() > 950.) { 
      log << MSG::DEBUG << "Position > 950 "  
          << (*it)->pointOnTrack().z() << endreq; 
    } 
    
    // dimension of the covariance matrix for one track ( = TRCOVDIM ) 
    int const TRCOVDIM  = 5;
    // Position where CeTemp is embedded in Ce.     
    HepVector    eTemp(TRCOVDIM,0);
    HepPoint3D newPoint;
    HepSymMatrix newpointErr;
    HepSymMatrix newSlpMomCorrErr;
    HepMatrix newPointSlpMomCorrErr;
    if (!(*it)->isResonance()){
      // Transport Particle parameters to the vertex position	  
      StatusCode sctrans = m_pTransporter->transport(*(*it), 
                                                     zVtxPos, 
                                                     transParticle); 
      if ( !sctrans.isSuccess() ) { 
        
        
        log << MSG::DEBUG << "Track extrapolation failed" << endreq; 
        return sctrans; 
      }			 
      
      newPoint =transParticle.pointOnTrack(); 
      newpointErr = transParticle.pointOnTrackErr(); 
      newSlpMomCorrErr = transParticle.slopesMomErr(); 
      newPointSlpMomCorrErr = transParticle.posSlopesCorr();
      
      eTemp(1) = newPoint.x(); 
      eTemp(2) = newPoint.y(); 
      
      eTemp(3) = transParticle.slopeX(); 
      eTemp(4) = transParticle.slopeY(); 
      
      // momenta  
      double px = transParticle.momentum().px(); 
      double py = transParticle.momentum().py(); 
      double pz = transParticle.momentum().pz(); 
      eTemp(5) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2)); 
      
    }
    else {
      newPoint = (*it)->pointOnTrack();
      newpointErr = (*it)->pointOnTrackErr();
      newSlpMomCorrErr = (*it)->slopesMomErr();
      newPointSlpMomCorrErr = (*it)->posSlopesCorr();
      
      eTemp(1) = newPoint.x();
      eTemp(2) = newPoint.y();
      
      eTemp(3) = transParticle.slopeX();
      eTemp(4) = transParticle.slopeY();
      
      double px = (*it)->momentum().px();
      double py = (*it)->momentum().py();
      double pz = (*it)->momentum().pz();
      eTemp(5) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
    }      
    log << MSG::DEBUG << " eTemp = "  << endreq;
    log << MSG::DEBUG << eTemp << endreq;     
    //embed the track parameters vector in the e vector 
    e.sub(embedded_pos,eTemp); 
    //      e(embedded_pos) = eTemp(1);
    //      e(embedded_pos + 1) = eTemp(2);
    //      e(embedded_pos + 2) = eTemp(3);
    //      e(embedded_pos + 3) = eTemp(4);
    //      e(embedded_pos + 4) = eTemp(5);
    
    log << MSG::DEBUG << " e = "  << endreq;
    log << MSG::DEBUG << e << endreq;
    
    posErrTemp(1,column) = newpointErr(1,1) ; 
    posErrTemp(2,column) = newpointErr(1,2); 
    posErrTemp(3,column) = newpointErr(2,2); 
    
    log << MSG::DEBUG << " posErrTemp = "  << endreq;
    log << MSG::DEBUG << posErrTemp << endreq;
    
    
    HepSymMatrix CeTemp(5,0);
    
    CeTemp(1,1)=newpointErr(1,1);             // x-x
    CeTemp(1,2)=newpointErr(2,1);             // x-y
    CeTemp(2,2)=newpointErr(2,2);             // y-y
    
    CeTemp(1,3)=newPointSlpMomCorrErr(1,1);   // x-sx
    CeTemp(1,4)=newPointSlpMomCorrErr(2,1);   // x-sy
    CeTemp(1,5)=newPointSlpMomCorrErr(3,1);   // x-p
    CeTemp(2,3)=newPointSlpMomCorrErr(1,2);   // y-sx
    CeTemp(2,4)=newPointSlpMomCorrErr(2,2);   // y-sy
    CeTemp(2,5)=newPointSlpMomCorrErr(3,2);   // y-p
    
    CeTemp(3,3)= newSlpMomCorrErr(1,1);       // sx-sx
    CeTemp(3,4)= newSlpMomCorrErr(2,1);       // sx-sy
    CeTemp(3,5)= newSlpMomCorrErr(3,1);       // sx-p
    CeTemp(4,4)= newSlpMomCorrErr(2,2);       // sy-sy
    CeTemp(4,5)= newSlpMomCorrErr(3,2);       // sy-p
    CeTemp(5,5)= newSlpMomCorrErr(3,3);       // p-p
    
    
    log << MSG::DEBUG << " CeTemp = "  << endreq;
    log << MSG::DEBUG << CeTemp << endreq;
    
    
    //embed the one track matrix in the Ce matrix
    Ce.sub(embedded_pos,CeTemp);
    
    
    
    
    log << MSG::DEBUG << " Ce = "  << endreq;
    log << MSG::DEBUG << Ce << endreq;
    
    
    //update position for next embedding
    embedded_pos = embedded_pos + 5;
    column = column + 1; 	 
    
    log << MSG::DEBUG << " embedded_pos = " << embedded_pos << endreq;
    log << MSG::DEBUG << " column = " << column << endreq;
    
  }//for(it=vtx.products)			 
  
  return StatusCode::SUCCESS;
  
} 





