// $Id: LagrangeMassVertexFitter.cpp,v 1.1, 2002/03/08, modified from gcorti
// Exps program $

// Include files from Gaudi
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

// local file
#include "LagrangeMassVertexFitter.h"
#include "DaVinciTools/IParticleTransporter.h"

//--------------------------------------------------------------------
//
//  ClassName  : LagrangeMassVertexFitter
// 
//  Description: Performs a mass constrained fit for two, three or four 
//                 tracks using the Lagrange multipliers technique.
//               It includes the mass constrained fit for 2, 3 or 4 
//                 tracks without ressonance, for 3 or 4 tracks with one
//                 ressonance and for 4 tracks with two ressonances.   
//               Based on Fortran code by J. Saborido
//                 ref: LHCb Phys Note 98-051 
//                           
//  @Author     : Joao de Mello
//  @Author     : Claudia Nunes 
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<LagrangeMassVertexFitter> s_factory;
const IToolFactory& LagrangeMassVertexFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
LagrangeMassVertexFitter::LagrangeMassVertexFitter(const std::string& type, 
                                                   const std::string& name, 
                                                   const IInterface* parent) 
  : AlgTool( type, name, parent ),
    m_ppSvc(0),
    m_pVertexUnconst(0),
    m_pTransporter(0),
    m_transporterType("CombinedTransporter") {
  
  declareInterface<IMassVertexFitter>(this);

  declareProperty("Transporter", m_transporterType);
  declareProperty( "WidthCut", m_widthCut = 1. );  
}

//==================================================================
// Initialize
//==================================================================
StatusCode LagrangeMassVertexFitter::initialize() {
  MsgStream log( msgSvc(), name() );
  
  // This tool needs to use internally the ParticlePropertySvc to retrieve the
  // mass to be used
  StatusCode sc = StatusCode::FAILURE;
  sc = service( "ParticlePropertySvc", m_ppSvc );
  if( sc.isFailure ()) {
    log << MSG::FATAL << "ParticlePropertySvc Not Found" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = toolSvc()->retrieveTool(m_transporterType, 
                               m_pTransporter, this);
  if(sc.isFailure()) {
    log << MSG::FATAL << " Unable to retrieve ParticleTransporter tool" ;
    return sc;
  }
 
  StatusCode sc1 = toolSvc()->retrieveTool("UnconstVertexFitter",
                                           m_pVertexUnconst,this);
  if(sc1.isFailure()) {
    log << MSG::FATAL << "Unable to retrieve Uncosntr. Fitter tool"
        << endreq ;
    return sc1;
  }
  
  
  return StatusCode::SUCCESS;
  
}

//=========================================================================
// Finalize
//=========================================================================
StatusCode LagrangeMassVertexFitter::finalize() {
   
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Release tools" << endreq;

  if( m_pTransporter   ) toolSvc()->releaseTool( m_pTransporter );
  if( m_pVertexUnconst ) toolSvc()->releaseTool( m_pVertexUnconst );

  return StatusCode::SUCCESS;
}


//==================================================================
// Perform mass constrained vertex fit (vector of particles).
// The procedure is implemented only for two, three and four particles
// without ressonance.
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithNoRess(const std::string& 
                                                   motherName,
                                                   const ParticleVector& pList,
                                                   Vertex& constrVtx,
                                                   Particle& motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithNoRess " << endreq;
  
  // check the size of the input particle vector.
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  log << MSG::INFO << "Number of tracks in fitWithNoRess = " 
      << ntracks << endreq;
  
  if (( ntracks != 2 ) && ( ntracks != 3 ) && ( ntracks != 4 )) {
    log << MSG::INFO << "Number of tracks must be 2, 3 or 4  " << endreq;
    return StatusCode::FAILURE;
  }
  
  
  Vertex uncVertex;
  StatusCode scUncVertex = m_pVertexUnconst->fitVertex(pList,uncVertex);
  if( scUncVertex.isFailure() ) {
    log << MSG::FATAL << " unconstrained vertex failure " << endreq;
  }
  
  HepPoint3D vertexpos =  uncVertex.position();
  double zcer = vertexpos.z();
  
  log << MSG::DEBUG << "vertex unconst position" <<vertexpos.x() 
      << "  " << vertexpos.y()  <<  " "   << vertexpos.z() << endreq;  
  
  // Mass of the mother particle ( = massConstr )
  ParticleProperty*  partProp = m_ppSvc->find( motherName );
  double  massConstr = (*partProp).mass();
  
  // covariance matrix of the mother particle
  HepSymMatrix Cx(6,0);
  
  // dimension of the covariance matrix for one track ( = TRCOVDIM )
  int const TRCOVDIM  = 5;
  
  // maximum number of tracks ( = NMAXTRK ).
  int const NMAXTRK   = 4;
  
  // number of iterations ( = NMAXITERA ).
  int const NMAXITERA = 20 ;
  
  
  // dimension of the covariance matrix for 2, 3 or 4 tracks.
  int dimCe = ntracks*TRCOVDIM;
  
  // mass of the daughter particles
  HepVector mass(NMAXTRK);
  
  // prepare the covariance matrix Ce and the vector e for 2, 3 or 4 tracks.
  HepSymMatrix Ce(dimCe, 0);
  HepVector e(dimCe,0);
  
  // Position where CeTemp is embedded in Ce.
  int embedded_pos = 1;
  
  HepSymMatrix CeTemp(TRCOVDIM,0);
  HepVector    eTemp(TRCOVDIM,0); 
  
  int index = 1;
  
  ParticleVector::const_iterator iterP;
  
  for(iterP = pList.begin(); iterP != pList.end(); 
      iterP++) {
    
    // extrapolate particle to zEstimate 
    Particle transParticle;
    StatusCode sctrans = m_pTransporter->transport(iterP,
                                                   zcer,
                                                   transParticle);
    
    
    if ( !sctrans.isSuccess() ) {
      log << MSG::DEBUG << "Track extrapolation failed" << endreq;
      return sctrans;
    }
    HepPoint3D newPoint =transParticle.pointOnTrack();
    HepSymMatrix newpointErr = transParticle.pointOnTrackErr();
    HepSymMatrix newSlpMomCorrErr = transParticle.slopesMomErr();
    HepMatrix newPointSlpMomCorrErr = transParticle.posSlopesCorr();
    
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

    eTemp(1) = newPoint.x();
    eTemp(2) = newPoint.y();
  
    eTemp(3) = transParticle.slopeX();
    eTemp(4) = transParticle.slopeY();
    // momenta 
 
    double px = transParticle.momentum().px();
    double py = transParticle.momentum().py();
    double pz = transParticle.momentum().pz();
    eTemp(5) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
    
    // get the mass of this daughter particle
    mass(index) = (*iterP)->mass();
    
    //embed the one track matrix in the Ce matrix
    Ce.sub(embedded_pos,CeTemp);
    
    //embed the track parameters vector in the e vector
    e.sub(embedded_pos,eTemp);
    
    //update position for next embedding
    embedded_pos = embedded_pos + TRCOVDIM;
    
    index = index + 1; 
   
  }
  
  
  // copy parameter vector
  HepVector e0(e); 
  
  
  // cut on the difference of the constrMass and constrCalcMass
  double DIFFMASSCUT = 1.0e-6;
  // diffMass, diffZVer and DIFFZVERCUT
  double DIFFZVERCUT = 1.0e-6; 
  double diffMass = 0;
  double diffZver = 0;
  
  // Calculated constrained mass from the daughter particles. 
  double massConstrCalc;
  
  //Chisquare of the fit.
  double  chis;
  
  // Parameters vector of the resulting track.
  HepVector tpf(6);
  
  bool final = false;
  int  icount= 0; 
  while (!final && (icount < NMAXITERA)) {
    
    StatusCode sc_iter = itera(e, Ce, mass, massConstr, dimCe, final);
    if ( !sc_iter.isSuccess() ) {
      log << MSG::DEBUG << " itera failed  " << endreq;
      return StatusCode::FAILURE;  
    }
    
    
    // check that momenta are physical
    if (e(5)*e(10) < 0.0 ){
      log << MSG::DEBUG << "momenta out of range " << endreq;
      return StatusCode::FAILURE;
    } 
    if ((( ntracks == 3 )|| (ntracks == 4)) && ( e(15) < 0.0 )){
      log << MSG::DEBUG << "momenta out of range " << endreq;
      return StatusCode::FAILURE; 
    }
    
    // get constraint conditions
    StatusCode sc_evalu=  evalu(e, mass, massConstrCalc, diffZver);
    if ( !sc_evalu.isSuccess() ) {
      log << MSG::DEBUG << "constraints calc failed " << endreq;
      return StatusCode::FAILURE;
    }
    
    
    // finish if constraint are satisfied
    diffMass = fabs((massConstr-massConstrCalc)/massConstr);
    
    icount = icount + 1;
    
    if ( (diffZver < DIFFZVERCUT) && (diffMass < DIFFMASSCUT)){
      final=true;
    }
  }
  
  // if not converged, return
  if(!final) {
    log << MSG::DEBUG << " no convergence  " << endreq;
    log << MSG::DEBUG << " massConstrCalc = " << massConstrCalc << endreq;
    log << MSG::DEBUG << " diffMass = " << diffMass << endreq;
    log << MSG::DEBUG << " icount = " << icount << endreq;
    return StatusCode::FAILURE; 
  }
  
  // later check the diffZver
  
  // Minimum found. Get the chisq of the fit.
  
  StatusCode sc_chisq = chisq( e0, e, Ce, chis);
  if ( !sc_chisq.isSuccess() ) {
    log << MSG::DEBUG << "chisq  calc failed " << endreq;
    return StatusCode::FAILURE;  
  }
  
  
  // update the covariance matrix for old parameters (here final=true)
  StatusCode sc_itera =  itera(e,Ce,mass,massConstr,dimCe,final);
  if ( !sc_itera.isSuccess() ) {
    log << MSG::DEBUG << " itera failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  
  // find the covariance matrix of the mother particle
  StatusCode sc_nwcov = nwcov(e, Ce, zcer, tpf, Cx, dimCe);
  if ( !sc_nwcov.isSuccess() ) {
    log << MSG::DEBUG << " nwcov failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  log << MSG::DEBUG << " lagrange: chis = " << chis << endreq;
  log << MSG::DEBUG << " lagrange: mass  = " << massConstrCalc << endreq;
  
  // check the covariance matrix for obvious problems
  //   diagonal positive
  int i;
  for (i=1;i<7;i++){
    if (Cx(i,i) < 0.0){
      log << MSG::DEBUG << "Prob with cov. matr: Cx ("<< i << "," 
          << i << ") = " << Cx (i,i) << endreq;
      return StatusCode::FAILURE;
    }
  }
  //   correlation rho(i,j) between -1 and 1
  for (i=1; i<7; i++){
    for (int j=1; j<7&&j>i; j++){
      if ( fabs (Cx(i,j))/sqrt(Cx(i,i)*Cx(j,j)) > 1.0) {
        log << MSG::DEBUG << "Prob with cov. matr: Cx ("<< i << "," 
            << j << ") = "<< Cx(i,j) << endreq;
        return StatusCode::FAILURE;         
      }
    }
  }
  
  // fill the vertex object
  //    position
  constrVtx.setPosition(HepPoint3D(tpf(1),tpf(2),tpf(3))); 
  //    position error
  HepSymMatrix posErr=Cx.sub(1,3);
  constrVtx.setPositionErr(posErr);
  //    chisq, degrees of freedom
  constrVtx.setChi2(chis);
  constrVtx.setNDoF(ntracks);
  //    decay with constrained mass
  constrVtx.setType(Vertex::DecayWithMass);
  //    particles that compose the vertex
  for(iterP = pList.begin(); iterP != pList.end(); 
      iterP++) {
    constrVtx.addToProducts(*iterP);
  }
  
  
  // calculate mother particle momentum
  HepLorentzVector motherHepVec;
  motherHepVec.setPz( tpf(6)/sqrt(pow(tpf(4),2)+pow(tpf(5),2)+1.0));
  motherHepVec.setPx( tpf(4)*motherHepVec.pz());
  motherHepVec.setPy( tpf(5)*motherHepVec.pz());
  motherHepVec.setE( sqrt(pow(massConstr,2)+pow(motherHepVec.px(),2)+ 
                          pow(motherHepVec.py(),2) +
                          pow(motherHepVec.pz(),2)));
  
  //fill the Mother particle
  //   momentum
  motherParticle.setMomentum(motherHepVec);
  //   point on track
  motherParticle.setPointOnTrack(HepPoint3D(tpf(1),tpf(2),tpf(3)));
  //   point on track error (same as error vertex pos error)
  motherParticle.setPointOnTrackErr(posErr);
  //   error on track slopes and prop to momentum
  HepSymMatrix m_slpMomCorrelationError=Cx.sub(4,6);
  motherParticle.setSlopesMomErr(m_slpMomCorrelationError);
  
  
  
  //   corr err slopes point 
  //     check this: I would like to do Cx.sub(4,6,1,3) but
  //     Cx is simetric, this only work for HepMatrix??
  HepMatrix CxCopy=Cx;
  HepMatrix m_posSlpMomCorrelationError=CxCopy.sub(4,6,1,3);
  motherParticle.setPosSlopesCorr(m_posSlpMomCorrelationError);
  
  
  //   particle id  
  motherParticle.setParticleID((*partProp).jetsetID());
  //   confidence level
  motherParticle.setConfLevel(1.);
  //   mass
  motherParticle.setMass((*partProp).mass());
  //    massErr??
  //    check ressonance
  
  // hbar in MeV s
  // float hbar = 6.58211889*pow(10,-22);
  
  //  float motherParticleWidth = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
//  if((motherParticleWidth < 1) && ((*partProp).lifetime()*pow(10,-9) < 
 //                                   pow(10,-15))) 
  if((*partProp).lifetime()*pow(10,-9) <
                                    pow(10,-15))

    motherParticle.setIsResonance(true);
  
  
  motherParticle.setEndVertex(&constrVtx);
  
  
  return StatusCode::SUCCESS;
}













//==================================================================
//             Perform mass constrained vertex fit 
//                          ( 2 tracks )
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithMass(const std::string& 
                                                 motherName,
                                                 Particle& particle1, 
                                                 Particle& particle2,
                                                 Vertex& constrVtx,
                                                 Particle& motherParticle )
{
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  
  return fitWithMass(motherName, pList, constrVtx, motherParticle);
  
}

//==================================================================
//            Perform mass constrained vertex fit 
//                        ( 3 tracks )
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithMass( const std::string& 
                                                  motherName,
                                                  Particle& particle1, 
                                                  Particle& particle2, 
                                                  Particle& particle3,
                                                  Vertex& constrVtx,
                                                  Particle& motherParticle )
{
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  pList.push_back(&particle3);
  
  return fitWithMass(motherName, pList, constrVtx, motherParticle);
  
}

//==================================================================
//            Perform mass constrained vertex fit 
//                        ( 4 tracks )
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithMass( const std::string& 
                                                  motherName,
                                                  Particle& particle1, 
                                                  Particle& particle2, 
                                                  Particle& particle3,
                                                  Particle& particle4,
                                                  Vertex& constrVtx,
                                                  Particle& motherParticle )
{
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  pList.push_back(&particle3);
  pList.push_back(&particle4);
  
  return fitWithMass(motherName, pList, constrVtx, motherParticle);
  
}

//==================================================================
// Perform mass constrained vertex fit (vector of particles).
// The procedure is implemented only for two and three particles
// with ressonances.
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithMass(const std::string& 
                                                 motherName,
                                                 const ParticleVector& 
                                                 pList,
                                                 Vertex& constrVtx,
                                                 Particle& motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << " lagrange fitter starting - fitWithMass " << endreq;
  
  // check the size of the input particle vector.
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  
  log << MSG::INFO << "Number of tracks in fitWithMass = " 
      << ntracks << endreq;
  
  if (( ntracks != 2 ) && ( ntracks != 3 ) && ( ntracks != 4 )) {
    log << MSG::INFO << "Number of tracks must be 2, 3 or 4  " << endreq;
    return StatusCode::FAILURE;
  }
  
  int numbRessTwoTracks = 0;
  int numbRessThreeTracks = 0;
  int numbRessFourTracks = 0;
  bool Part1TwoTracks = false;
  bool Part2TwoTracks = false;
  bool Part1ThreeTracks = false;
  bool Part2ThreeTracks = false;
  bool Part3ThreeTracks = false;
  bool Part1FourTracks = false;
  bool Part2FourTracks = false;
  bool Part3FourTracks = false;
  bool Part4FourTracks = false;
  
  Particle* part1 = pList[0];
  Particle* part2 = pList[1];
  Particle* part3 = pList[2];
  Particle* part4 = pList[3];


  const ParticleID& part1ID = part1->particleID().pid();
  int part1StdHepID = part1ID.pid();
  
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(part1StdHepID  );
  
  // hbar in MeV s
  float hbar = 6.58211889*pow(10,-22);
  
  float part1WidthHBar = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if( (part1->isResonance()) && (part1WidthHBar > m_widthCut)) 
    part1->setIsResonance(false);
  
  const ParticleID& part2ID = part2->particleID().pid();
  int part2StdHepID = part2ID.pid();
  
  partProp = m_ppSvc->findByStdHepID(part2StdHepID  );
  
  float part2WidthHBar = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if( (part2->isResonance()) && (part2WidthHBar > m_widthCut)) 
    part2->setIsResonance(false);
  
  
  if(ntracks == 2){
    
    
    if(part1->isResonance()){
      numbRessTwoTracks++;
      Part1TwoTracks = true;
    }//if(part1->isResonance()) 
    
    
    if(part2->isResonance()){
      numbRessTwoTracks++;
      Part2TwoTracks = true;
    }//if(part2->isResonance())
    
    
    if ( (numbRessTwoTracks != 0) && (numbRessTwoTracks != 1) && 
         (numbRessTwoTracks != 2)  ) {    
      log << MSG::INFO << "Number of ressonances must be 0, 1 or 2  " 
          << endreq;      
      return StatusCode::FAILURE;
    } 
    
    SmartRefVector<Particle>::iterator itPart1;
    SmartRefVector<Particle>::iterator itPart2; 
    ParticleVector p3Tracks;
    ParticleVector pTracksPart1Daughters;
    ParticleVector pTracksPart2Daughters;
    double subMass1;
    double subMass2; 
    
    if(numbRessTwoTracks == 0){
      
      return fitWithNoRess(motherName, pList, constrVtx, motherParticle );
      
    }//if(numbRessTwoTracks == 0)    
    
    if((numbRessTwoTracks == 1) && Part1TwoTracks){
      
      
      for ( itPart1 = part1->endVertex()->products().begin();
            itPart1 != part1->endVertex()->products().end(); itPart1++ ){
        
        p3Tracks.push_back(*itPart1);
        pTracksPart1Daughters.push_back(*itPart1); 
        subMass1 = part1->mass();
        
      }//for itPart1
      
      p3Tracks.push_back(part2);
      
      
      return fitWithOneSubMass(motherName, subMass1, p3Tracks, constrVtx, 
                               motherParticle); 
      
    }//if((numbRessTwoTracks == 1) && Part1TwoTracks)
    
    if((numbRessTwoTracks == 1) && Part2TwoTracks){
      
      
      for ( itPart2 = part2->endVertex()->products().begin();
            itPart2 != part2->endVertex()->products().end(); itPart2++ ){
        
        p3Tracks.push_back(*itPart2);
        pTracksPart2Daughters.push_back(*itPart2); 
        subMass2 = part2->mass();
      }//for itPart2
      
      p3Tracks.push_back(part1);
      
      
      return fitWithOneSubMass(motherName, subMass2, p3Tracks, constrVtx, 
                               motherParticle); 
      
    }//if((numbRessTwoTracks == 1) && Part2TwoTracks)
    
    if(numbRessTwoTracks == 2){
      
      for ( itPart1 = part1->endVertex()->products().begin();
            itPart1 != part1->endVertex()->products().end(); itPart1++ ){
        
        p3Tracks.push_back(*itPart1);
        pTracksPart1Daughters.push_back(*itPart1);
        subMass1 = part1->mass();
        
      }//for itPart1
      
      for ( itPart2 = part2->endVertex()->products().begin();
            itPart2 != part2->endVertex()->products().end(); itPart2++ ){
        
        p3Tracks.push_back(*itPart2);
        pTracksPart2Daughters.push_back(*itPart2);
        subMass2 = part2->mass();
        
      }//for itPart2
      
      return fitWithTwoSubMass(motherName, subMass1, subMass2, p3Tracks, 
                               constrVtx, motherParticle); 
      
    }//if(numbRessTwoTracks == 2)
    
  }//if(ntracks == 2)
  
  
  if(ntracks == 3){
    
    const ParticleID& part3ID = part3->particleID().pid();
    int part3StdHepID = part3ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part3StdHepID  );
    
    float part3WidthHBar = hbar/(pow(10,-9)*((*partProp).lifetime()));
    
    if( (part3->isResonance()) && (part3WidthHBar > m_widthCut)) 
      part3->setIsResonance(false);
    
    if(part1->isResonance()){
      numbRessThreeTracks++;
      Part1ThreeTracks++;
    }//if(part1->isResonance()) 
    
    if(part2->isResonance()){
      numbRessThreeTracks++;
      Part2ThreeTracks++;
    }//if(part2->isResonance())
    
    if(part3->isResonance()){
      numbRessThreeTracks++;
      Part3ThreeTracks++;
    }//if(part3->isResonance())
    
    if ( (numbRessThreeTracks != 0) && (numbRessThreeTracks != 1)  ) {
      log << MSG::INFO << "Number of ressonances must be 0 or 1  " << endreq;
      return StatusCode::FAILURE;
    }//if( (numbRessThreeTracks != 0) && (numbRessThreeTracks != 1))  
    
    SmartRefVector<Particle>::iterator itPart1;
    SmartRefVector<Particle>::iterator itPart2;
    SmartRefVector<Particle>::iterator itPart3;
    ParticleVector p3Tracks;
    ParticleVector pTracksPart1Daughters;
    ParticleVector pTracksPart2Daughters;
    ParticleVector pTracksPart3Daughters;
    double subMass1;
    double subMass2;
    double subMass3;
    
    
    if(numbRessThreeTracks == 0){
      
      return fitWithNoRess(motherName, pList, constrVtx, motherParticle );
      
    }//if(numbRessThreeTracks == 0)
    
    if((numbRessThreeTracks == 1) && Part1ThreeTracks){
      
      for ( itPart1 = part1->endVertex()->products().begin();
            itPart1 != part1->endVertex()->products().end(); itPart1++ ){
        
        p3Tracks.push_back(*itPart1);
        pTracksPart1Daughters.push_back(*itPart1); 
        subMass1 = part1->mass();
        
      }//for itPart1
      
      p3Tracks.push_back(part2);
      
      p3Tracks.push_back(part3);
      
      return fitWithOneSubMass(motherName,subMass1, p3Tracks, constrVtx, 
                               motherParticle); 
      
    }//if((numbRessThreeTracks == 1) && Part1ThreeTracks)
    
    if((numbRessThreeTracks == 1) && Part2ThreeTracks){
      
      for ( itPart2 = part2->endVertex()->products().begin();
            itPart2 != part2->endVertex()->products().end(); itPart2++ ){
        
        p3Tracks.push_back(*itPart2);
        pTracksPart2Daughters.push_back(*itPart2);
        subMass2 = part2->mass();
        
      }//for itPart2
      
      p3Tracks.push_back(part1);
      
      p3Tracks.push_back(part3);
      
      return fitWithOneSubMass(motherName, subMass2, p3Tracks, constrVtx, 
                               motherParticle); 
      
    }//if((numbRessTwoTracks == 1) && Part2ThreeTracks)
    
    if((numbRessThreeTracks == 1) && Part3ThreeTracks){
      
      for ( itPart3 = part3->endVertex()->products().begin();
            itPart3 != part3->endVertex()->products().end(); itPart3++ ){
        
        p3Tracks.push_back(*itPart3);
        pTracksPart3Daughters.push_back(*itPart3);
        subMass3 = part3->mass();
        
      }//for itPart3
      
      p3Tracks.push_back(part2);
      
      p3Tracks.push_back(part1);
      
      return fitWithOneSubMass(motherName, subMass3, p3Tracks, constrVtx, 
                               motherParticle); 
      
    }//if((numbRessThreeTracks == 1) && Part3ThreeTracks)
    
  }//if(ntracks == 3)
  
  if(ntracks == 4){
    
    const ParticleID& part3ID = part3->particleID().pid();
    int part3StdHepID = part3ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part3StdHepID  );
    
    float part3Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
    if( (part3->isResonance()) && (part3Width > m_widthCut)) 
      part3->setIsResonance(false);
    
    const ParticleID& part4ID = part4->particleID().pid();
    int part4StdHepID = part4ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part4StdHepID  );
    
    float part4Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
    
    if( (part4->isResonance()) && (part4Width > m_widthCut)) 
      part4->setIsResonance(false);
    
    if(part1->isResonance()){
      numbRessFourTracks++;
      Part1FourTracks++;
    }//if(part1->isResonance()) 
    
    if(part2->isResonance()){
      numbRessFourTracks++;
      Part2FourTracks++;
    }//if(part2->isResonance())
    
    if(part3->isResonance()){
      numbRessFourTracks++;
      Part3FourTracks++;
    }//if(part3->isResonance())
    
    if(part4->isResonance()){
      numbRessFourTracks++;
      Part4FourTracks++;
    }//if(part4->isResonance())
    
    if (numbRessFourTracks != 0) {
      log << MSG::INFO << "Number of ressonances must be 0  " << endreq;
      return StatusCode::FAILURE;
    }//if(numbRessThreeTracks != 0)  
    
    if(numbRessFourTracks == 0){
      
      return   fitWithNoRess(motherName, pList, constrVtx, motherParticle );	
      
    }//if(numbRessFourTracks == 0)
    
  }//if(ntracks == 4)
  
  return StatusCode::SUCCESS; 
  
  
}













//==================================================================
// Perform mass constrained vertex fit (vector of particles).
// The procedure is implemented for three or four tracks
// with one ressonance.
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithOneSubMass(const std::string& 
                                                       motherName,
                                                       double subMass,
                                                       const ParticleVector& 
                                                       pList,
                                                       Vertex& constrVtx,
                                                       Particle& 
                                                       motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithOneSubMass " 
      << endreq;
  
  // check the size of the input particle vector.
  
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  
  log << MSG::DEBUG << "Number of tracks in fitWithOneSubMass = " 
      << ntracks << endreq;
  
  if (( ntracks != 3 ) && ( ntracks != 4 )) {
    log << MSG::INFO << "Number of tracks must be 3 or 4  " << endreq;
    return StatusCode::FAILURE;
  }
  
  Vertex uncVertex;
  StatusCode scUncVertex = m_pVertexUnconst->fitVertex(pList,uncVertex);
  if( scUncVertex.isFailure() ) {
    log << MSG::FATAL << " unconstrained vertex failure " << endreq;
  }
  
  HepPoint3D vertexpos =  uncVertex.position();
  double zcer = vertexpos.z();
  
  log << MSG::DEBUG << "vertex unconst position" << vertexpos.x() 
      << " , " << vertexpos.y()  <<  " , "   << vertexpos.z() << endreq;  
  
  // Mass of the mother particle ( = massConstr )
  ParticleProperty*  partProp = m_ppSvc->find( motherName );
  double  massConstr = (*partProp).mass();
  
  // Mass of the sub particle - ressonance - ( = subMassConstr )
  double  subMassConstr = subMass;
  
  // covariance matrix of the mother particle
  HepSymMatrix Cx(6,0);
  
  // dimension of the covariance matrix for one track ( = TRCOVDIM )
  int const TRCOVDIM  = 5;
  
  // maximum number of tracks ( = NMAXTRK ).
  int const NMAXTRK   = 4;
  
  // number of iterations ( = NMAXITERA ).
  int const NMAXITERA = 20 ;
  
  // dimension of the covariance matrix for 3 or 4 tracks.
  int dimCe = ntracks*TRCOVDIM;
  
  // mass of the daughter particles
  HepVector mass(NMAXTRK);
  
  // prepare the covariance matrix Ce and the vector e for 3 or 4 tracks.
  HepSymMatrix Ce(dimCe, 0);
  HepVector e(dimCe,0);
  
  // Position where CeTemp is embedded in Ce.
  int embedded_pos = 1;
  
  HepSymMatrix CeTemp(TRCOVDIM,0);
  HepVector    eTemp(TRCOVDIM,0); 
  
  int index = 1;
  
  ParticleVector::const_iterator iterP;
  
  for(iterP = pList.begin(); iterP != pList.end(); 
      iterP++) {
    
    // extrapolate particle to zEstimate 
    Particle transParticle;
    StatusCode sctrans = m_pTransporter->transport(iterP,
                                                   zcer,
                                                   transParticle);
    
    
    if ( !sctrans.isSuccess() ) {
      log << MSG::DEBUG << "Track extrapolation failed" << endreq;
      return sctrans;
    }
    HepPoint3D newPoint =transParticle.pointOnTrack();
    HepSymMatrix newpointErr = transParticle.pointOnTrackErr();
    HepSymMatrix newSlpMomCorrErr = transParticle.slopesMomErr();
    HepMatrix newPointSlpMomCorrErr = transParticle.posSlopesCorr();
    
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
    
    eTemp(1) = newPoint.x();
    eTemp(2) = newPoint.y();
    
    eTemp(3) = transParticle.slopeX();
    eTemp(4) = transParticle.slopeY();
    // momenta 
    double px = transParticle.momentum().px();
    double py = transParticle.momentum().py();
    double pz = transParticle.momentum().pz();
    eTemp(5) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
    
    // get the mass of this daughter particle
    mass(index) = (*iterP)->mass();
    
    //embed the one track matrix in the Ce matrix
    Ce.sub(embedded_pos,CeTemp);
    
    //embed the track parameters vector in the e vector
    e.sub(embedded_pos,eTemp);
    
    //update position for next embedding
    embedded_pos = embedded_pos + TRCOVDIM;
    
    index = index + 1; 
    
  }
  
  // copy parameter vector
  HepVector e0(e); 
  
  // cut on the difference of the constrMass and constrCalcMass
  double DIFFMASSCUT = 1.0e-6;
  // diffMass, diffZVer and DIFFZVERCUT
  double DIFFZVERCUT = 1.0e-6; 
  double diffMass = 0;
  double diffZver = 0;
  
  
  // Calculated constrained mass from the daughter particles. 
  double massConstrCalc;
  
  //Chisquare of the fit.
  double  chis;
  
  // Parameters vector of the resulting track.
  HepVector tpf(6);
  
  bool final = false;
  int  icount= 0; 
  while (!final && (icount < NMAXITERA)) {
    
    StatusCode sc_iter = itera(e, Ce, mass, massConstr, subMassConstr, dimCe, 
                               final);
    if ( !sc_iter.isSuccess() ) {
      log << MSG::DEBUG << " itera failed  " << endreq;
      return StatusCode::FAILURE;  
    }
    
    
    // check that momenta are physical
    if (e(5)*e(10) < 0.0 ){
      log << MSG::DEBUG << "momenta out of range " << endreq;
      return StatusCode::FAILURE;
    } 
    
    if ((( ntracks == 3 )|| (ntracks == 4)) && ( e(15) < 0.0 )){
      log << MSG::DEBUG << "momenta out of range " << endreq;
      return StatusCode::FAILURE; 
    }
    
    // get constraint conditions
    
    StatusCode sc_evalu=  evalu(e, mass, subMassConstr, massConstrCalc, 
                                diffZver);
    if ( !sc_evalu.isSuccess() ) {
      log << MSG::DEBUG << "constraints calc failed " << endreq;
      return StatusCode::FAILURE;
    }
    
    // finish if constraint are satisfied
    diffMass = fabs((massConstr-massConstrCalc)/massConstr);
    
    icount = icount + 1;
    
    if ( (diffZver < DIFFZVERCUT) && (diffMass < DIFFMASSCUT)){
      final=true;
    }
  }
  
  // if not converged, return
  if(!final) {
    log << MSG::DEBUG << " no convergence  " << endreq;
    log << MSG::DEBUG << " massConstrCalc = " << massConstrCalc << endreq;
    log << MSG::DEBUG << " diffMass = " << diffMass << endreq;
    log << MSG::DEBUG << " icount = " << icount << endreq;
    return StatusCode::FAILURE; 
  }
  
  // later check the diffZver
  
  // Minimum found. Get the chisq of the fit.
  
  StatusCode sc_chisq = chisq( e0, e, Ce, chis);
  if ( !sc_chisq.isSuccess() ) {
    log << MSG::DEBUG << "chisq  calc failed " << endreq;
    return StatusCode::FAILURE;  
  }
  
  
  // update the covariance matrix for old parameters (here final=true)
  StatusCode sc_itera =  itera(e,Ce,mass,massConstr,subMassConstr,dimCe,final);
  if ( !sc_itera.isSuccess() ) {
    log << MSG::DEBUG << " itera failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  
  // find the covariance matrix of the mother particle
  StatusCode sc_nwcov = nwcov(e, Ce, zcer, tpf, Cx, dimCe);
  if ( !sc_nwcov.isSuccess() ) {
    log << MSG::DEBUG << " nwcov failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  log << MSG::DEBUG << " lagrange: chis = " << chis << endreq;
  log << MSG::DEBUG << " lagrange: mass  = " << massConstrCalc << endreq;
  
  // check the covariance matrix for obvious problems
  //   diagonal positive
  int i;
  for (i=1;i<7;i++){
    if (Cx(i,i) < 0.0){
      log << MSG::DEBUG << "Prob with cov. matr: Cx ("<< i << "," 
          << i << ") = " << Cx (i,i) << endreq;
      return StatusCode::FAILURE;
    }
  }
  //   correlation rho(i,j) between -1 and 1
  for (i=1; i<7; i++){
    for (int j=1; j<7&&j>i; j++){
      if ( fabs (Cx(i,j))/sqrt(Cx(i,i)*Cx(j,j)) > 1.0) {
        log << MSG::DEBUG << "Prob with cov. matr: Cx ("<< i << "," 
            << j << ") = "<< Cx(i,j) << endreq;
        return StatusCode::FAILURE;         
      }
    }
  }
  
  // fill the vertex object
  //    position
  constrVtx.setPosition(HepPoint3D(tpf(1),tpf(2),tpf(3))); 
  //    position error
  HepSymMatrix posErr=Cx.sub(1,3);
  constrVtx.setPositionErr(posErr);
  //    chisq, degrees of freedom
  constrVtx.setChi2(chis);
  constrVtx.setNDoF(ntracks);
  //    decay with constrained mass
  constrVtx.setType(Vertex::DecayWithMass);
  //    particles that compose the vertex
  for(iterP = pList.begin(); iterP != pList.end(); 
      iterP++) {
    constrVtx.addToProducts(*iterP);
  }
  
  
  // calculate mother particle momentum
  HepLorentzVector motherHepVec;
  motherHepVec.setPz( tpf(6)/sqrt(pow(tpf(4),2)+pow(tpf(5),2)+1.0));
  motherHepVec.setPx( tpf(4)*motherHepVec.pz());
  motherHepVec.setPy( tpf(5)*motherHepVec.pz());
  motherHepVec.setE( sqrt(pow(massConstr,2)+pow(motherHepVec.px(),2)+ 
                          pow(motherHepVec.py(),2) +
                          pow(motherHepVec.pz(),2)));
  
  //fill the Mother particle
  //   momentum
  motherParticle.setMomentum(motherHepVec);
  //   point on track
  motherParticle.setPointOnTrack(HepPoint3D(tpf(1),tpf(2),tpf(3)));
  //   point on track error (same as error vertex pos error)
  motherParticle.setPointOnTrackErr(posErr);
  //   error on track slopes and prop to momentum
  HepSymMatrix m_slpMomCorrelationError=Cx.sub(4,6);
  motherParticle.setSlopesMomErr(m_slpMomCorrelationError);
  
  //   corr err slopes point 
  //     check this: I would like to do Cx.sub(4,6,1,3) but
  //     Cx is simetric, this only work for HepMatrix??
  HepMatrix CxCopy=Cx;
  HepMatrix m_posSlpMomCorrelationError=CxCopy.sub(4,6,1,3);
  motherParticle.setPosSlopesCorr(m_posSlpMomCorrelationError);

  //   particle id  
  motherParticle.setParticleID((*partProp).jetsetID());
  //   confidence level
  motherParticle.setConfLevel(1.);
  //   mass
  motherParticle.setMass((*partProp).mass());
  //    massErr??
  //    check ressonance
  
   // hbar in MeV s
  // float hbar = 6.58211889*pow(10,-22);
  
  //  float motherParticleWidth = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
//  if((motherParticleWidth < 1) && ((*partProp).lifetime()*pow(10,-9) <
 // pow(10,-15))){
   

  if((*partProp).lifetime()*pow(10,-9) < pow(10,-15)) {
 
    motherParticle.setIsResonance(true);
    
  }//isRessonance
  
  //  motherParticle.setIsResonance(true);
  
  motherParticle.setEndVertex(&constrVtx);
  
  
  return StatusCode::SUCCESS;
}

// end of the first one ( fitWithOneSubMass method )











//==================================================================
// Perform mass constrained vertex fit (vector of particles).
// The procedure is implemented for four particles
// with two ressonances.
//==================================================================
StatusCode LagrangeMassVertexFitter::fitWithTwoSubMass(const std::string& 
                                                       motherName,
                                                       double subMass1,
                                                       double subMass2,
                                                       const ParticleVector& 
                                                       pList,
                                                       Vertex& constrVtx,
                                                       Particle& 
                                                       motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithTwoSubMass " 
      << endreq;
  
  
  
  
  // check the size of the input particle vector.
  
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  
  log << MSG::DEBUG << "Number of tracks in fitWithTwoSubMass = " 
      << ntracks << endreq;
  
  if ( ntracks != 4 ) {
    log << MSG::INFO << "Number of tracks must be 4  " << endreq;
    return StatusCode::FAILURE;
  }
  
  Vertex uncVertex;
  StatusCode scUncVertex = m_pVertexUnconst->fitVertex(pList,uncVertex);
  if( scUncVertex.isFailure() ) {
    log << MSG::FATAL << " unconstrained vertex failure " << endreq;
  }
  
  HepPoint3D vertexpos =  uncVertex.position();
  double zcer = vertexpos.z();
  
  log << MSG::DEBUG << "vertex unconst position" << vertexpos.x() 
      << " , " << vertexpos.y()  <<  " , "   << vertexpos.z() << endreq;  
  
  // Mass of the mother particle ( = massConstr )
  ParticleProperty*  partProp = m_ppSvc->find( motherName );
  double  massConstr = (*partProp).mass();
  
  // Mass of the sub particle 1 ( = subMass1Constr )
  
  double  subMass1Constr = subMass1;
  
  // Mass of the sub particle 2 ( = subMass2Constr )
  
  double  subMass2Constr = subMass2;
  
  // covariance matrix of the mother particle
  HepSymMatrix Cx(6,0);
  
  // dimension of the covariance matrix for one track ( = TRCOVDIM )
  int const TRCOVDIM  = 5;
  
  // maximum number of tracks ( = NMAXTRK ).
  int const NMAXTRK   = 4;
  
  // number of iterations ( = NMAXITERA ).
  int const NMAXITERA = 20 ;

  // dimension of the covariance matrix for 4 tracks.
  int dimCe = ntracks*TRCOVDIM;
  
  // mass of the daughter particles
  HepVector mass(NMAXTRK);
 
  // prepare the covariance matrix Ce and the vector e for 4 tracks.
  HepSymMatrix Ce(dimCe, 0);
  HepVector e(dimCe,0);
  
  // Position where CeTemp is embedded in Ce.
  int embedded_pos = 1;
  
  HepSymMatrix CeTemp(TRCOVDIM,0);
  HepVector    eTemp(TRCOVDIM,0); 
  
  int index = 1;
  
  ParticleVector::const_iterator iterP;
  
  for(iterP = pList.begin(); iterP != pList.end(); 
      iterP++) {
    
    // extrapolate particle to zEstimate 
    Particle transParticle;
    StatusCode sctrans = m_pTransporter->transport(iterP,
                                                   zcer,
                                                   transParticle);
    
    
    if ( !sctrans.isSuccess() ) {
      log << MSG::DEBUG << "Track extrapolation failed" << endreq;
      return sctrans;
    }
    HepPoint3D newPoint =transParticle.pointOnTrack();
    HepSymMatrix newpointErr = transParticle.pointOnTrackErr();
    HepSymMatrix newSlpMomCorrErr = transParticle.slopesMomErr();
    HepMatrix newPointSlpMomCorrErr = transParticle.posSlopesCorr();
    
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
    
    eTemp(1) = newPoint.x();
    eTemp(2) = newPoint.y();
    
    eTemp(3) = transParticle.slopeX();
    eTemp(4) = transParticle.slopeY();
    // momenta 
    double px = transParticle.momentum().px();
    double py = transParticle.momentum().py();
    double pz = transParticle.momentum().pz();
    eTemp(5) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
    
    // get the mass of this daughter particle
    mass(index) = (*iterP)->mass();
    
    //embed the one track matrix in the Ce matrix
    Ce.sub(embedded_pos,CeTemp);
    
    //embed the track parameters vector in the e vector
    e.sub(embedded_pos,eTemp);
    
    //update position for next embedding
    embedded_pos = embedded_pos + TRCOVDIM;

    index = index + 1; 
   
  }
  
  // copy parameter vector
  HepVector e0(e); 
  
  // cut on the difference of the constrMass and constrCalcMass
  double DIFFMASSCUT = 1.0e-6;
  // diffMass, diffZVer and DIFFZVERCUT
  double DIFFZVERCUT = 1.0e-6; 
  double diffMass = 0;
  double diffZver = 0;
  
  // Calculated constrained mass from the daughter particles. 
  double massConstrCalc;
  
  //Chisquare of the fit.
  double  chis;
  
  // Parameters vector of the resulting track.
  HepVector tpf(6);
  
  bool final = false;
  int  icount= 0; 
  while (!final && (icount < NMAXITERA)) {
    
    StatusCode sc_iter = itera(e, Ce, mass, massConstr, subMass1Constr, 
                               subMass2Constr, dimCe, final);
    if ( !sc_iter.isSuccess() ) {
      log << MSG::DEBUG << " itera failed  " << endreq;
      return StatusCode::FAILURE;  
    }
    
    
    // check that momenta are physical
    if (e(5)*e(10) < 0.0 ){
      log << MSG::DEBUG << "momenta out of range " << endreq;
      return StatusCode::FAILURE;
    } 
    if (( ntracks == 4) && ( e(15) < 0.0 )){
      log << MSG::DEBUG << "momenta out of range " << endreq;
      return StatusCode::FAILURE; 
    }
    
    // get constraint conditions
    StatusCode sc_evalu=  evalu(e, mass, subMass1Constr, subMass2Constr, 
                                massConstrCalc, diffZver);
    if ( !sc_evalu.isSuccess() ) {
      log << MSG::DEBUG << "constraints calc failed " << endreq;
      return StatusCode::FAILURE;
    }
    
    
    // finish if constraint are satisfied
    diffMass = fabs((massConstr-massConstrCalc)/massConstr);
    
    icount = icount + 1;
    
    if ((diffZver < DIFFZVERCUT) && (diffMass < DIFFMASSCUT)){
      final=true;
    }
  }
  
  // if not converged, return
  if(!final) {
    log << MSG::DEBUG << " no convergence  " << endreq;
    log << MSG::DEBUG << " massConstrCalc = " << massConstrCalc << endreq;
    log << MSG::DEBUG << " diffMass = " << diffMass << endreq;
    log << MSG::DEBUG << " icount = " << icount << endreq;
    return StatusCode::FAILURE; 
  }
  
  // later check the diffZver
  
  // Minimum found. Get the chisq of the fit.
  
  StatusCode sc_chisq = chisq( e0, e, Ce, chis);
  if ( !sc_chisq.isSuccess() ) {
    log << MSG::DEBUG << "chisq  calc failed " << endreq;
    return StatusCode::FAILURE;  
  }
  
  
  // update the covariance matrix for old parameters (here final=true)
  StatusCode sc_itera = 
    itera(e,Ce,mass,massConstr,subMass1Constr,subMass2Constr,dimCe,final);
  if ( !sc_itera.isSuccess() ) {
    log << MSG::DEBUG << " itera failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  
  // find the covariance matrix of the mother particle
  StatusCode sc_nwcov = nwcov(e, Ce, zcer, tpf, Cx, dimCe);
  if ( !sc_nwcov.isSuccess() ) {
    log << MSG::DEBUG << " nwcov failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  log << MSG::DEBUG << " lagrange: chis = " << chis << endreq;
  log << MSG::DEBUG << " lagrange: mass  = " << massConstrCalc << endreq;
  
  // check the covariance matrix for obvious problems
  //   diagonal positive
  int i;
  for (i=1;i<7;i++){
    if (Cx(i,i) < 0.0){
      log << MSG::DEBUG << "Prob with cov. matr: Cx ("<< i << "," 
          << i << ") = " << Cx (i,i) << endreq;
      return StatusCode::FAILURE;
    }
  }
  //   correlation rho(i,j) between -1 and 1
  for (i=1; i<7; i++){
    for (int j=1; j<7&&j>i; j++){
      if ( fabs (Cx(i,j))/sqrt(Cx(i,i)*Cx(j,j)) > 1.0) {
        log << MSG::DEBUG << "Prob with cov. matr: Cx ("<< i << "," 
            << j << ") = "<< Cx(i,j) << endreq;
        return StatusCode::FAILURE;         
      }
    }
  }
  
  // fill the vertex object
  //    position
  constrVtx.setPosition(HepPoint3D(tpf(1),tpf(2),tpf(3))); 
  //    position error
  HepSymMatrix posErr=Cx.sub(1,3);
  constrVtx.setPositionErr(posErr);
  //    chisq, degrees of freedom
  constrVtx.setChi2(chis);
  constrVtx.setNDoF(ntracks);
  //    decay with constrained mass
  constrVtx.setType(Vertex::DecayWithMass);
  //    particles that compose the vertex
  for(iterP = pList.begin(); iterP != pList.end(); 
      iterP++) {
    constrVtx.addToProducts(*iterP);
  }
  
  
  // calculate mother particle momentum
  HepLorentzVector motherHepVec;
  motherHepVec.setPz( tpf(6)/sqrt(pow(tpf(4),2)+pow(tpf(5),2)+1.0));
  motherHepVec.setPx( tpf(4)*motherHepVec.pz());
  motherHepVec.setPy( tpf(5)*motherHepVec.pz());
  motherHepVec.setE( sqrt(pow(massConstr,2)+pow(motherHepVec.px(),2)+ 
                          pow(motherHepVec.py(),2) +
                          pow(motherHepVec.pz(),2)));
  
  //fill the Mother particle
  //   momentum
  motherParticle.setMomentum(motherHepVec);
  //   point on track
  motherParticle.setPointOnTrack(HepPoint3D(tpf(1),tpf(2),tpf(3)));
  //   point on track error (same as error vertex pos error)
  motherParticle.setPointOnTrackErr(posErr);
  //   error on track slopes and prop to momentum
  HepSymMatrix m_slpMomCorrelationError=Cx.sub(4,6);
  motherParticle.setSlopesMomErr(m_slpMomCorrelationError);
  
  //   corr err slopes point 
  //     check this: I would like to do Cx.sub(4,6,1,3) but
  //     Cx is simetric, this only work for HepMatrix??
  HepMatrix CxCopy=Cx;
  HepMatrix m_posSlpMomCorrelationError=CxCopy.sub(4,6,1,3);
  motherParticle.setPosSlopesCorr(m_posSlpMomCorrelationError);
  
  //   particle id  
  motherParticle.setParticleID((*partProp).jetsetID());
  //   confidence level
  motherParticle.setConfLevel(1.);
  //   mass
  motherParticle.setMass((*partProp).mass());
  //    massErr??  
  //    check ressonance
  
  // hbar in MeV s
  //float hbar = 6.58211889*pow(10,-22);
  
  //  float motherParticleWidth = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  
//  if( (motherParticleWidth < 1) && ((*partProp).lifetime()*pow(10,-9) < 
  //                                   pow(10,-15))){
    
  if( (*partProp).lifetime()*pow(10,-9) < pow(10,-15)){
    motherParticle.setIsResonance(true);
  }//isRessonance
  
  //  motherParticle.setIsResonance(true);
  
  motherParticle.setEndVertex(&constrVtx);
  
  
  return StatusCode::SUCCESS;
}

// end of the second one ( fitWithTwoSubMass method )










//==================================================================
// Method name: itera ( 1 ressonance )
// This method performs one iteration of the constrained fit for 3
// or 4 tracks with one ressonance. 
// It returns the parameters vector (e) and the covariance matrix (Ce) 
// after one iteration of the contraint fit.  
//==================================================================
StatusCode LagrangeMassVertexFitter::itera(HepVector& e, HepSymMatrix& Ce,
                                           HepVector& mass, double massConstr,
                                           double subMassConstr,
                                           int dimCe, bool final) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ITERA !!! " << endreq;
  
  int ntracks = m_ntracks;
  HepMatrix B(ntracks,dimCe,0);
  HepVector cte(ntracks);
  
  double p1 = e(5);
  double p2 = e(10);
  double dm1 = mass(1);
  double dm2 = mass(2);
  double subm = subMassConstr;
  
  double p3, dm3, p4, dm4;
  
  if (( ntracks==3 ) || ( ntracks == 4)){
    p3 = e(15);
    dm3 = mass(3);
  }
  
  if ( ntracks == 4) {
    p4 = e(20);
    dm4 = mass(4);
  }
  
  
  //several combinations of momenta and slopes to abreviate calculus
  
  double s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double s22 = pow(e(8),2) + pow(e(9),2) + 1.0;  
  
  double p13, p23, s33, s13, s23, co13, co23;
  
  if (( ntracks == 3 ) || (ntracks == 4) ){
    p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
    p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
    s33 = pow(e(13),2) + pow(e(14),2) + 1.0;
    s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
    s23 = e(8)*e(13) + e(9)*e(14) + 1.0;
    co13 = s13 / sqrt( s11*s33 );
    co23 = s23 / sqrt( s22*s33 );
    
  }
  
  double p14, p24, p34, s14, s24, s34, s44, co14, co24, co34;
  
  if ( ntracks == 4){
    p14 = (pow(p1,2) + pow(dm1,2)) * (pow(p4,2) + pow(dm4,2));
    p24 = (pow(p2,2) + pow(dm2,2)) * (pow(p4,2) + pow(dm4,2));
    p34 = (pow(p3,2) + pow(dm3,2)) * (pow(p4,2) + pow(dm4,2));
    s14 = e(3)*e(18) + e(4)*e(19) + 1.0;
    s24 = e(8)*e(18) + e(9)*e(19) + 1.0;
    s34 = e(13)*e(18) + e(14)*e(19) + 1.0;
    s44 = pow(e(18),2) + pow(e(19),2) + 1.0;    
    co14 = s14 / sqrt( s11*s44);
    co24 = s24 / sqrt( s22*s44);
    co34 = s34 / sqrt( s33*s44);
    
  }

  double beta;
  
  if( ntracks == 3 ) {
    beta = ( pow(massConstr,2) - pow(subm,2) - pow(dm3,2) ) / 2.0 ;
  } 
  
  if (ntracks == 4 ) {
    beta = ( pow(massConstr,2) - pow(subm,2) - 
             pow(dm3,2) - pow(dm4,2) ) / 2.0 ;
  } 
  
  
  // Independent constant vector in the Taylor expansion of the
  // constraint equation. 
  
  if ( ntracks == 3 ){
    cte(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    cte(3) = sqrt(p13) + sqrt(p23) - p1*p3*co13 - p2*p3*co23 - beta;
  }
  
  if ( ntracks == 4 ){
    cte(1) = (e(6)  - e(1))*(e(4) - e(9))  - (e(7)  - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));  
    cte(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));
    cte(4) = sqrt(p13) + sqrt(p14) + sqrt(p23) + sqrt(p24) +
      sqrt(p34) - p1*p3*co13 - p1*p4*co14 - p2*p3*co23
      - p2*p4*co24 - p3*p4*co34 - beta;    
  }
  
  log << MSG::DEBUG << "lagrange constraint equations for one ressonance" 
      << endreq;
  log << MSG::DEBUG << cte << endreq;
  
  
  // Partial derivatives of 'sqrt' of momenta combinations.
  
  double d_5sp13,  d15sp13, d10sp23, d15sp23;
  
  if (( ntracks == 3 ) || (ntracks == 4) ){
    d_5sp13 = p1 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                         ( pow(p1,2) + pow(dm1,2) ) );
    d15sp13 = p3 * sqrt( (pow(p1,2) + pow(dm1,2)) / 
                         ( pow(p3,2) + pow(dm3,2) ) );
    d10sp23 = p2 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                         ( pow(p2,2) + pow(dm2,2) ) );
    d15sp23 = p3 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                         ( pow(p3,2) + pow(dm3,2) ) );
  }
  
  double d_5sp14, d20sp14, d10sp24, d20sp24, d15sp34, d20sp34;
  
  if ( ntracks == 4 ){
    d_5sp14 = p1 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                         ( pow(p1,2) + pow(dm1,2) ) );
    d20sp14 = p4 * sqrt( (pow(p1,2) + pow(dm1,2)) /
                         ( pow(p4,2) + pow(dm4,2) ) );  
    d10sp24 = p2 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                         ( pow(p2,2) + pow(dm2,2) ) );
    d20sp24 = p4 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                         ( pow(p4,2) + pow(dm4,2) ) );
    d15sp34 = p3 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                         ( pow(p3,2) + pow(dm3,2) ) );    
    d20sp34 = p4 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                         ( pow(p4,2) + pow(dm4,2) ) );  
  }
  
  
  // Partial derivatives for the cosines.
  
  double d_3co13, d_4co13, d13co13, d14co13, d_8co23,d_9co23,d13co23,d14co23;
  
  if (( ntracks == 3 ) || ( ntracks == 4)) {
    d_3co13 = ( e(13) - e(3)*s13/s11 ) / sqrt( s11*s33 );
    d_4co13 = ( e(14) - e(4)*s13/s11 ) / sqrt( s11*s33 );
    d13co13 = ( e(3) - e(13)*s13/s33 ) / sqrt( s11*s33 );
    d14co13 = ( e(4) - e(14)*s13/s33 ) / sqrt( s11*s33 );
    d_8co23 = ( e(13) - e(8)*s23/s22 ) / sqrt( s22*s33 );
    d_9co23 = ( e(14) - e(9)*s23/s22 ) / sqrt( s22*s33 );
    d13co23 = ( e(8) - e(13)*s23/s33 ) / sqrt( s22*s33 );
    d14co23 = ( e(9) - e(14)*s23/s33 ) / sqrt( s22*s33 );
  }
  
  double d_3co14, d_4co14, d18co14, d19co14,d_8co24, d_9co24, d18co24, d19co24,
    d13co34,d14co34,d18co34,d19co34;
  
  if ( ntracks == 4) {
    d_3co14 = ( e(18) - e(3)*s14/s11 ) / sqrt( s11*s44 );
    d_4co14 = ( e(19) - e(4)*s14/s11 ) / sqrt( s11*s44 );
    d18co14 = ( e(3) - e(18)*s14/s44 ) / sqrt( s11*s44 );
    d19co14 = ( e(4) - e(19)*s14/s44 ) / sqrt( s11*s44 );
    d_8co24 = ( e(18) - e(8)*s24/s22 ) / sqrt( s22*s44 );
    d_9co24 = ( e(19) - e(9)*s24/s22 ) / sqrt( s22*s44 );
    d18co24 = ( e(8) - e(18)*s24/s44 ) / sqrt( s22*s44 );
    d19co24 = ( e(9) - e(19)*s24/s44 ) / sqrt( s22*s44 );
    d13co34 = ( e(18) - e(13)*s34/s33) / sqrt( s33*s44 );
    d14co34 = ( e(19) - e(14)*s34/s33) / sqrt( s33*s44 );
    d18co34 = ( e(13) - e(18)*s34/s44) / sqrt( s33*s44 );
    d19co34 = ( e(14) - e(19)*s34/s44) / sqrt( s33*s44 );
  }
  
  // Constraint matrix in the linear aproximation
  
  B(1,1) = e(9) - e(4);
  B(1,2) = e(3) - e(8);
  B(1,3) = e(2) - e(7);
  B(1,4) = e(6) - e(1);
  B(1,6) =-B(1,1);
  B(1,7) =-B(1,2);
  B(1,8) =-B(1,3);
  B(1,9) =-B(1,4);
  
  if (( ntracks == 3 ) || (ntracks == 4)){
    B(2, 1) = e(14) - e(4);
    B(2, 2) = e(3)  - e(13);
    B(2, 3) = e(2)  - e(12);
    B(2, 4) = e(11) - e(1);
    B(2,11) =-B(2,1);
    B(2,12) =-B(2,2);
    B(2,13) =-B(2,3);
    B(2,14) =-B(2,4);
  }
  
  if ( ntracks == 3) {
    
    B(3, 3) = - p1*p3*d_3co13;
    B(3, 4) = - p1*p3*d_4co13;
    B(3, 8) = - p2*p3*d_8co23;
    B(3, 9) = - p2*p3*d_9co23;
    B(3,13) = - p1*p3*d13co13 - p2*p3*d13co23;
    B(3,14) = - p1*p3*d14co13 - p2*p3*d14co23;
    B(3, 5) = d_5sp13 - p3*co13;
    B(3,10) = d10sp23 - p3*co23;
    B(3,15) = d15sp13 + d15sp23 - p1*co13 - p2*co23;
  }
  
  if ( ntracks == 4) {
    
    B(3, 1) = e(19) - e(4);
    B(3, 2) = e(3)  - e(18);
    B(3, 3) = e(2)  - e(17);
    B(3, 4) = e(16) - e(1);
    B(3,16) = - B(3, 1);
    B(3,17) = - B(3, 2);
    B(3,18) = - B(3, 3);
    B(3,19) = - B(3, 4);
    
    B(4, 3) = - p1*p3*d_3co13 - p1*p4*d_3co14;
    B(4, 4) = - p1*p3*d_4co13 - p1*p4*d_4co14;
    B(4, 8) = - p2*p3*d_8co23 - p2*p4*d_8co24;
    B(4, 9) = - p2*p3*d_9co23 - p2*p4*d_9co24;
    B(4,13) = -p1*p3*d13co13 - p2*p3*d13co23 - p3*p4*d13co34;
    B(4,14) = -p1*p3*d14co13 - p2*p3*d14co23 - p3*p4*d14co34;
    B(4,18) = -p1*p4*d18co14 - p2*p4*d18co24 - p3*p4*d18co34;
    B(4,19) = -p1*p4*d19co14 - p2*p4*d19co24 - p3*p4*d19co34;
    
    B(4, 5) = d_5sp13 + d_5sp14 - p3*co13 - p4*co14;
    B(4,10) = d10sp23 + d10sp24 - p3*co23 - p4*co24;
    B(4,15) = d15sp13 + d15sp23 + d15sp34 - p1*co13 - p2*co23 - p4*co34;
    B(4,20) = d20sp14 + d20sp24 + d20sp34 - p1*co14 - p2*co24 - p3*co34;
  }
  
  //  Du = Ce*Bt
  
  HepMatrix Bt=B.T();
  
  HepMatrix Du = Ce*Bt;
  
  //  Dv = B*Ce*Bt
  HepSymMatrix  Dv =Ce.similarity(B);
  
  //  Dvi = Dv inverse
  
  int inv;
  HepSymMatrix Dvi = Dv.inverse(inv);
  
  if ( inv != 0 ){
    log << MSG::DEBUG << "Dv inverse failure" << endreq;     
    return StatusCode::FAILURE;
  }
  
  //  Dw = Du*Dvi = Ce*Bt * (B*Ce*Bt)i
  
  HepMatrix Dw = Du*Dvi;
  
  //  de = Ce*Bt * (B*Ce*Bt)i * cte
  
  HepVector de = Dw*cte;
  
  //  log << MSG::DEBUG << "de - track parameters variation " << endreq;
  
  //  log << MSG::DEBUG << de << endreq;
  
  // update 'e' values acording to the fit result (iter. mode)
  
  if (!final){
    // e = e - Ce*Bt * (B*Ce*Bt)i * cte
    e = e - de;
  } else {
    // update the covariance matrix ( final = true )
    
    // Dy = Ce*Bt * (B*Ce*Bt)i * B
    
    HepMatrix Dy = Dw*B;
    
    // identity matrix     
    HepDiagMatrix One(dimCe,1);
    Ce.assign((One-Dy)*Ce);
    
  }
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: itera ( 2 ressonances )
// This method performs one iteration of the constrained fit for 
// 4 tracks with two ressonances. 
// It returns the parameters vector (e) and the covariance matrix (Ce) 
// after one iteration of the contraint fit.  
//==================================================================
StatusCode LagrangeMassVertexFitter::itera(HepVector& e, HepSymMatrix& Ce,
                                           HepVector& mass, double massConstr,
                                           double subMass1Constr, 
                                           double subMass2Constr,
                                           int dimCe, bool final) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ITERA !!! " << endreq;
  
  int ntracks = m_ntracks;  
  HepMatrix B(ntracks,dimCe,0);
  HepVector cte(ntracks);
  
  double p1 = e(5);
  double p2 = e(10);
  double p3 = e(15);
  double p4 = e(20);
  double dm1 = mass(1);
  double dm2 = mass(2); 
  double dm3 = mass(3); 
  double dm4 = mass(4);
  double subm1 = subMass1Constr;
  double subm2 = subMass2Constr;
  
  //several combinations of momenta and slopes to abreviate calculus
  
  double s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
  double s33 = pow(e(13),2) + pow(e(14),2) + 1.0;  
  double s44 = pow(e(18),2) + pow(e(19),2) + 1.0;    
  
  double   p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
  double   p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
  double   p14 = (pow(p1,2) + pow(dm1,2)) * (pow(p4,2) + pow(dm4,2));
  double   p24 = (pow(p2,2) + pow(dm2,2)) * (pow(p4,2) + pow(dm4,2));
   
  double   s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
  double   s23 = e(8)*e(13) + e(9)*e(14) + 1.0;
  double   s14 = e(3)*e(18) + e(4)*e(19) + 1.0;
  double   s24 = e(8)*e(18) + e(9)*e(19) + 1.0;
  
  double   co13 = s13 / sqrt( s11*s33 );
  double   co23 = s23 / sqrt( s22*s33 );  
  double   co14 = s14 / sqrt( s11*s44);
  double   co24 = s24 / sqrt( s22*s44);
 
  double beta = ( pow(massConstr,2) - pow(subm1,2) - pow(subm2,2) ) / 2.0 ;
  
  
  // Independent constant vector in the Taylor expansion of the
  // constraint equation. 
  
  cte(1) = (e(6)  - e(1))*(e(4) - e(9))  - (e(7)  - e(2))*(e(3) - e(8));
  cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));  
  cte(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));
  cte(4) = sqrt(p13) + sqrt(p14) + sqrt(p23) + sqrt(p24) +
    - p1*p3*co13 - p1*p4*co14 - p2*p3*co23
    - p2*p4*co24 - beta;     
  
  log << MSG::DEBUG << "lagrange constrained equations for 4 tracks with"
      << " two ressonances" << endreq;
  log << MSG::DEBUG << cte << endreq;
  
  
  // Partial derivatives of 'sqrt' of momenta combinations.
  
  double   d_5sp13 = p1 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                                ( pow(p1,2) + pow(dm1,2) ) );
  double   d15sp13 = p3 * sqrt( (pow(p1,2) + pow(dm1,2)) / 
                                ( pow(p3,2) + pow(dm3,2) ) );
  double   d10sp23 = p2 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                                ( pow(p2,2) + pow(dm2,2) ) );
  double   d15sp23 = p3 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                                ( pow(p3,2) + pow(dm3,2) ) );
  
  double    d_5sp14 = p1 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                                 ( pow(p1,2) + pow(dm1,2) ) );
  double    d20sp14 = p4 * sqrt( (pow(p1,2) + pow(dm1,2)) /
                                 ( pow(p4,2) + pow(dm4,2) ) );  
  double    d10sp24 = p2 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                                 ( pow(p2,2) + pow(dm2,2) ) );
  double    d20sp24 = p4 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                                 ( pow(p4,2) + pow(dm4,2) ) );
  
  // Partial derivatives for the cosines.
  
  double   d_3co13 = ( e(13) - e(3)*s13/s11 ) / sqrt( s11*s33 );
  double   d_4co13 = ( e(14) - e(4)*s13/s11 ) / sqrt( s11*s33 );
  double   d13co13 = ( e(3) - e(13)*s13/s33 ) / sqrt( s11*s33 );
  double   d14co13 = ( e(4) - e(14)*s13/s33 ) / sqrt( s11*s33 );
  double   d_8co23 = ( e(13) - e(8)*s23/s22 ) / sqrt( s22*s33 );
  double   d_9co23 = ( e(14) - e(9)*s23/s22 ) / sqrt( s22*s33 );
  double   d13co23 = ( e(8) - e(13)*s23/s33 ) / sqrt( s22*s33 );
  double   d14co23 = ( e(9) - e(14)*s23/s33 ) / sqrt( s22*s33 );
    
  double    d_3co14 = ( e(18) - e(3)*s14/s11 ) / sqrt( s11*s44 );
  double    d_4co14 = ( e(19) - e(4)*s14/s11 ) / sqrt( s11*s44 );
  double    d18co14 = ( e(3) - e(18)*s14/s44 ) / sqrt( s11*s44 );
  double    d19co14 = ( e(4) - e(19)*s14/s44 ) / sqrt( s11*s44 );
  double    d_8co24 = ( e(18) - e(8)*s24/s22 ) / sqrt( s22*s44 );
  double    d_9co24 = ( e(19) - e(9)*s24/s22 ) / sqrt( s22*s44 );
  double    d18co24 = ( e(8) - e(18)*s24/s44 ) / sqrt( s22*s44 );
  double    d19co24 = ( e(9) - e(19)*s24/s44 ) / sqrt( s22*s44 );
 
  // Constraint matrix in the linear aproximation
  
  B(1,1) = e(9) - e(4);
  B(1,2) = e(3) - e(8);
  B(1,3) = e(2) - e(7);
  B(1,4) = e(6) - e(1);
  B(1,6) =-B(1,1);
  B(1,7) =-B(1,2);
  B(1,8) =-B(1,3);
  B(1,9) =-B(1,4);
  
  B(2, 1) = e(14) - e(4);
  B(2, 2) = e(3)  - e(13);
  B(2, 3) = e(2)  - e(12);
  B(2, 4) = e(11) - e(1);
  B(2,11) =-B(2,1);
  B(2,12) =-B(2,2);
  B(2,13) =-B(2,3);
  B(2,14) =-B(2,4);
  
  B(3, 1) = e(19) - e(4);
  B(3, 2) = e(3)  - e(18);
  B(3, 3) = e(2)  - e(17);
  B(3, 4) = e(16) - e(1);
  B(3,16) = - B(3, 1);
  B(3,17) = - B(3, 2);
  B(3,18) = - B(3, 3);
  B(3,19) = - B(3, 4);
  
  B(4, 3) = - p1*p3*d_3co13 - p1*p4*d_3co14;
  B(4, 4) = - p1*p3*d_4co13 - p1*p4*d_4co14;
  B(4, 8) = - p2*p3*d_8co23 - p2*p4*d_8co24;
  B(4, 9) = - p2*p3*d_9co23 - p2*p4*d_9co24;
  B(4,13) = -p1*p3*d13co13 - p2*p3*d13co23 ;
  B(4,14) = -p1*p3*d14co13 - p2*p3*d14co23 ;
  B(4,18) = -p1*p4*d18co14 - p2*p4*d18co24 ;
  B(4,19) = -p1*p4*d19co14 - p2*p4*d19co24 ;
  
  B(4, 5) = d_5sp13 + d_5sp14 - p3*co13 - p4*co14;
  B(4,10) = d10sp23 + d10sp24 - p3*co23 - p4*co24;
  B(4,15) = d15sp13 + d15sp23 - p1*co13 - p2*co23;
  B(4,20) = d20sp14 + d20sp24 - p1*co14 - p2*co24;
  
  
  //  Du = Ce*Bt
  
  HepMatrix Bt=B.T();
  
  HepMatrix Du = Ce*Bt;
  
  //  Dv = B*Ce*Bt
  HepSymMatrix  Dv =Ce.similarity(B);
  
  //  Dvi = Dv inverse
  
  int inv;
  HepSymMatrix Dvi = Dv.inverse(inv);
  
  if ( inv != 0 ){
    log << MSG::DEBUG << "Dv inverse failure" << endreq;     
    return StatusCode::FAILURE;
  }
  
  //  Dw = Du*Dvi = Ce*Bt * (B*Ce*Bt)i
  
  HepMatrix Dw = Du*Dvi;
  
  //  de = Ce*Bt * (B*Ce*Bt)i * cte
 
  HepVector de = Dw*cte;
  
  //  log << MSG::DEBUG << "de - track parameters variation " << endreq;
  
  //  log << MSG::DEBUG << de << endreq;
  
  // update 'e' values acording to the fit result (iter. mode)
  
  if (!final){
    // e = e - Ce*Bt * (B*Ce*Bt)i * cte
    e = e - de;
  } else {
    // update the covariance matrix ( final = true )
    
    // Dy = Ce*Bt * (B*Ce*Bt)i * B
    
    HepMatrix Dy = Dw*B;
    
    // identity matrix     
    HepDiagMatrix One(dimCe,1);
    Ce.assign((One-Dy)*Ce);
    
  }
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: itera ( without ressonances )
// This method performs one iteration of the constrained fit for 2, 3
// or 4 tracks without ressonance. 
// It returns the parameters vector (e) and the covariance matrix (Ce) 
// after one iteration of the contraint fit.  
//==================================================================
StatusCode LagrangeMassVertexFitter::itera(HepVector& e, HepSymMatrix& Ce,
                                           HepVector& mass, double massConstr,
                                           int dimCe, bool final) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ITERA !!! " << endreq;
  
  int ntracks = m_ntracks;
  HepMatrix B(ntracks,dimCe,0);
  HepVector cte(ntracks);
  
  double p1 = e(5);
  double p2 = e(10);
  double dm1 = mass(1);
  double dm2 = mass(2);
  double p3, dm3, p4, dm4;
  
  if (( ntracks==3 ) || ( ntracks == 4)){
    p3 = e(15);
    dm3 = mass(3);
  }
  
  if ( ntracks == 4) {
    p4 = e(20);
    dm4 = mass(4);
  }
  
  
  //several combinations of momenta and slopes to abreviate calculus
  
  double p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2));
  double s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
  double s12 = e(3)*e(8) + e(4)*e(9) + 1.0;
  double co12 = s12 / sqrt( s11*s22 );
  double p13, p23, s33, s13, s23, co13, co23;
    
  if (( ntracks == 3 ) || (ntracks == 4) ){
    p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
    p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
    s33 = pow(e(13),2) + pow(e(14),2) + 1.0;
    s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
    s23 = e(8)*e(13) + e(9)*e(14) + 1.0;
    co13 = s13 / sqrt( s11*s33 );
    co23 = s23 / sqrt( s22*s33 );
      
  }

  double p14, p24, p34, s14, s24, s34, s44, co14, co24, co34;
  
  if ( ntracks == 4){
    p14 = (pow(p1,2) + pow(dm1,2)) * (pow(p4,2) + pow(dm4,2));
    p24 = (pow(p2,2) + pow(dm2,2)) * (pow(p4,2) + pow(dm4,2));
    p34 = (pow(p3,2) + pow(dm3,2)) * (pow(p4,2) + pow(dm4,2));
    s14 = e(3)*e(18) + e(4)*e(19) + 1.0;
    s24 = e(8)*e(18) + e(9)*e(19) + 1.0;
    s34 = e(13)*e(18) + e(14)*e(19) + 1.0;
    s44 = pow(e(18),2) + pow(e(19),2) + 1.0;    
    co14 = s14 / sqrt( s11*s44);
    co24 = s24 / sqrt( s22*s44);
    co34 = s34 / sqrt( s33*s44);    
  }
  
  double beta;
  if (ntracks == 2) { 
    beta = ( pow(massConstr,2) - pow(dm1,2) - pow(dm2,2) ) / 2.0 ;
  }
  
  
  if( ntracks == 3 ) {
    beta = ( pow(massConstr,2) - pow(dm1,2) - pow(dm2,2) - 
             pow(dm3,2) ) / 2.0 ;
  } 
  
  if (ntracks == 4 ) {
    beta = ( pow(massConstr,2) - pow(dm1,2) - pow(dm2,2) - 
             pow(dm3,2) - pow(dm4,2) ) / 2.0 ;
  } 
  
  
  // Independent constant vector in the Taylor expansion of the
  // constraint equation. 
  
  if ( ntracks == 2 ){
    cte(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    cte(2) = sqrt(p12) - p1*p2*co12 - beta;
  }
  
  

  if ( ntracks == 3 ){
    cte(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    cte(3) = sqrt(p12) + sqrt(p13) + sqrt(p23) -
      p1*p2*co12 - p1*p3*co13 - p2*p3*co23 - beta;
  }
  
  if ( ntracks == 4 ){
    cte(1) = (e(6)  - e(1))*(e(4) - e(9))  - (e(7)  - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));  
    cte(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));
    cte(4) = sqrt(p12) + sqrt(p13) + sqrt(p14) + sqrt(p23) + sqrt(p24) +
      sqrt(p34) - p1*p2*co12 - p1*p3*co13 - p1*p4*co14 - p2*p3*co23
      - p2*p4*co24 - p3*p4*co34 - beta;    
  }
  
  log << MSG::DEBUG << "lagrange constraint equations " << endreq;
  log << MSG::DEBUG << cte << endreq;
  
  
  // Partial derivatives of 'sqrt' of momenta combinations.
  
  double d_5sp12 = p1 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                              ( pow(p1,2) + pow(dm1,2) ) );
  double d10sp12 = p2 * sqrt( (pow(p1,2) + pow(dm1,2)) / 
                              ( pow(p2,2) + pow(dm2,2) ) );
  
  double d_5sp13,  d15sp13, d10sp23, d15sp23;
  if (( ntracks == 3 ) || (ntracks == 4) ){
    d_5sp13 = p1 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                         ( pow(p1,2) + pow(dm1,2) ) );
    d15sp13 = p3 * sqrt( (pow(p1,2) + pow(dm1,2)) / 
                         ( pow(p3,2) + pow(dm3,2) ) );
    d10sp23 = p2 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                         ( pow(p2,2) + pow(dm2,2) ) );
    d15sp23 = p3 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                         ( pow(p3,2) + pow(dm3,2) ) );
  }
  
  double d_5sp14, d20sp14, d10sp24, d20sp24, d15sp34, d20sp34;
  
  if ( ntracks == 4 ){
    d_5sp14 = p1 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                         ( pow(p1,2) + pow(dm1,2) ) );
    d20sp14 = p4 * sqrt( (pow(p1,2) + pow(dm1,2)) /
                         ( pow(p4,2) + pow(dm4,2) ) );  
    d10sp24 = p2 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                         ( pow(p2,2) + pow(dm2,2) ) );
    d20sp24 = p4 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                         ( pow(p4,2) + pow(dm4,2) ) );
    d15sp34 = p3 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                         ( pow(p3,2) + pow(dm3,2) ) );    
    d20sp34 = p4 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                         ( pow(p4,2) + pow(dm4,2) ) );  
  }
  
  
  // Partial derivatives for the cosines.
  
  double d_3co12 = ( e(8) - e(3)*s12/s11 ) / sqrt( s11*s22 );
  double d_4co12 = ( e(9) - e(4)*s12/s11 ) / sqrt( s11*s22 );
  double d_8co12 = ( e(3) - e(8)*s12/s22 ) / sqrt( s11*s22 );
  double d_9co12 = ( e(4) - e(9)*s12/s22 ) / sqrt( s11*s22 );
  double d_3co13, d_4co13, d13co13, d14co13, d_8co23,d_9co23,d13co23,d14co23;
  if (( ntracks == 3 ) || ( ntracks == 4)) {
    d_3co13 = ( e(13) - e(3)*s13/s11 ) / sqrt( s11*s33 );
    d_4co13 = ( e(14) - e(4)*s13/s11 ) / sqrt( s11*s33 );
    d13co13 = ( e(3) - e(13)*s13/s33 ) / sqrt( s11*s33 );
    d14co13 = ( e(4) - e(14)*s13/s33 ) / sqrt( s11*s33 );
    d_8co23 = ( e(13) - e(8)*s23/s22 ) / sqrt( s22*s33 );
    d_9co23 = ( e(14) - e(9)*s23/s22 ) / sqrt( s22*s33 );
    d13co23 = ( e(8) - e(13)*s23/s33 ) / sqrt( s22*s33 );
    d14co23 = ( e(9) - e(14)*s23/s33 ) / sqrt( s22*s33 );
  }
  double d_3co14, d_4co14, d18co14, d19co14,d_8co24, d_9co24, d18co24, d19co24,
    d13co34,d14co34,d18co34,d19co34;
  if ( ntracks == 4) {
    d_3co14 = ( e(18) - e(3)*s14/s11 ) / sqrt( s11*s44 );
    d_4co14 = ( e(19) - e(4)*s14/s11 ) / sqrt( s11*s44 );
    d18co14 = ( e(3) - e(18)*s14/s44 ) / sqrt( s11*s44 );
    d19co14 = ( e(4) - e(19)*s14/s44 ) / sqrt( s11*s44 );
    d_8co24 = ( e(18) - e(8)*s24/s22 ) / sqrt( s22*s44 );
    d_9co24 = ( e(19) - e(9)*s24/s22 ) / sqrt( s22*s44 );
    d18co24 = ( e(8) - e(18)*s24/s44 ) / sqrt( s22*s44 );
    d19co24 = ( e(9) - e(19)*s24/s44 ) / sqrt( s22*s44 );
    d13co34 = ( e(18) - e(13)*s34/s33) / sqrt( s33*s44 );
    d14co34 = ( e(19) - e(14)*s34/s33) / sqrt( s33*s44 );
    d18co34 = ( e(13) - e(18)*s34/s44) / sqrt( s33*s44 );
    d19co34 = ( e(14) - e(19)*s34/s44) / sqrt( s33*s44 );
  }
  
  
  // Constraint matrix in the linear aproximation
  B(1,1) = e(9) - e(4);
  B(1,2) = e(3) - e(8);
  B(1,3) = e(2) - e(7);
  B(1,4) = e(6) - e(1);
  B(1,6) =-B(1,1);
  B(1,7) =-B(1,2);
  B(1,8) =-B(1,3);
  B(1,9) =-B(1,4);
 
  if ( ntracks == 2 ) {
    B(2, 3) = -p1*p2*d_3co12;
    B(2, 4) = -p1*p2*d_4co12;
    B(2, 8) = -p1*p2*d_8co12;
    B(2, 9) = -p1*p2*d_9co12;
    B(2, 5) = d_5sp12 - p2*co12;
    B(2,10) = d10sp12 - p1*co12;
  }
 
  if (( ntracks == 3 ) || (ntracks == 4)){
    B(2, 1) = e(14) - e(4);
    B(2, 2) = e(3)  - e(13);
    B(2, 3) = e(2)  - e(12);
    B(2, 4) = e(11) - e(1);
    B(2,11) =-B(2,1);
    B(2,12) =-B(2,2);
    B(2,13) =-B(2,3);
    B(2,14) =-B(2,4);
  }
 
  if ( ntracks == 3) {
   
    B(3, 3) = -p1*p2*d_3co12 - p1*p3*d_3co13;
    B(3, 4) = -p1*p2*d_4co12 - p1*p3*d_4co13;
    B(3, 8) = -p1*p2*d_8co12                   - p2*p3*d_8co23;
    B(3, 9) = -p1*p2*d_9co12                   - p2*p3*d_9co23;
    B(3,13) =                  - p1*p3*d13co13 - p2*p3*d13co23;
    B(3,14) =                  - p1*p3*d14co13 - p2*p3*d14co23;
    B(3, 5) = d_5sp12 + d_5sp13 - p2*co12 - p3*co13;
    B(3,10) = d10sp12 + d10sp23 - p1*co12 - p3*co23;
    B(3,15) = d15sp13 + d15sp23 - p1*co13 - p2*co23;
  }
 
  if ( ntracks == 4) {

    B(3, 1) = e(19) - e(4);
    B(3, 2) = e(3)  - e(18);
    B(3, 3) = e(2)  - e(17);
    B(3, 4) = e(16) - e(1);
    B(3,16) = - B(3, 1);
    B(3,17) = - B(3, 2);
    B(3,18) = - B(3, 3);
    B(3,19) = - B(3, 4);
    
    B(4, 3) = -p1*p2*d_3co12 - p1*p3*d_3co13 - p1*p4*d_3co14;
    B(4, 4) = -p1*p2*d_4co12 - p1*p3*d_4co13 - p1*p4*d_4co14;
    B(4, 8) = -p1*p2*d_8co12 - p2*p3*d_8co23 - p2*p4*d_8co24;
    B(4, 9) = -p1*p2*d_9co12 - p2*p3*d_9co23 - p2*p4*d_9co24;
    B(4,13) = -p1*p3*d13co13 - p2*p3*d13co23 - p3*p4*d13co34;
    B(4,14) = -p1*p3*d14co13 - p2*p3*d14co23 - p3*p4*d14co34;
    B(4,18) = -p1*p4*d18co14 - p2*p4*d18co24 - p3*p4*d18co34;
    B(4,19) = -p1*p4*d19co14 - p2*p4*d19co24 - p3*p4*d19co34;
    
    B(4, 5) = d_5sp12 + d_5sp13 + d_5sp14 - p2*co12 - p3*co13 - p4*co14;
    B(4,10) = d10sp12 + d10sp23 + d10sp24 - p1*co12 - p3*co23 - p4*co24;
    B(4,15) = d15sp13 + d15sp23 + d15sp34 - p1*co13 - p2*co23 - p4*co34;
    B(4,20) = d20sp14 + d20sp24 + d20sp34 - p1*co14 - p2*co24 - p3*co34;
  }
 
  //  Du = Ce*Bt

  HepMatrix Bt=B.T();
  
  HepMatrix Du = Ce*Bt;
  
  //  Dv = B*Ce*Bt
  HepSymMatrix  Dv =Ce.similarity(B);
  
  //  Dvi = Dv inverse
  
  int inv;
  HepSymMatrix Dvi = Dv.inverse(inv);
  
  if ( inv != 0 ){
    log << MSG::DEBUG << "Dv inverse failure" << endreq;     
    return StatusCode::FAILURE;
  }
  
  //  Dw = Du*Dvi = Ce*Bt * (B*Ce*Bt)i
  
  HepMatrix Dw = Du*Dvi;
  
  //  de = Ce*Bt * (B*Ce*Bt)i * cte
  
  HepVector de = Dw*cte;
  
  //  log << MSG::DEBUG << "de - track parameters variation " << endreq;
  
  //  log << MSG::DEBUG << de << endreq;
  
  // update 'e' values acording to the fit result (iter. mode)
  
  if (!final){
    // e = e - Ce*Bt * (B*Ce*Bt)i * cte
    e = e - de;
  } else {
    // update the covariance matrix ( final = true )
    
    // Dy = Ce*Bt * (B*Ce*Bt)i * B
    
    HepMatrix Dy = Dw*B;
    
    // identity matrix     
    HepDiagMatrix One(dimCe,1);
    Ce.assign((One-Dy)*Ce);
    
  }
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: evalu ( 1 ressonance )  
// This method evaluates how well the constrained equations are satisfied
// at a given iteration. This method works for the case or 3 or 4 
// tracks with one ressonance. 
// It returns the invariant mass at this step ( = massConstrCalc ) and 
// the difference in the estimated z-vertex ( = diffZver ).
//==================================================================

StatusCode LagrangeMassVertexFitter::evalu(HepVector& e, HepVector& mass,
                                           double subMassConstr,  
                                           double&  massConstrCalc, 
                                           double  diffZver)
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " EVALU !!! " << endreq;
  
  double dm1 = mass(1);
  double dm2 = mass(2);
  double dm3;
  double p1  = e(5);
  double p2  = e(10);
  double p3;
  double subm = subMassConstr;
  
  int ntracks = m_ntracks;
  
  if (( ntracks == 3 ) || ( ntracks == 4) ) {
    p3  = e(15);
    dm3 = mass(3);
  }

  double p4;
  double dm4;

  if ( ntracks == 4) {
    p4 = e(20);  
    dm4 = mass(4);
  }

  massConstrCalc = 0.0;
  diffZver       = 0.0;

  double s11, s22;
  double p13, p23, s33, s13, s23, co13, co23;

  double p14, p24, p34, s14, s24, s34, s44, co14, co24, co34;
   
  if ( ntracks == 3 ){
  
    p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
    p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
    s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
    s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
    s33 = pow(e(13),2) + pow(e(14),2) + 1.0;    
    s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
    s23 = e(8)*e(13) + e(9)*e(14) + 1.0;    
    co13 = s13 / sqrt( s11*s33 );
    co23 = s23 / sqrt( s22*s33 );
      
    massConstrCalc = pow(subm,2) + pow(dm3,2)
      + 2.*( sqrt(p13) + sqrt(p23) )
      - 2.*( p1*p3*co13  + p2*p3*co23  );
    massConstrCalc = sqrt(massConstrCalc);
    
    diffZver = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZver = fabs(diffZver);
  }

  else if ( ntracks == 4) {
       
    p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
    p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
    s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
    s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
    s33 = pow(e(13),2) + pow(e(14),2) + 1.0;   
    s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
    s23 = e(8)*e(13) + e(9)*e(14) + 1.0;  
    co13 = s13 / sqrt( s11*s33 );
    co23 = s23 / sqrt( s22*s33 );
      
    p14 = (pow(p1,2) + pow(dm1,2)) * (pow(p4,2) + pow(dm4,2));
    p24 = (pow(p2,2) + pow(dm2,2)) * (pow(p4,2) + pow(dm4,2));
    p34 = (pow(p3,2) + pow(dm3,2)) * (pow(p4,2) + pow(dm4,2));
    s14 = e(3)*e(18) + e(4)*e(19) + 1.0;
    s24 = e(8)*e(18) + e(9)*e(19) + 1.0;
    s34 = e(13)*e(18) + e(14)*e(19) + 1.0;
    s44 = pow(e(18),2) + pow(e(19),2) + 1.0;
    co14 = s14 / sqrt( s11*s44);
    co24 = s24 / sqrt( s22*s44);
    co34 = s34 / sqrt( s33*s44);

    massConstrCalc = pow(subm,2) + pow(dm3,2) + pow( dm4,2)
      + 2.0*( sqrt(p13) + sqrt(p14) + sqrt(p23)  +
              sqrt(p24) + sqrt(p34) )
      - 2.*( p1*p3*co13  + p1*p4*co14 + p2*p3*co23 + 
             p2*p4*co24  + p3*p4*co34 );
    massConstrCalc = sqrt(massConstrCalc);
    
    diffZver = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZver = fabs(diffZver);
    
  }
  
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: evalu ( 2 ressonances )  
// This method evaluates how well the constrained equations are satisfied
// at a given iteration. This method works for the case of 4 
// tracks with two ressonances. 
// It returns the invariant mass at this step ( = massConstrCalc ) and 
// the difference in the estimated z-vertex ( = diffZver ).
//==================================================================

StatusCode LagrangeMassVertexFitter::evalu(HepVector& e, HepVector& mass,
                                           double subMass1Constr,
                                           double subMass2Constr,
                                           double&  massConstrCalc, 
                                           double  diffZver)					   
  
  
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " EVALU !!! " << endreq;
  
  double dm1 = mass(1);
  double dm2 = mass(2);
  double dm3 = mass(3);
  double dm4 = mass(4);
  double p1  = e(5);
  double p2  = e(10);
  double p3 = e(15);
  double p4 = e(20);  
  double subm1 = subMass1Constr;
  double subm2 = subMass2Constr;

  // int ntracks = m_ntracks;
  
  massConstrCalc = 0.0;
  diffZver       = 0.0;
  
  double s11, s22;
  double p13, p23, s33, s13, s23, co13, co23;
  
  double p14, p24, s14, s24, s44, co14, co24;
  
  p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
  p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
  s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
  s33 = pow(e(13),2) + pow(e(14),2) + 1.0;   
  s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
  s23 = e(8)*e(13) + e(9)*e(14) + 1.0;   
  co13 = s13 / sqrt( s11*s33 );
  co23 = s23 / sqrt( s22*s33 );
  
  p14 = (pow(p1,2) + pow(dm1,2)) * (pow(p4,2) + pow(dm4,2));
  p24 = (pow(p2,2) + pow(dm2,2)) * (pow(p4,2) + pow(dm4,2));   
  s14 = e(3)*e(18) + e(4)*e(19) + 1.0;
  s24 = e(8)*e(18) + e(9)*e(19) + 1.0;    
  s44 = pow(e(18),2) + pow(e(19),2) + 1.0;
  co14 = s14 / sqrt( s11*s44);
  co24 = s24 / sqrt( s22*s44);    
  
  massConstrCalc = pow(subm1,2) + pow(subm2,2) 
    + 2.0*( sqrt(p13) + sqrt(p14) + sqrt(p23)  +
            sqrt(p24) )
    - 2.*( p1*p3*co13  + p1*p4*co14 + p2*p3*co23 + 
           p2*p4*co24 );
  massConstrCalc = sqrt(massConstrCalc);
  
  diffZver = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
  diffZver = fabs(diffZver);
  
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: evalu ( without ressonances )  
// This method evaluates how well the constrained equations are satisfied
// at a given iteration. This method works for the case of 2, 3 or 4 
// tracks without ressonances. 
// It returns the invariant mass at this step ( = massConstrCalc ) and 
// the difference in the estimated z-vertex ( = diffZver ).
//==================================================================

StatusCode LagrangeMassVertexFitter::evalu(HepVector& e, HepVector& mass,
                                           double&  massConstrCalc, 
                                           double  diffZver)
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " EVALU !!! " << endreq;
  
  double dm1 = mass(1);
  double dm2 = mass(2);
  double dm3;
  double p1  = e(5);
  double p2  = e(10);
  double p3;
  
  int ntracks = m_ntracks;
  if (( ntracks == 3 ) || ( ntracks == 4) ) {
    p3  = e(15);
    dm3 = mass(3);
  }
  
  double p4;
  double dm4;
  
  if ( ntracks == 4) {
    p4 = e(20);  
    dm4 = mass(4);
  }


  massConstrCalc = 0.0;
  diffZver       = 0.0;
  
  double p12, s11, s22, s12, co12;
  double p13, p23, s33, s13, s23, co13, co23;
  
  double p14, p24, p34, s14, s24, s34, s44, co14, co24, co34;
  
  if ( ntracks == 2 ) {
    p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2) );
    s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
    s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
    s12 = e(3)*e(8) + e(4)*e(9) + 1.0;   
    co12 = s12 / sqrt( s11*s22 );
    
    massConstrCalc = pow(dm1,2) + pow(dm2,2) + 2.*sqrt(p12) - 2.*p1*p2*co12;
    massConstrCalc = sqrt(massConstrCalc);
    
    diffZver = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZver = fabs( diffZver );
  }
  else if ( ntracks == 3 ){
    
    p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2) );
    p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
    p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
    s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
    s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
    s33 = pow(e(13),2) + pow(e(14),2) + 1.0;
    s12 = e(3)*e(8) + e(4)*e(9) + 1.0;
    s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
    s23 = e(8)*e(13) + e(9)*e(14) + 1.0;
    co12 = s12 / sqrt( s11*s22 );
    co13 = s13 / sqrt( s11*s33 );
    co23 = s23 / sqrt( s22*s33 );
      
    massConstrCalc = pow(dm1,2) + pow(dm2,2) + pow(dm3,2)
      + 2.*( sqrt(p12) + sqrt(p13) + sqrt(p23) )
      - 2.*( p1*p2*co12  + p1*p3*co13  + p2*p3*co23  );
    massConstrCalc = sqrt(massConstrCalc);
    
    diffZver = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZver = fabs(diffZver);
  }
  
  else if ( ntracks == 4) {
    
    p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2) );
    p13 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p3,2) + pow(dm3,2) );
    p23 = ( pow(p2,2) + pow(dm2,2) ) * ( pow(p3,2) + pow(dm3,2) );
    s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
    s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
    s33 = pow(e(13),2) + pow(e(14),2) + 1.0;
    s12 = e(3)*e(8) + e(4)*e(9) + 1.0;
    s13 = e(3)*e(13) + e(4)*e(14) + 1.0;
    s23 = e(8)*e(13) + e(9)*e(14) + 1.0;
    co12 = s12 / sqrt( s11*s22 );
    co13 = s13 / sqrt( s11*s33 );
    co23 = s23 / sqrt( s22*s33 );
      
    p14 = (pow(p1,2) + pow(dm1,2)) * (pow(p4,2) + pow(dm4,2));
    p24 = (pow(p2,2) + pow(dm2,2)) * (pow(p4,2) + pow(dm4,2));
    p34 = (pow(p3,2) + pow(dm3,2)) * (pow(p4,2) + pow(dm4,2));
    s14 = e(3)*e(18) + e(4)*e(19) + 1.0;
    s24 = e(8)*e(18) + e(9)*e(19) + 1.0;
    s34 = e(13)*e(18) + e(14)*e(19) + 1.0;
    s44 = pow(e(18),2) + pow(e(19),2) + 1.0;
    co14 = s14 / sqrt( s11*s44);
    co24 = s24 / sqrt( s22*s44);
    co34 = s34 / sqrt( s33*s44);

    massConstrCalc = pow(dm1,2) + pow(dm2,2) + pow(dm3,2) + pow( dm4,2)
      + 2.0*( sqrt(p12) + sqrt(p13) + sqrt(p14) + sqrt(p23)  +
              sqrt(p24) + sqrt(p34) )
      - 2.*( p1*p2*co12  + p1*p3*co13  + p1*p4*co14 + p2*p3*co23 + 
             p2*p4*co24  + p3*p4*co34 );
    massConstrCalc = sqrt(massConstrCalc);
    
    diffZver = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZver = fabs(diffZver);
    
  }
  
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: chisq
// This method computes the Chisquare of the contrained fit. 
//==================================================================
StatusCode LagrangeMassVertexFitter::chisq(HepVector& e0, HepVector& e,
                                           HepSymMatrix& Ce, double& chis)
  
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " CHISQ !!! " << endreq;
  
  HepVector ed = e0 - e;
  
  
  int inv;
  HepSymMatrix Ge = Ce.inverse(inv); 
  
  if ( inv == 0 ) {
    chis = dot(ed, (Ge*ed));
  }
  else {
    log << MSG::DEBUG << " matrix inversion prob " << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "chi =  " << chis << endreq;  
  if (chis < 0.0) {
    return StatusCode::FAILURE;
  } else {
    return StatusCode::SUCCESS;
  }
}

//==================================================================
// Method name: nwcov
// This method computes the parameters and the covariance matrix of the
// new formed track.
//==================================================================
StatusCode LagrangeMassVertexFitter::nwcov(HepVector& e, HepSymMatrix& Ce,
                                           double zcer, HepVector& tpf,
                                           HepSymMatrix& Cx, int dimCe)
  
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " NWCOV !!! " << endreq;
  
  // variables to simplify notation
  
  double s11 = pow(e(3),2) + pow(e(4),2)  + 1.0 ;
  double s22 = pow(e(8),2) + pow(e(9),2)  + 1.0 ;
  
  double pz1 = e(5)/sqrt(s11);
  double pz2 = e(10)/sqrt(s22);
  double s33, pz3 ;
  
  int ntracks = m_ntracks;  
  
  if(( ntracks == 3 ) || ( ntracks == 4) ){
    s33 = pow(e(13),2) + pow(e(14),2)  + 1.0 ;
    pz3 = e(15)/sqrt(s33);
  } 
  
  double sumpz = pz1 + pz2;
  double sum1pz = e(3)*pz1 + e(8)*pz2;
  double sum2pz = e(4)*pz1 + e(9)*pz2;
  
  if (( ntracks == 3 ) || ( ntracks ==4) ){
    sumpz = sumpz + pz3;
    sum1pz = sum1pz + e(13)*pz3;
    sum2pz = sum2pz + e(14)*pz3;
  }
  
  double s44, pz4;
  
  if ( ntracks == 4) {
    s44 = pow(e(18),2) + pow(e(19),2) + 1.0;
    pz4 = e(20)/sqrt(s44);
    sumpz = sumpz + pz4;
    sum1pz = sum1pz + e(18)*pz4;
    sum2pz = sum2pz + e(19)*pz4;
  }

  double d_3pz1 = -e(3)*pz1/s11;
  double d_4pz1 = -e(4)*pz1/s11;
  double d_5pz1 = 1.0/sqrt(s11);
  
  double d_8pz2 = -e(8)*pz2/s22;
  double d_9pz2 = -e(9)*pz2/s22;
  double d10pz2 = 1.0/sqrt(s22);
  double d13pz3, d14pz3, d15pz3 ;

  if(( ntracks == 3 ) || ( ntracks ==4) ){
    d13pz3 = -e(13)*pz3/s33;
    d14pz3 = -e(14)*pz3/s33;
    d15pz3 = 1.0/sqrt(s33);
  }
  
  double d18pz4, d19pz4, d20pz4;
  
  if ( ntracks == 4 ) {
    d18pz4 = -e(18)*pz4/s44;
    d19pz4 = -e(19)*pz4/s44;
    d20pz4 = 1.0/sqrt(s44);
  }
  
  // definition of the parameters for the resulting particle
  tpf(1) = e(1) + e(3)*(e(6)-e(1))/(e(3)-e(8));
  tpf(2) = e(2) + e(4)*(e(6)-e(1))/(e(3)-e(8));
  tpf(3) = zcer + (e(6)-e(1))/(e(3)-e(8));
  tpf(4) = sum1pz/sumpz;
  tpf(5) = sum2pz/sumpz;
  tpf(6) = sqrt( pow(sum1pz,2) + pow(sum2pz,2) + pow(sumpz,2) );

  // declaration  and definition of the auxiliar matrices´ terms

  HepMatrix Te(6,dimCe,0);

  Te(1,1) = 1. - e(3) / (e(3)-e(8));
  Te(1,3) = -e(8)*(e(6)-e(1)) / pow((e(3)-e(8)),2);
  Te(1,6) =  e(3) / (e(3)-e(8));
  Te(1,8) =  e(3)*(e(6)-e(1)) / pow((e(3)-e(8)),2);
  Te(2,1) = -e(4) / (e(3)-e(8));
  Te(2,2) = 1.0;
  Te(2,3) = -e(4)*(e(6)-e(1)) / pow((e(3)-e(8)),2);
  Te(2,4) =  (e(6)-e(1)) / (e(3)-e(8));
  Te(2,6) = -Te(2,1);
  Te(2,8) = -Te(2,3);
  Te(3,1) = -1. / (e(3)-e(8));
  Te(3,3) = -(e(6)-e(1)) / pow((e(3)-e(8)),2);
  Te(3,6) = -Te(3,1);
  Te(3,8) = -Te(3,3);
  Te(4, 3) = ( pz1 + e(3)*d_3pz1 )/ sumpz - sum1pz*d_3pz1/pow(sumpz,2);
  Te(4, 8) = ( pz2 + e(8)*d_8pz2 )/ sumpz - sum1pz*d_8pz2/ pow(sumpz,2);
  Te(4, 4) = e(3)*d_4pz1/sumpz - sum1pz*d_4pz1/ pow(sumpz,2);
  Te(4, 5) = e(3)*d_5pz1/sumpz - sum1pz*d_5pz1/ pow(sumpz,2);
  Te(4, 9) = e(8)*d_9pz2/sumpz - sum1pz*d_9pz2/ pow(sumpz,2);
  Te(4,10) = e(8)*d10pz2/sumpz - sum1pz*d10pz2/ pow(sumpz,2);
  
  if (( ntracks == 3 ) || ( ntracks == 4) ){
    Te(4,13) =( pz3 + e(13)*d13pz3 )/ sumpz - sum1pz*d13pz3/ pow(sumpz,2);
    Te(4,14) = e(13)*d14pz3/sumpz - sum1pz*d14pz3/ pow(sumpz,2);
    Te(4,15) = e(13)*d15pz3/sumpz - sum1pz*d15pz3/ pow(sumpz,2);
  }

  Te(5, 4) = ( pz1 + e(4)*d_4pz1 )/ sumpz - sum2pz*d_4pz1/ pow(sumpz,2);
  Te(5, 9) = ( pz2 + e(9)*d_9pz2 )/ sumpz - sum2pz*d_9pz2/ pow(sumpz,2);
  Te(5, 3) = e(4)*d_3pz1/sumpz - sum2pz*d_3pz1/ pow(sumpz,2);
  Te(5, 5) = e(4)*d_5pz1/sumpz - sum2pz*d_5pz1/ pow(sumpz,2);
  Te(5, 8) = e(9)*d_8pz2/sumpz - sum2pz*d_8pz2/ pow(sumpz,2);
  Te(5,10) = e(9)*d10pz2/sumpz - sum2pz*d10pz2/ pow(sumpz,2);

  if (( ntracks == 3 ) || ( ntracks == 4) ){
    Te(5,13) = e(14)*d13pz3/sumpz - sum2pz*d13pz3/ pow(sumpz,2);
    Te(5,14) = ( pz3 + e(14)*d14pz3 )/sumpz - sum2pz*d14pz3/ pow(sumpz,2);
    Te(5,15) = e(14)*d15pz3/sumpz - sum2pz*d15pz3/ pow(sumpz,2);
  }

  Te(6, 3) = ( d_3pz1 * ( sumpz + sum1pz*e(3) + sum2pz*e(4) ) +
               sum1pz * pz1 ) / tpf(6);
  Te(6, 4) = ( d_4pz1 * ( sumpz + sum1pz*e(3) + sum2pz*e(4) ) +
               sum2pz * pz1 ) / tpf(6);
  Te(6, 8) = ( d_8pz2 * ( sumpz + sum1pz*e(8) + sum2pz*e(9) ) +
               sum1pz * pz2 ) / tpf(6);
  Te(6, 9) = ( d_9pz2 * ( sumpz + sum1pz*e(8) + sum2pz*e(9) ) +
               sum2pz * pz2 ) / tpf(6);
  Te(6, 5) = ( sumpz + sum1pz*e(3) + sum2pz*e(4) ) * d_5pz1 / tpf(6);
  Te(6,10) = ( sumpz + sum1pz*e(8) + sum2pz*e(9) ) * d10pz2 / tpf(6);

  if (( ntracks == 3 ) || ( ntracks == 4) ){
    Te(6,13) = ( d13pz3 * ( sumpz + sum1pz*e(13) + sum2pz*e(14) ) +
                 sum1pz * pz3 ) / tpf(6);
    Te(6,14) = ( d14pz3 * ( sumpz + sum1pz*e(13) + sum2pz*e(14) ) +
                 sum2pz * pz3 ) / tpf(6);
    Te(6,15) = ( sumpz + sum1pz*e(13) + sum2pz*e(14) ) * d15pz3/ tpf(6);
  }

  if (ntracks == 4) {
    
    Te(4,18) = (pz4 + e(18)*d18pz4)/sumpz - sum1pz*d18pz4/pow(sumpz,2);
    Te(4,19) = (e(18)*d19pz4)/sumpz - sum1pz*d19pz4/pow(sumpz,2);
    Te(4,20) = (e(18)*d20pz4)/sumpz - sum1pz*d20pz4/pow(sumpz,2);
    Te(5,18) = (e(19)*d18pz4)/sumpz - sum2pz*d18pz4/pow(sumpz,2);
    Te(5,19) = (pz4 + e(19)*d19pz4)/sumpz - sum2pz*d19pz4/pow(sumpz,2);
    Te(5,20) = (e(19)*d20pz4)/sumpz - sum2pz*d20pz4/pow(sumpz,2);
    Te(6,18)= ( d18pz4 * ( sumpz + sum1pz*e(18) + sum2pz*e(19) ) +
                sum1pz * pz4 ) / tpf(6);
    Te(6,19)= ( d19pz4 * ( sumpz + sum1pz*e(18) + sum2pz*e(19) ) +
                sum2pz * pz4 ) / tpf(6);
    Te(6,20) = ( sumpz + sum1pz*e(18) + sum2pz*e(19) ) * d20pz4/ tpf(6);
  }
  
  // Make the matrix multiplication to obtain Cx 
  //  Cx =Te*Ce*Te.T;
  Cx = Ce.similarity(Te);
  
  return StatusCode::SUCCESS;
  
}

//==================================================================

