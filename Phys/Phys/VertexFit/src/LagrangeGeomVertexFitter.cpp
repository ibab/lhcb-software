// $Id: LagrangeGeomVertexFitter.cpp,v 1.1, 2002/03/08, modified from gcorti
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
#include "LagrangeGeomVertexFitter.h"
#include "Kernel/IParticleTransporter.h"

//--------------------------------------------------------------------
//
//  ClassName  : LagrangeGeomVertexFitter
// 
//  Description: Performs a constrained fit for two, three or four 
//                 tracks using the Lagrange multipliers technique.
//               It includes the geometrical constrained fit for 2, 3 or 4 
//                 tracks without ressonance. It also includes the
//                 geometrical constrained fit for the mother particle and
//                 the mass constrained fit for the ressonances for the 
//                 cases of 3 or 4 tracks with one ressonance and
//                 4 tracks with two ressonances.   
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
static ToolFactory<LagrangeGeomVertexFitter> s_factory;
const IToolFactory& LagrangeGeomVertexFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
LagrangeGeomVertexFitter::LagrangeGeomVertexFitter(const std::string& type, 
                                                   const std::string& name, 
                                                   const IInterface* parent) 
  : AlgTool( type, name, parent ),
    m_ppSvc(0),
    m_pVertexUnconst(0),
    m_pTransporter(0),
    m_transporterType("CombinedTransporter")  {
  
  declareInterface<IGeomVertexFitter>(this);
  declareProperty("Transporter", m_transporterType);  
}

//==================================================================
// Initialize
//==================================================================
StatusCode LagrangeGeomVertexFitter::initialize() {
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










//==================================================================
//          Perform geometrical constrained vertex fit 
//               (2 tracks without ressonance )
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeom(const std::string& motherName,
                                                 Particle& particle1, 
                                                 Particle& particle2,
                                                 Vertex& constrVtx,
                                                 Particle& motherParticle )
{
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  
  return fitWithGeom(motherName, pList, constrVtx, motherParticle);
  
}
//==================================================================
//         Perform geometrical constrained vertex fit 
//              ( 3 tracks without ressonance )
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeom(const std::string& motherName,
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
  
  return fitWithGeom(motherName, pList, constrVtx, motherParticle);
  
}

//==================================================================
//          Perform geometrical constrained vertex fit 
//               ( 4 tracks without ressonance )
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeom(const std::string& motherName,
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
  
  return fitWithGeom(motherName, pList, constrVtx, motherParticle);
  
}


//==================================================================
// Perform geometrical constrained vertex fit (vector of particles).
// The procedure is implemented only for two, three and four particles
// without ressonance.
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeom(const std::string& motherName,
                                                 const ParticleVector& pList,
                                                 Vertex& constrVtx,
                                                 Particle& motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithGeom " << endreq;
  
  // check the size of the input particle vector.
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  log << MSG::DEBUG << "Number of tracks in fitWithGeom = " 
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
  
  StatusCode sc;
  
  const ParticleID& part1ID = part1->particleID().pid();
  int part1StdHepID = part1ID.pid();
  
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(part1StdHepID  );
  
  // hbar in MeV s
  float hbar = 6.58211889*pow(10,-22);
  
  float part1Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if( (part1->isResonance()) && (part1Width > 10)  ) part1->setIsResonance(0);
  
  const ParticleID& part2ID = part2->particleID().pid();
  int part2StdHepID = part2ID.pid();
  
  partProp = m_ppSvc->findByStdHepID(part2StdHepID  );
  
  float part2Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if((part2->isResonance()) && (part2Width > 10) ) part2->setIsResonance(0);
  
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
    }//if((numbRessTwoTracks != 0) && (numbRessTwoTracks != 1) && 
    // (numbRessTwoTracks != 2)) 
    
    SmartRefVector<Particle>::iterator itPart1;
    SmartRefVector<Particle>::iterator itPart2; 
    ParticleVector p3Tracks;
    ParticleVector pTracksPart1Daughters;
    ParticleVector pTracksPart2Daughters;
    double subMass1;
    double subMass2; 
    
    if(numbRessTwoTracks == 0 ){
      
      sc = fitWithGeomWithoutRess(motherName, pList, constrVtx, 
                                  motherParticle); 
      
    }//if(numbRessTwoTracks == 0)
    
    
    if((numbRessTwoTracks == 1) && Part1TwoTracks){
      
      for ( itPart1 = part1->endVertex()->products().begin();
            itPart1 != part1->endVertex()->products().end(); itPart1++ ){
        
        p3Tracks.push_back(*itPart1);
        pTracksPart1Daughters.push_back(*itPart1); 
        subMass1 = part1->mass();
        
      }//for itPart1
      
      p3Tracks.push_back(part2);
      
      sc =  fitWithGeomWithoutRess(motherName, p3Tracks, constrVtx, 
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
      
      
      sc =   fitWithGeomWithoutRess(motherName, p3Tracks, constrVtx, 
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
      
      sc =   fitWithGeomWithoutRess(motherName, p3Tracks, constrVtx, 
                                    motherParticle); 
      
    }//if(numbRessTwoTracks == 2)
    
  }//if(ntracks == 2)
  
  if(ntracks == 3){
    
    const ParticleID& part3ID = part3->particleID().pid();
    int part3StdHepID = part3ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part3StdHepID  );
    
    // hbar in MeV s
    float hbar = 6.58211889*pow(10,-22);
    
    float part3Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
    
    if((part3->isResonance()) && (part3Width > 10) ) part3->setIsResonance(0);
    
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
    }//if((numbRessThreeTracks != 0) && (numbRessThreeTracks != 1))  
    
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
    
    if(numbRessThreeTracks == 0 ){
      
      sc =  fitWithGeomWithoutRess(motherName, pList, constrVtx, 
                                   motherParticle); 
      
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
      
      sc =  fitWithGeomWithoutRess(motherName, p3Tracks, constrVtx, 
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
      
      sc = fitWithGeomWithoutRess(motherName, p3Tracks, constrVtx, 
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
      
      sc =  fitWithGeomWithoutRess(motherName, p3Tracks, constrVtx, 
                                   motherParticle); 
      
    }//if((numbRessThreeTracks == 1) && Part3ThreeTracks)
    
  }//if(ntracks == 3)  
  
  if(ntracks == 4){
    
    const ParticleID& part3ID = part3->particleID().pid();
    int part3StdHepID = part3ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part3StdHepID  );
    
    float part3Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
    
    if((part3->isResonance()) && (part3Width > 10) ) part3->setIsResonance(0);
    
    const ParticleID& part4ID = part4->particleID().pid();
    int part4StdHepID = part4ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part4StdHepID  );
    
    float part4Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
    
    if((part4->isResonance()) && (part4Width > 10) ) part4->setIsResonance(0);
    
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
      
      sc =   fitWithGeomWithoutRess(motherName, pList, constrVtx, 
                                    motherParticle );	
      
    }//if(numbRessFourTracks == 0)
    
  }//if(ntracks == 4)
  
  if( sc.isSuccess() ) {
    
    ParticleVector::const_iterator iterP;
    
    for(iterP = pList.begin(); iterP != pList.end(); iterP++) {
      constrVtx.addToProducts(*iterP);
    }//for iterP
    
    return StatusCode::SUCCESS; 
  }//if( sc.isSuccess() )
  else{
    return StatusCode::FAILURE; 
  }//else
  
  
  
}












//==================================================================
// Perform geometrical constrained vertex fit (vector of particles).
// The procedure is implemented only for two, three and four particles
// without ressonance.
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeomWithoutRess(const std::string& 
                                                            motherName,
                                                            const 
                                                            ParticleVector& 
                                                            pList,
                                                            Vertex& constrVtx,
                                                            Particle& 
                                                            motherParticle )
  
  
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithGeomWithoutRess " 
      << endreq;
  
  // check the size of the input particle vector.
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  log << MSG::DEBUG << "Number of tracks in fitWithGeomWithoutRess = " 
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
    
  }//for iterP
  
  
  // copy parameter vector
  HepVector e0(e); 
  
  //Chisquare of the fit.
  double  chis;
  
  // Parameters vector of the resulting track(x,y,z,x',y',p,E).
  HepVector tpf(7);
  
  bool final = false;
  int  icount= 0; 
  while (!final && (icount < NMAXITERA)) {
    
    StatusCode sc_iter = itera(e, Ce, dimCe, final);
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
    
    // diffZVer and DIFFZVERCUT
    double DIFFZVERCUT = 1.0e-6;
    HepVector diffZVer(6,0);    
    
    StatusCode sc_evalu =  evalu(e, diffZVer);
    if ( !sc_evalu.isSuccess() ) {
      log << MSG::DEBUG << "constraints calc failed " << endreq;
      return StatusCode::FAILURE;
    }
    
    icount = icount + 1;
    
    // finish if constraint is satisfied
    if( ntracks == 2){
      
      if ( diffZVer(1) < DIFFZVERCUT ){
        final=true;
      }  
    }//if ntracks = 2
    
    if( ntracks == 3){
      
      if ( (diffZVer(1) < DIFFZVERCUT) && (diffZVer(2) < DIFFZVERCUT) &&
           (diffZVer(3) < DIFFZVERCUT) ){
        final=true;
      }  
    }//if ntracks = 3
    
    if( ntracks == 4){
      
      if ( (diffZVer(1) < DIFFZVERCUT) && (diffZVer(2) < DIFFZVERCUT) &&
           (diffZVer(3) < DIFFZVERCUT) && (diffZVer(4) < DIFFZVERCUT) &&
           (diffZVer(5) < DIFFZVERCUT) ){
        final=true;
      }  
    }//if ntracks = 4
    
  }
  
  // if not converged, return
  if(!final) {
    log << MSG::DEBUG << " no convergence  " << endreq;
    log << MSG::DEBUG << " icount = " << icount << endreq;
    return StatusCode::FAILURE; 
  }
  
  
  // Minimum found. Get the chisq of the fit.
  
  StatusCode sc_chisq = chisq( e0, e, Ce, chis);
  if ( !sc_chisq.isSuccess() ) {
    log << MSG::DEBUG << "chisq  calc failed " << endreq;
    return StatusCode::FAILURE;  
  }
  
  // update the covariance matrix for old parameters (here final=true)
  
  StatusCode sc_itera =  itera(e,Ce,dimCe,final);
  if ( !sc_itera.isSuccess() ) {
    log << MSG::DEBUG << " itera failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  // find the covariance matrix of the mother particle
  
  StatusCode sc_nwcov = nwcov(mass, e, Ce, zcer, tpf, Cx, dimCe);
  if ( !sc_nwcov.isSuccess() ) {
    log << MSG::DEBUG << " nwcov failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  log << MSG::DEBUG << " lagrange: chis = " << chis << endreq;
  
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
  
  if(ntracks == 2){
    constrVtx.setNDoF(ntracks-1);
  }
  
  if(ntracks == 3){
    constrVtx.setNDoF(ntracks);
  }
  
  if(ntracks == 4){
    constrVtx.setNDoF(ntracks+1);
  }
  
  //    decay with constrained mass
  constrVtx.setType(Vertex::DecayWithMass);
  //    particles that compose the vertex
  
  //  for(iterP = pList.begin(); iterP != pList.end(); 
  //      iterP++) {
  //    constrVtx.addToProducts(*iterP);
  //  }
  
  // calculate mother particle momentum
  HepLorentzVector motherHepVec;
  motherHepVec.setPz( tpf(6)/sqrt(pow(tpf(4),2)+pow(tpf(5),2)+1.0));
  motherHepVec.setPx( tpf(4)*motherHepVec.pz());
  motherHepVec.setPy( tpf(5)*motherHepVec.pz());
  motherHepVec.setE( tpf(7)); 
  
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
  motherParticle.setMass(motherParticle.momentum().mag());
  //    massErr??
  //    check ressonance
  
  // hbar in MeV s
  float hbar = 6.58211889*pow(10,-22);
  
  float motherParticleWidth = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if( (motherParticleWidth < 10) && ((*partProp).lifetime()*pow(10,-9) < 
                                     pow(10,-15))){
    
    motherParticle.setIsResonance(1);
    
  }//isRessonance
  
  motherParticle.setEndVertex(&constrVtx);
  
  return StatusCode::SUCCESS;
}
















//==================================================================
//   Perform geometrical and sub mass constrained vertex fit 
//               (2 tracks with ressonances )
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeomAndSubMass(const std::string& 
                                                           motherName,
                                                           Particle& 
                                                           particle1, 
                                                           Particle& particle2,
                                                           Vertex& constrVtx,
                                                           Particle& 
                                                           motherParticle )
{
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  
  return fitWithGeomAndSubMass(motherName, pList, constrVtx, motherParticle);
  
}
//==================================================================
//  Perform geometrical and sub mass constrained vertex fit 
//              ( 3 tracks with ressonance )
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeomAndSubMass( const std::string& 
                                                            motherName,
                                                            Particle& 
                                                            particle1, 
                                                            Particle& 
                                                            particle2, 
                                                            Particle& 
                                                            particle3,
                                                            Vertex& constrVtx,
                                                            Particle& 
                                                            motherParticle )
{
  ParticleVector pList;
  pList.push_back(&particle1);
  pList.push_back(&particle2);
  pList.push_back(&particle3);
  
  return fitWithGeomAndSubMass(motherName, pList, constrVtx, motherParticle);
  
}

//==================================================================
// Perform geometrical and sub mass constrained vertex 
//fit(vector of particles).
// The procedure is implemented only for two and three particles
// with ressonances.
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeomAndSubMass(const std::string& 
                                                           motherName,
                                                           const 
                                                           ParticleVector& 
                                                           pList,
                                                           Vertex& constrVtx,
                                                           Particle& 
                                                           motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithGeomAndSubMass " 
      << endreq;
  
  // check the size of the input particle vector.
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  
  log << MSG::DEBUG << "Number of tracks in fitWithGeomAndSubMass = " 
      << ntracks << endreq;
  
  if (( ntracks != 2 ) && ( ntracks != 3 )) {
    log << MSG::INFO << "Number of tracks must be 2 or 3  " << endreq;
    return StatusCode::FAILURE;
  }
  
  int numbRessTwoTracks = 0;
  int numbRessThreeTracks = 0;
  bool Part1TwoTracks = false;
  bool Part2TwoTracks = false;
  bool Part1ThreeTracks = false;
  bool Part2ThreeTracks = false;
  bool Part3ThreeTracks = false;
  
  Particle* part1 = pList[0];
  Particle* part2 = pList[1];
  Particle* part3 = pList[2];
  
  StatusCode sc;
  
  const ParticleID& part1ID = part1->particleID().pid();
  int part1StdHepID = part1ID.pid();
  
  ParticleProperty*  partProp = m_ppSvc->findByStdHepID(part1StdHepID  );
  
  // hbar in MeV s
  float hbar = 6.58211889*pow(10,-22);
  
  float part1Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if((part1->isResonance()) && (part1Width > 10) ) part1->setIsResonance(0);
  
  const ParticleID& part2ID = part2->particleID().pid();
  int part2StdHepID = part2ID.pid();
  
  partProp = m_ppSvc->findByStdHepID(part2StdHepID  );
  
  float part2Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
  
  if((part2->isResonance()) && (part2Width > 10) ) part2->setIsResonance(0);
  
  if(ntracks == 2){
    
    if(part1->isResonance()){
      numbRessTwoTracks++;
      Part1TwoTracks = true;
    }//if(part1->isResonance()) 
    
    
    if(part2->isResonance()){
      numbRessTwoTracks++;
      Part2TwoTracks = true;
    }//if(part2->isResonance())
    
    if ( (numbRessTwoTracks != 1) && (numbRessTwoTracks != 2)  ) {    
      log << MSG::INFO << "Number of ressonances must be 1 or 2  " << endreq;
      
      return StatusCode::FAILURE;
    }//if((numbRessTwoTracks != 1) && (numbRessTwoTracks != 2)) 
    
    SmartRefVector<Particle>::iterator itPart1;
    SmartRefVector<Particle>::iterator itPart2; 
    ParticleVector p3Tracks;
    ParticleVector pTracksPart1Daughters;
    ParticleVector pTracksPart2Daughters;
    double subMass1;
    double subMass2; 
    
    if((numbRessTwoTracks == 1) && Part1TwoTracks){
      
      for ( itPart1 = part1->endVertex()->products().begin();
            itPart1 != part1->endVertex()->products().end(); itPart1++ ){
        
        p3Tracks.push_back(*itPart1);
        pTracksPart1Daughters.push_back(*itPart1); 
        subMass1 = part1->mass();
        
      }//for itPart1
      
      p3Tracks.push_back(part2);
      
      sc = fitWithGeomAndOneSubMass(motherName, subMass1, 
                                    pTracksPart1Daughters, 
                                    p3Tracks, constrVtx,motherParticle); 
      
    }//if((numbRessTwoTracks == 1) && Part1TwoTracks)
    
    if((numbRessTwoTracks == 1) && Part2TwoTracks){
      
      for ( itPart2 = part2->endVertex()->products().begin();
            itPart2 != part2->endVertex()->products().end(); itPart2++ ){
        
        p3Tracks.push_back(*itPart2);
        pTracksPart2Daughters.push_back(*itPart2); 
        subMass2 = part2->mass();
        
      }//for itPart2
      
      p3Tracks.push_back(part1);
      
      sc = fitWithGeomAndOneSubMass(motherName, subMass2, 
                                    pTracksPart2Daughters,
                                    p3Tracks, constrVtx, motherParticle); 
      
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
      
      sc = fitWithGeomAndTwoSubMass(motherName, subMass1, subMass2,
                                    pTracksPart1Daughters, 
                                    pTracksPart2Daughters, 
                                    p3Tracks, constrVtx, motherParticle); 
      
    }//if(numbRessTwoTracks == 2)
    
  }//if(ntracks == 2)
  
  if(ntracks == 3){
    
    const ParticleID& part3ID = part3->particleID().pid();
    int part3StdHepID = part3ID.pid();
    
    partProp = m_ppSvc->findByStdHepID(part3StdHepID  );
    
    float part3Width = hbar/(pow(10,-9)*((*partProp).lifetime()));
    
    if((part3->isResonance()) && (part3Width > 10) ) part3->setIsResonance(0);
    
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
    
    if ( numbRessThreeTracks != 1  ) {
      log << MSG::INFO << "Number of ressonances must be 1  " << endreq;
      return StatusCode::FAILURE;
    }//if(numbRessThreeTracks != 1)  
    
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
    
    if((numbRessThreeTracks == 1) && Part1ThreeTracks){
      
      for ( itPart1 = part1->endVertex()->products().begin();
            itPart1 != part1->endVertex()->products().end(); itPart1++ ){
        
        p3Tracks.push_back(*itPart1);
        pTracksPart1Daughters.push_back(*itPart1); 
        subMass1 = part1->mass();
        
      }//for itPart1
      
      p3Tracks.push_back(part2);
      
      p3Tracks.push_back(part3);
      
      sc = fitWithGeomAndOneSubMass(motherName,subMass1,
                                    pTracksPart1Daughters,
                                    p3Tracks, constrVtx, motherParticle); 
      
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
      
      sc = fitWithGeomAndOneSubMass(motherName, subMass2,
                                    pTracksPart2Daughters, 
                                    p3Tracks, constrVtx, motherParticle); 
      
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
      
      sc = fitWithGeomAndOneSubMass(motherName, subMass3, 
                                    pTracksPart3Daughters, 
                                    p3Tracks, constrVtx, motherParticle); 
      
    }//if((numbRessThreeTracks == 1) && Part3ThreeTracks)
    
  }//if(ntracks == 3)
  
  if( sc.isSuccess() ) {
    
    ParticleVector::const_iterator iterP;
    
    for(iterP = pList.begin(); iterP != pList.end(); iterP++) {
      constrVtx.addToProducts(*iterP);
    }//for iterP
    
    return StatusCode::SUCCESS; 
    
  }//if( sc.isSuccess() )
  else{
    return StatusCode::FAILURE; 
  }//else
  
}














//==================================================================
// Perform geometrical and sub mass constrained vertex 
// fit(vector of particles).
// The procedure is implemented for three or four tracks
// with one ressonance.
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeomAndOneSubMass(const std::
                                                              string& 
                                                              motherName,
                                                              double subMass,
                                                              const 
                                                              ParticleVector&
                                                              pDaughter,
                                                              const 
                                                              ParticleVector& 
                                                              pList,
                                                              Vertex& 
                                                              constrVtx,
                                                              Particle& 
                                                              motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithGeomAndOneSubMass " 
      << endreq;
  
  // check the size of the input particle vector.  
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  
  log << MSG::DEBUG << "Number of tracks in fitWithGeomAndOneSubMass = " 
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
  
  // number of iterations ( = NMAXITERA ).
  int const NMAXITERA = 20 ;
  
  // maximum number of tracks ( = NMAXTRK ).
  int const NMAXTRK   = 4;
  
  // mass of the daughter particles
  HepVector mass(NMAXTRK);
  
  // dimension of the covariance matrix for 3 or 4 tracks.
  int dimCe = ntracks*TRCOVDIM;
  
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
  
  // Calculated constrained mass from the daughters of the ressonances. 
  double subMassConstrCalc;
  
  //Chisquare of the fit.
  double  chis;
  
  // Parameters vector of the resulting track.
  HepVector tpf(6);
  
  bool final = false;
  int  icount= 0; 
  
  // diffZVer and diffSubMass
  HepVector diffZVer(6,0);
  double diffSubMass = 0;
  
  while (!final && (icount < NMAXITERA)) {
    
    StatusCode sc_iter = itera(e, Ce, pDaughter ,  
                               subMassConstr, dimCe,final);
    
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
    
    double DIFFZVERCUT = 1.0e-6;
    double DIFFSUBMASSCUT = 1.0e-6;   
    
    StatusCode sc_evalu=  evalu(e,pDaughter,
                                subMassConstrCalc,diffZVer);
    if ( !sc_evalu.isSuccess() ) {
      log << MSG::DEBUG << "constraints calc failed " << endreq;
      return StatusCode::FAILURE;
    }
    
    // finish if constraints are satisfied
    diffSubMass = fabs((subMassConstr - subMassConstrCalc)/subMassConstr);
    
    icount = icount + 1;
    
    if( ntracks == 3){
      
      if ( (diffZVer(1) < DIFFZVERCUT) && (diffZVer(2) < DIFFZVERCUT) &&
           (diffZVer(3) < DIFFZVERCUT)
           && (diffSubMass < DIFFSUBMASSCUT)){
        final=true;
      }  
    }//if ntracks = 3
    
    if( ntracks == 4){
      
      if ( (diffZVer(1) < DIFFZVERCUT) && (diffZVer(2) < DIFFZVERCUT) &&
           (diffZVer(3) < DIFFZVERCUT) && (diffZVer(4) < DIFFZVERCUT) &&
           (diffZVer(5) < DIFFZVERCUT)
           && (diffSubMass < DIFFSUBMASSCUT)){
        final=true;
      }  
    }//if ntracks = 4
    
  }
  
  // if not converged, return
  if(!final) {
    log << MSG::DEBUG << " no convergence  " << endreq;
    log << MSG::DEBUG << " subMassConstrCalc = " << subMassConstrCalc 
        << endreq;
    log << MSG::DEBUG << " diffSubMass = " << diffSubMass << endreq;
    log << MSG::DEBUG << " diffZVer = " << diffZVer << endreq;
    log << MSG::DEBUG << " icount = " << icount << endreq;
    return StatusCode::FAILURE; 
  }
  
  // later check the diffZVer
  
  // Minimum found. Get the chisq of the fit.
  
  StatusCode sc_chisq = chisq( e0, e, Ce, chis);
  if ( !sc_chisq.isSuccess() ) {
    log << MSG::DEBUG << "chisq  calc failed " << endreq;
    return StatusCode::FAILURE;  
  }
  
  // update the covariance matrix for old parameters (here final=true)
  StatusCode sc_itera =  itera(e,Ce,pDaughter,subMassConstr,dimCe,final);
  if ( !sc_itera.isSuccess() ) {
    log << MSG::DEBUG << " itera failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  // find the covariance matrix of the mother particle
  StatusCode sc_nwcov = nwcov(mass, e, Ce, zcer, tpf, Cx, dimCe);
  if ( !sc_nwcov.isSuccess() ) {
    log << MSG::DEBUG << " nwcov failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  log << MSG::DEBUG << " lagrange: chis = " << chis << endreq;
  log << MSG::DEBUG << " lagrange: subMass  = " << subMassConstrCalc << endreq;
  
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
  
  if(ntracks == 3){
    constrVtx.setNDoF(ntracks+1);
  }
  
  if(ntracks == 4){
    constrVtx.setNDoF(ntracks+2);
  }
  
  //    decay with constrained mass
  constrVtx.setType(Vertex::DecayWithMass);
  //    particles that compose the vertex
  
  //  for(iterP = pList.begin(); iterP != pList.end(); 
  //      iterP++) {
  //    constrVtx.addToProducts(*iterP);
  //  }
  
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
  motherParticle.setMass(motherParticle.momentum().mag());
  //    massErr??
  //    check ressonance
  
  if((*partProp).lifetime()*pow(10,-9) < pow(10,-15)){
    
    motherParticle.setIsResonance(1);
    
  }//isRessonance
  
  motherParticle.setEndVertex(&constrVtx);
  
  
  return StatusCode::SUCCESS;
}

// end of the first one ( fitWithGeomAndOneSubMass method )














//==================================================================
// Perform geometrical and sub mass constrained vertex 
// fit(vector of particles).
// The procedure is implemented for four particles
// with two ressonances.
//==================================================================
StatusCode LagrangeGeomVertexFitter::fitWithGeomAndTwoSubMass(const std::
                                                              string& 
                                                              motherName,
                                                              double subMass1,
                                                              double subMass2,
                                                              const 
                                                              ParticleVector& 
                                                              pDaughter1,
                                                              const 
                                                              ParticleVector& 
                                                              pDaughter2,
                                                              const 
                                                              ParticleVector& 
                                                              pList,
                                                              Vertex& 
                                                              constrVtx,
                                                              Particle& 
                                                              motherParticle )
{
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " lagrange fitter starting - fitWithGeomAndTwoSubMass " 
      << endreq;
  
  // check the size of the input particle vector.  
  m_ntracks = pList.size();
  int ntracks = m_ntracks;
  
  log << MSG::DEBUG << "Number of tracks in fitWithGeomAndTwoSubMass = " 
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
  
  // number of iterations ( = NMAXITERA ).
  int const NMAXITERA = 20 ;
  
  // dimension of the covariance matrix for 4 tracks.
  int dimCe = ntracks*TRCOVDIM;
  
  // maximum number of tracks ( = NMAXTRK ).
  int const NMAXTRK   = 4;
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
  
  // diffZVer, diffSubMass1 and diffSubMass2
  HepVector diffZVer(6,0);
  double diffSubMass1 = 0;
  double diffSubMass2 = 0;
  double DIFFZVERCUT = 1.0e-6;
  double DIFFSUBMASSCUT = 1.0e-6;
  
  // Calculated constrained masses from the daughters of the ressonances. 
  double subMass1ConstrCalc;
  double subMass2ConstrCalc;
  
  //Chisquare of the fit.
  double  chis;
  
  // Parameters vector of the resulting track.
  HepVector tpf(6);
  
  bool final = false;
  int  icount= 0; 
  while (!final && (icount < NMAXITERA)) {
    
    StatusCode sc_iter = itera(e, Ce,pDaughter1,pDaughter2, subMass1Constr, 
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
    
    StatusCode sc_evalu=  evalu(e,pDaughter1,pDaughter2, subMass1ConstrCalc, 
                                subMass2ConstrCalc,diffZVer);
    
    if ( !sc_evalu.isSuccess() ) {
      log << MSG::DEBUG << "constraints calc failed " << endreq;
      return StatusCode::FAILURE;
    }
    
    
    // finish if constraints are satisfied
    diffSubMass1 = fabs((subMass1Constr - subMass1ConstrCalc)/subMass1Constr);   
    diffSubMass2 = fabs((subMass2Constr - subMass2ConstrCalc)/subMass2Constr);
    
    icount = icount + 1;
    
    if( ntracks == 4){
      
      if ( (diffZVer(1) < DIFFZVERCUT) && (diffZVer(2) < DIFFZVERCUT) &&
           (diffZVer(3) < DIFFZVERCUT) && (diffZVer(4) < DIFFZVERCUT) &&
           (diffZVer(5) < DIFFZVERCUT) && (diffSubMass1 < DIFFSUBMASSCUT)
           && (diffSubMass2 < DIFFSUBMASSCUT))
        {
          final=true;
        }  
    }//if ntracks = 4
    
    
  }
  
  // if not converged, return
  if(!final) {
    log << MSG::DEBUG << " no convergence  " << endreq;
    log << MSG::DEBUG << " subMass1ConstrCalc = " << subMass1ConstrCalc 
        << endreq;
    log << MSG::DEBUG << " subMass2ConstrCalc = " << subMass2ConstrCalc 
        << endreq;
    log << MSG::DEBUG << " icount = " << icount << endreq;
    return StatusCode::FAILURE; 
  }
  
  // Minimum found. Get the chisq of the fit.
  
  StatusCode sc_chisq = chisq( e0, e, Ce, chis);
  if ( !sc_chisq.isSuccess() ) {
    log << MSG::DEBUG << "chisq  calc failed " << endreq;
    return StatusCode::FAILURE;  
  }
  
  // update the covariance matrix for old parameters (here final=true)
  StatusCode sc_itera = 
    itera(e,Ce,pDaughter1,pDaughter2,subMass1Constr,
          subMass2Constr,dimCe,final);
  if ( !sc_itera.isSuccess() ) {
    log << MSG::DEBUG << " itera failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  // find the covariance matrix of the mother particle
  StatusCode sc_nwcov = nwcov(mass, e, Ce, zcer, tpf, Cx, dimCe);
  if ( !sc_nwcov.isSuccess() ) {
    log << MSG::DEBUG << " nwcov failed  " << endreq;
    return StatusCode::FAILURE;  
  }
  
  log << MSG::DEBUG << " lagrange: chis = " << chis << endreq;
  log << MSG::DEBUG << " lagrange: submass1  = " << subMass1ConstrCalc 
      << endreq;
  log << MSG::DEBUG << " lagrange: submass2  = " << subMass2ConstrCalc 
      << endreq;
  
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
  
  constrVtx.setNDoF(ntracks+3);
  
  //    decay with constrained mass
  constrVtx.setType(Vertex::DecayWithMass);
  //    particles that compose the vertex
  //  for(iterP = pList.begin(); iterP != pList.end(); 
  //      iterP++) {
  //    constrVtx.addToProducts(*iterP);
  //  }
  
  
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
  motherParticle.setMass(motherParticle.momentum().mag());
  //    massErr??  
  //    check ressonance
  
  if((*partProp).lifetime()*pow(10,-9) < pow(10,-15)){
    
    motherParticle.setIsResonance(1);
    
  }//isRessonance
  
  //  motherParticle.setIsResonance(1);
  
  motherParticle.setEndVertex(&constrVtx);
  
  
  return StatusCode::SUCCESS;
}

// end of the second one ( fitWithGeomAndTwoSubMass method )










//==================================================================
// Method name: itera ( 1 ressonance )
// This method performs one iteration of the constrained fit for 3
// or 4 tracks with one ressonance. 
// It returns the parameters vector (e) and the covariance matrix (Ce) 
// after one iteration of the contrained fit.  
//==================================================================
StatusCode LagrangeGeomVertexFitter::itera(HepVector& e, HepSymMatrix& Ce,
                                           const ParticleVector& pDaughter,
                                           double subMassConstr,
                                           int dimCe, bool final) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ITERA !!! " << endreq;
  
  int ntracks = m_ntracks;
  HepMatrix B;
  HepVector cte;
  
  if ( ntracks == 3) {
    HepMatrix B3(ntracks+1,dimCe,0);
    HepVector cte3(ntracks+1);
    
    B = B3;
    cte = cte3;
  }
  
  
  if ( ntracks == 4) {
    HepMatrix B4(ntracks+2,dimCe,0);
    HepVector cte4(ntracks+2);
    
    B = B4;
    cte = cte4;
  }
  
  
  double p1 = e(5);
  double p2 = e(10);
  
  Particle* daughter1 = pDaughter[0];
  Particle* daughter2 = pDaughter[1];
  
  double dm1 = daughter1->mass();
  double dm2 = daughter2->mass();
  double subm = subMassConstr;
  
  //several combinations of momenta and slopes to abreviate calculus
  double p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2));  
  double s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double s22 = pow(e(8),2) + pow(e(9),2) + 1.0;  
  double s12 = e(3)*e(8) + e(4)*e(9) + 1.0;
  double co12 = s12 / sqrt( s11*s22 );
  
  double beta = ( pow(subm,2) - pow(dm1,2) - pow(dm2,2) ) / 2.0 ;
  
  // Partial derivatives of 'sqrt' of momenta combinations.
  
  double d_5sp12 = p1 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                              ( pow(p1,2) + pow(dm1,2) ) );
  double d10sp12 = p2 * sqrt( (pow(p1,2) + pow(dm1,2)) / 
                              ( pow(p2,2) + pow(dm2,2) ) );
  
  // Partial derivatives for the cosines.
  
  double d_3co12 = ( e(8) - e(3)*s12/s11 ) / sqrt( s11*s22 );
  double d_4co12 = ( e(9) - e(4)*s12/s11 ) / sqrt( s11*s22 );
  double d_8co12 = ( e(3) - e(8)*s12/s22 ) / sqrt( s11*s22 );
  double d_9co12 = ( e(4) - e(9)*s12/s22 ) / sqrt( s11*s22 );
  
  // Independent constant vector in the Taylor expansion of the
  // constrained equation. 
  
  if ( ntracks == 3 ){
    cte(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    cte(3) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));       
    cte(4) = sqrt(p12) - p1*p2*co12 - beta;
  }
  
  if ( ntracks == 4 ){
    cte(1) = (e(6)  - e(1))*(e(4) - e(9))  - (e(7)  - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));  
    cte(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));                    
    cte(4) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    cte(5) = (e(6) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));     
    cte(6) = sqrt(p12) - p1*p2*co12 - beta;    
  }
  
  log << MSG::DEBUG << "lagrange constrained equations for one ressonance" 
      << endreq;
  log << MSG::DEBUG << cte << endreq;
  
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
    
    B(3, 1) = e(14) - e(4);
    B(3, 2) = e(3)  - e(8);
    B(3, 3) = e(2)  - e(12);
    B(3, 4) = e(6) - e(1);
    B(3,6) = -B(3,1);
    B(3,8) = -B(3,3);
    B(3,12) = -B(3,2);
    B(3,14) = -B(3,4);
    
    B(4, 3) = -p1*p2*d_3co12;
    B(4, 4) = -p1*p2*d_4co12;
    B(4, 8) = -p1*p2*d_8co12;
    B(4, 9) = -p1*p2*d_9co12;
    B(4, 5) = d_5sp12 - p2*co12;
    B(4,10) = d10sp12 - p1*co12;    
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
    
    B(4, 1) = e(14) - e(4);
    B(4, 2) = e(3)  - e(8);
    B(4, 3) = e(2)  - e(12);
    B(4, 4) = e(6) - e(1);
    B(4,6) = - B(4, 1);
    B(4,8) = - B(4, 3);
    B(4,12) = - B(4, 2);
    B(4,14) = - B(4, 4);
    
    B(5, 1) = e(19) - e(4);
    B(5, 2) = e(3)  - e(8);
    B(5, 3) = e(2)  - e(17);
    B(5, 4) = e(6) - e(1);
    B(5,6) = - B(5, 1);
    B(5,8) = - B(5, 3);
    B(5,17) = - B(5, 2);
    B(5,19) = - B(5, 4);
    
    B(6, 3) = -p1*p2*d_3co12;
    B(6, 4) = -p1*p2*d_4co12;
    B(6, 8) = -p1*p2*d_8co12;
    B(6, 9) = -p1*p2*d_9co12;
    B(6, 5) = d_5sp12 - p2*co12;
    B(6,10) = d10sp12 - p1*co12;     
  }
  
  //  Du = Ce*Bt
  
  HepMatrix Bt=B.T();
  
  HepMatrix Du = Ce*Bt;
  
  //  Dv = B*Ce*Bt
  HepSymMatrix  Dv =Ce.similarity(B);
  
  log << MSG::DEBUG << "Dv = " << endreq;
  
  log << MSG::DEBUG << Dv << endreq;
  
  //  Dvi = Dv inverse
  
  int inv;
  HepSymMatrix Dvi = Dv.inverse(inv);
  
  log << MSG::DEBUG << " inv = " << inv << endreq;
  
  log << MSG::DEBUG << "Dvi = " << endreq;
  
  log << MSG::DEBUG << Dvi << endreq;
  
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
StatusCode LagrangeGeomVertexFitter::itera(HepVector& e, HepSymMatrix& Ce,
                                           const ParticleVector& pDaughter1,
                                           const ParticleVector& pDaughter2,
                                           double subMass1Constr, 
                                           double subMass2Constr,
                                           int dimCe, bool final) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ITERA !!! " << endreq;
  
  int ntracks = m_ntracks;  
  HepMatrix B(ntracks+3,dimCe,0);
  HepVector cte(ntracks+3);
  
  double p1 = e(5);
  double p2 = e(10); 
  double p3 = e(15);
  double p4 = e(20);
  
  Particle* daughter1 = pDaughter1[0];
  Particle* daughter2 = pDaughter1[1];
  Particle* daughter3 = pDaughter2[0];
  Particle* daughter4 = pDaughter2[1];
  
  double dm1 = daughter1->mass();
  double dm2 = daughter2->mass();    
  double dm3 = daughter3->mass();
  double dm4 = daughter4->mass();
  double subm1 = subMass1Constr;
  double subm2 = subMass2Constr;
  
  //several combinations of momenta and slopes to abreviate calculus
  
  double p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2));
  double s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
  double s12 = e(3)*e(8) + e(4)*e(9) + 1.0;
  double co12 = s12 / sqrt( s11*s22 );
  
  double p34 = p34 = (pow(p3,2) + pow(dm3,2)) * (pow(p4,2) + pow(dm4,2)); 
  double s33 = pow(e(13),2) + pow(e(14),2) + 1.0;
  double s44 = pow(e(18),2) + pow(e(19),2) + 1.0; 
  double s34 = e(13)*e(18) + e(14)*e(19) + 1.0; 
  double co34 = s34 / sqrt( s33*s44);
  
  double beta1 = ( pow(subm1,2) - pow(dm1,2) - pow(dm2,2) ) / 2.0 ; 
  double beta2 = ( pow(subm2,2) - pow(dm3,2) - pow(dm4,2) ) / 2.0 ;
  
  // Independent constant vector in the Taylor expansion of the
  // constraint equation. 
  
  cte(1) = (e(6)  - e(1))*(e(4) - e(9))  - (e(7)  - e(2))*(e(3) - e(8));
  cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));  
  cte(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));              
  cte(4) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
  cte(5) = (e(6) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));     
  cte(6) = sqrt( p12 ) - p1*p2*co12 - beta1;
  cte(7) = sqrt( p34 ) - p3*p4*co34 - beta2;
  
  log << MSG::DEBUG << "lagrange constrained equations for 4 tracks with two ressonances" << endreq;
  log << MSG::DEBUG << cte << endreq;
  
  // Partial derivatives of 'sqrt' of momenta combinations.
  
  double d_5sp12 = p1 * sqrt( (pow(p2,2) + pow(dm2,2)) / 
                              ( pow(p1,2) + pow(dm1,2) ) );
  double d10sp12 = p2 * sqrt( (pow(p1,2) + pow(dm1,2)) / 
                              ( pow(p2,2) + pow(dm2,2) ) );
  double d15sp34 = p3 * sqrt( (pow(p4,2) + pow(dm4,2)) / 
                              ( pow(p3,2) + pow(dm3,2) ) );    
  double d20sp34 = p4 * sqrt( (pow(p3,2) + pow(dm3,2)) / 
                              ( pow(p4,2) + pow(dm4,2) ) ); 			      
  
  // Partial derivatives for the cosines.
  
  double d_3co12 = ( e(8) - e(3)*s12/s11 ) / sqrt( s11*s22 );
  double d_4co12 = ( e(9) - e(4)*s12/s11 ) / sqrt( s11*s22 );
  double d_8co12 = ( e(3) - e(8)*s12/s22 ) / sqrt( s11*s22 );
  double d_9co12 = ( e(4) - e(9)*s12/s22 ) / sqrt( s11*s22 );
  double d13co34 = ( e(18) - e(13)*s34/s33) / sqrt( s33*s44 );
  double d14co34 = ( e(19) - e(14)*s34/s33) / sqrt( s33*s44 );
  double d18co34 = ( e(13) - e(18)*s34/s44) / sqrt( s33*s44 );
  double d19co34 = ( e(14) - e(19)*s34/s44) / sqrt( s33*s44 );
  
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
  
  B(4, 1) = e(14) - e(4);
  B(4, 2) = e(3)  - e(8);
  B(4, 3) = e(2)  - e(12);
  B(4, 4) = e(6) - e(1);
  B(4,6) = - B(4, 1);
  B(4,8) = - B(4, 3);
  B(4,12) = - B(4, 2);
  B(4,14) = - B(4, 4);
  
  B(5, 1) = e(19) - e(4);
  B(5, 2) = e(3)  - e(8);
  B(5, 3) = e(2)  - e(17);
  B(5, 4) = e(6) - e(1);
  B(5,6) = - B(5, 1);
  B(5,8) = - B(5, 3);
  B(5,17) = - B(5, 2);
  B(5,19) = - B(5, 4); 
  
  
  B(6, 3) = -p1*p2*d_3co12;
  B(6, 4) = -p1*p2*d_4co12;
  B(6, 8) = -p1*p2*d_8co12;
  B(6, 9) = -p1*p2*d_9co12;
  B(6, 5) = d_5sp12 - p2*co12;
  B(6,10) = d10sp12 - p1*co12;
  
  B(7, 3) = -p3*p4*d13co34;
  B(7, 4) = -p3*p4*d14co34;
  B(7, 8) = -p3*p4*d18co34;
  B(7, 9) = -p3*p4*d19co34;
  B(7, 5) = d15sp34 - p4*co34;
  B(7,10) = d20sp34 - p3*co34;  
  
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
// after one iteration of the contrained fit.  
//==================================================================
StatusCode LagrangeGeomVertexFitter::itera(HepVector& e, HepSymMatrix& Ce,
                                           int dimCe, bool final) 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << " ITERA !!! " << endreq;
  
  int ntracks = m_ntracks;
  HepMatrix B;
  HepVector cte;
  
  if ( ntracks == 2) {
    HepMatrix B2(ntracks-1,dimCe,0);
    HepVector cte2(ntracks-1);
    
    B = B2;
    cte = cte2;
  }
  
  if ( ntracks == 3) {
    HepMatrix B3(ntracks,dimCe,0);
    HepVector cte3(ntracks);
    
    B = B3;
    cte = cte3;
  }
  
  
  if ( ntracks == 4) {
    HepMatrix B4(ntracks+1,dimCe,0);
    HepVector cte4(ntracks+1);
    
    B = B4;
    cte = cte4;
  }  
  
  // Independent constant vector in the Taylor expansion of the
  // constrained equation. 
  
  if ( ntracks == 2 ){
    cte(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));    
  }
  
  if ( ntracks == 3 ){
    cte(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));    
    cte(3) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));    
  }
  
  if ( ntracks == 4 ){
    cte(1) = (e(6)  - e(1))*(e(4) - e(9))  - (e(7)  - e(2))*(e(3) - e(8));
    cte(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));  
    cte(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));      
    cte(4) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    cte(5) = (e(6) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));     
  }
  
  log << MSG::DEBUG << "lagrange constrained equations " << endreq;
  log << MSG::DEBUG << cte << endreq;
  
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
    B(3, 1) = e(14) - e(4);
    B(3, 2) = e(3)  - e(8);
    B(3, 3) = e(2)  - e(12);
    B(3, 4) = e(6) - e(1);
    B(3,6) = -B(3,1);
    B(3,8) = -B(3,3);
    B(3,12) = -B(3,2);
    B(3,14) = -B(3,4);  
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
    
    B(4, 1) = e(14) - e(4);
    B(4, 2) = e(3)  - e(8);
    B(4, 3) = e(2)  - e(12);
    B(4, 4) = e(6) - e(1);
    B(4,6) = - B(4, 1);
    B(4,8) = - B(4, 3);
    B(4,12) = - B(4, 2);
    B(4,14) = - B(4, 4);
    
    B(5, 1) = e(19) - e(4);
    B(5, 2) = e(3)  - e(8);
    B(5, 3) = e(2)  - e(17);
    B(5, 4) = e(6) - e(1);
    B(5,6) = - B(5, 1);
    B(5,8) = - B(5, 3);
    B(5,17) = - B(5, 2);
    B(5,19) = - B(5, 4); 
  }
  
  //  Du = Ce*Bt
  
  HepMatrix Bt=B.T();
  
  HepMatrix Du = Ce*Bt;
  
  //  Dv = B*Ce*Bt
  HepSymMatrix  Dv =Ce.similarity(B);
  
  //  Dvi = Dv inverse
  
  log << MSG::DEBUG << " Dv = " << endreq;
  log << MSG::DEBUG << Dv << endreq;
  
  int inv;
  HepSymMatrix Dvi = Dv.inverse(inv);
  
  log << MSG::DEBUG << " Dvi = " << Dvi <<
    endreq;
  
  if ( inv != 0 ){
    log << MSG::DEBUG << "Dv inverse failure" << endreq;     
    return StatusCode::FAILURE;
  }
  
  //  Dw = Du*Dvi = Ce*Bt * (B*Ce*Bt)i
  
  HepMatrix Dw = Du*Dvi;
  
  //  de = Ce*Bt * (B*Ce*Bt)i * cte
  
  HepVector de = Dw*cte;
  
  //    log << MSG::DEBUG << "de - track parameters variation " << endreq;
  
  //    log << MSG::DEBUG << de << endreq;
  
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
// It returns the invariant sub mass at this step ( = submassConstrCalc ) and 
// the difference in the estimated z-vertex ( = diffZVer ).
//==================================================================

StatusCode LagrangeGeomVertexFitter::evalu(HepVector& e, 
                                           const ParticleVector& pDaughter,
                                           double&  subMassConstrCalc, 
                                           HepVector&  diffZVer)
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " EVALU !!! " << endreq;
  
  int ntracks = m_ntracks;
  
  Particle* daughter1 = pDaughter[0];
  Particle* daughter2 = pDaughter[1];
  
  double dm1 = daughter1->mass();
  double dm2 = daughter2->mass();  
  double p1  = e(5);
  double p2  = e(10);  
  
  subMassConstrCalc = 0.0;
  diffZVer(6,0);
  
  double p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2) );
  double s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
  double s12 = e(3)*e(8) + e(4)*e(9) + 1.0;   
  double co12 = s12 / sqrt( s11*s22 );
  
  subMassConstrCalc = pow(dm1,2) + pow(dm2,2) + 2.*sqrt(p12) - 2.*p1*p2*co12;
  subMassConstrCalc = sqrt(subMassConstrCalc);
  
  if ( ntracks == 3) {
    
    diffZVer(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZVer(1) = fabs(diffZVer(1));
    
    diffZVer(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZVer(2) = fabs(diffZVer(2));
    
    diffZVer(3) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    diffZVer(3) = fabs(diffZVer(3));
    
  } // if ntracks = 3
  
  if ( ntracks == 4) {
    
    diffZVer(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZVer(1) = fabs( diffZVer(1));
    
    diffZVer(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZVer(2) = fabs( diffZVer(2));
    
    diffZVer(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));
    diffZVer(3) = fabs( diffZVer(3));
    
    diffZVer(4) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    diffZVer(4) = fabs( diffZVer(4));
    
    diffZVer(5) = (e(6) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(8));
    diffZVer(5) = fabs( diffZVer(5));
    
  } // if ntracks = 4
  
  
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: evalu ( 2 ressonances )  
// This method evaluates how well the constrained equations are satisfied
// at a given iteration. This method works for the case of 4 
// tracks with two ressonances. 
// It returns the invariant sub masses at this step ( = subMass1ConstrCalc 
// and subMass2ConstrCalc ) and the difference in the estimated 
// z-vertex ( = diffZVer ).
//==================================================================

StatusCode LagrangeGeomVertexFitter::evalu(HepVector& e, 
                                           const ParticleVector& pDaughter1,
                                           const ParticleVector& pDaughter2,
                                           double&  subMass1ConstrCalc, 
                                           double&  subMass2ConstrCalc,
                                           HepVector&  diffZVer)					   
  
  
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " EVALU !!! " << endreq;
  
  int ntracks = m_ntracks;
  
  Particle* daughter1 = pDaughter1[0];
  Particle* daughter2 = pDaughter1[1];
  Particle* daughter3 = pDaughter2[0];
  Particle* daughter4 = pDaughter2[1];
  
  double dm1 = daughter1->mass();
  double dm2 = daughter2->mass();
  double dm3 = daughter3->mass();
  double dm4 = daughter4->mass();
  double p1 = e(5);
  double p2 = e(10);
  double p3 = e(15);
  double p4 = e(20);  
  
  subMass1ConstrCalc = 0.0;
  subMass2ConstrCalc = 0.0;
  diffZVer(6,0);
  
  double   p12 = ( pow(p1,2) + pow(dm1,2) ) * ( pow(p2,2) + pow(dm2,2) );
  double   s11 = pow(e(3),2) + pow(e(4),2) + 1.0;
  double   s22 = pow(e(8),2) + pow(e(9),2) + 1.0;
  double   s12 = e(3)*e(8) + e(4)*e(9) + 1.0;   
  double   co12 = s12 / sqrt( s11*s22 );
  
  subMass1ConstrCalc = pow(dm1,2) + pow(dm2,2) + 2.*sqrt(p12) - 
    2.*p1*p2*co12;
  subMass1ConstrCalc = sqrt(subMass1ConstrCalc);
  
  double   p34 = (pow(p3,2) + pow(dm3,2)) * (pow(p4,2) + pow(dm4,2));
  double   s33 = pow(e(13),2) + pow(e(14),2) + 1.0;
  double   s44 = pow(e(18),2) + pow(e(19),2) + 1.0;
  double   s34 = e(13)*e(18) + e(14)*e(19) + 1.0;
  double   co34 = s34 / sqrt( s33*s44);
  
  subMass2ConstrCalc = pow(dm3,2) + pow(dm4,2) + 2.*sqrt(p34) - 2.*p3*p4*co34;
  subMass2ConstrCalc = sqrt(subMass2ConstrCalc);
  
  if ( ntracks == 4) {
    
    diffZVer(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZVer(1) = fabs( diffZVer(1));
    
    diffZVer(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZVer(2) = fabs( diffZVer(2));
    
    diffZVer(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));
    diffZVer(3) = fabs( diffZVer(3));
    
    diffZVer(4) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    diffZVer(4) = fabs( diffZVer(4));
    
    diffZVer(5) = (e(6) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(8));
    diffZVer(5) = fabs( diffZVer(5));
    
  } // if ntracks = 4
  
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: evalu ( without ressonances )  
// This method evaluates how well the constrained equations are satisfied
// at a given iteration. This method works for the case of 2, 3 or 4 
// tracks without ressonances. 
// It returns the difference in the estimated z-vertex ( = diffZVer ).
//==================================================================

StatusCode LagrangeGeomVertexFitter::evalu(HepVector& e, HepVector&  diffZVer)
{
  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << " EVALU !!! " << endreq;
  
  int ntracks = m_ntracks;
  
  diffZVer(6,0);
  
  if ( ntracks == 2 ) {   
    diffZVer(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZVer(1) = fabs( diffZVer(1) );
  }
  
  if ( ntracks == 3 ){ 
    
    diffZVer(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZVer(1) = fabs(diffZVer(1));
    
    diffZVer(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZVer(2) = fabs(diffZVer(2));
    
    diffZVer(3) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    diffZVer(3) = fabs(diffZVer(3));
    
  }
  
  if ( ntracks == 4) {   
    
    diffZVer(1) = (e(6) - e(1))*(e(4) - e(9)) - (e(7) - e(2))*(e(3) - e(8));
    diffZVer(1) = fabs( diffZVer(1));
    
    diffZVer(2) = (e(11) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(13));
    diffZVer(2) = fabs( diffZVer(2));
    
    diffZVer(3) = (e(16) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(18));
    diffZVer(3) = fabs( diffZVer(3));
    
    diffZVer(4) = (e(6) - e(1))*(e(4) - e(14)) - (e(12) - e(2))*(e(3) - e(8));
    diffZVer(4) = fabs( diffZVer(4));
    
    diffZVer(5) = (e(6) - e(1))*(e(4) - e(19)) - (e(17) - e(2))*(e(3) - e(8));
    diffZVer(5) = fabs( diffZVer(5));  
    
  }
  
  return StatusCode::SUCCESS;
}










//==================================================================
// Method name: chisq
// This method computes the Chisquare of the contrained fit. 
//==================================================================
StatusCode LagrangeGeomVertexFitter::chisq(HepVector& e0, HepVector& e,
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
StatusCode LagrangeGeomVertexFitter::nwcov(HepVector& mass, HepVector& e, 
                                           HepSymMatrix& Ce,
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
  
  // In order to get the mother energy, obtain each daughter's energy
  
  HepLorentzVector p1( 0., 0., 0., 0. );
  HepLorentzVector p2( 0., 0., 0., 0. );
  HepLorentzVector p3( 0., 0., 0., 0. );
  HepLorentzVector p4( 0., 0., 0., 0. );
  p1.setPz( e(5)/sqrt(pow(e(3),2)+pow(e(4),2)+1.0));
  p1.setPx( e(3)*p1.pz());
  p1.setPy( e(4)*p1.pz());
  p1.setE( sqrt(pow(mass(1),2)+pow(p1.px(),2)+
                pow(p1.py(),2) +
                pow(p1.pz(),2)));
  
  p2.setPz( e(10)/sqrt(pow(e(8),2)+pow(e(9),2)+1.0));
  p2.setPx( e(8)*p2.pz());
  p2.setPy( e(9)*p2.pz());
  p2.setE( sqrt(pow(mass(2),2)+pow(p2.px(),2)+
                pow(p2.py(),2) +
                pow(p2.pz(),2)));
  if (ntracks == 3) {
    p3.setPz( e(15)/sqrt(pow(e(13),2)+pow(e(14),2)+1.0));
    p3.setPx( e(13)*p3.pz());
    p3.setPy( e(14)*p3.pz());
    p3.setE( sqrt(pow(mass(3),2)+pow(p3.px(),2)+
                  pow(p3.py(),2) +
                  pow(p3.pz(),2)));
  }
  if (ntracks == 4) {
    p4.setPz( e(20)/sqrt(pow(e(18),2)+pow(e(19),2)+1.0));
    p4.setPx( e(18)*p4.pz());
    p4.setPy( e(19)*p4.pz());
    p4.setE( sqrt(pow(mass(4),2)+pow(p4.px(),2)+
                  pow(p4.py(),2) +
                  pow(p4.pz(),2)));
  }
  
  tpf(7) = p1.e()+p2.e();
  if (ntracks == 3) tpf(7) += p3.e();
  if (ntracks == 4) tpf(7) += p3.e()+p4.e();   
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

